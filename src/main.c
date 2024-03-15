/*******************************************************************************
 * Electric TARGET #2 V5
 *      PIC32MK0512MCJ064  3.3V  120MHz
 *          Xtal 8MHz       2nd Xtal 32.768kHz
 * 
 * V5   2号機
 *      マイク5個
 *      Comparator: 5 used
 *      InputCapture: 5 used
 *      データはI2Cマスターとして、ESP32S3(7"LCD Yellow board)　slaveへ送信
 * 
 *      5つのセンサから、3つのセンサを選んで座標を計算 -> 10とおりの結果
 *      センサ1を抜いた時の結果は4とおり -> 4点のばらつきを計算。
 *      同様にセンサ2を抜いた時のばらつき、
 *      。。。センサ5を抜いた時のばらつき、
 *      の5とおりのうちの一番ばらつきが小さいものを採用する。
 * 
 * DEBUGGer: 9600bps　-> 115200bps (2024.01.17 ~)
 * タマモニ有線のため　また9600bpsへ (2024.02.25 ~)
 *                 また -> 115200bpsに (2024.02.28 ~)
 *
 * 
 * Main.c
 * 
 * 2023.12.17
 *  
 * 2023.12.20   v0.10   USB充電時の長押しオフに見えるようにスリープモードを設定
 * 2023.12.24   v0.12   I2Cの修正、バッテリーと充電時スリープ機能の整理
 * 2023.12.24   v0.13   ESP32からの読み出し用関数
 * 2024.01.03   v0.14   BMP280気温気圧センサ追加
 *                       Compiler optimization level -0  for DEBUG
 * 2024.01.06   v0.20   電源まわり完了　(espスリープ)
 * 2024.01.08   v0.21   コンパレータ入力　測定部の修正
 * 2024.01.12   v0.22   分散の計算。ばらつき最小グループを採用。
 * 2024.01.14   v0.23   正面LEDインジケータのためI2C I/Oエキスパンダ追加。
 * 2024.01.17   v0.24   マトLCDへの表示成功。UARTデバッガ出力を9600->115200bpsへ
 * 2024.01.19   v0.25   Harmony -> MCCへ
 * 2024.01.20   v0.30   計算間違い修正　z[1]のところz[0]と間違えていたためccが900ほど違っていて計算座標がずれていた。(1号機_V4も修正済み)
 * 2024.01.21   v0.31   フルデバッグモード..計算経過を追加
 * 2024.01.21   v0.40   センサ3個が一直線に並んでいる時(e=0)の計算式を追加。
 * 2024.01.27   v0.41   センサ一直線の時x0が2つ求まるので、判定式を追加。センサ1の球r#1+r0の長さがほぼ(x0,y0)と(x#1,y#1)の距離に等しい、ことから差が小さい方を選択する。
 * 2024.01.30   v0.50   整理整頓
 * 2024.02.25   v0.51   有線部分の作成:
 *                       ブランチv051-wired-connect
 * 2024.02.25   v0.52   有線接続の時、ターゲットオフセット分をタマモニへのデータに加算しなくてはいけない。(オフセットデータを持っていない)
 *                       有線接続の時、タマモニからのターゲットコマンド(UART)を解釈してESPへI2Cコマンドで送る。
 *                       通信不良はタマモニのエラッタのせいだった。対策の配線が間違っていた。
 * 2024.02.28   v0.53   タマモニ有線接続&DEBUGger 　　9600bps -> 115200bps
 * 2024.03.10   v0.55   PT1_ESP (センサ1 ESP-NOW無線より) --> EXT2/PT1_ESP
 *                       VideoSYNC --> CLC2 CLC3ENで　有線と無線(OCMP PWM)切り替え  
 * 2024.03.12   v0.56   pt1 無線の時の遅れ時間測定 PT1_DELAY_TEST
 * 2024.03.13   v0.57   OCMP(PWM)テスト
 * 2024.03.14   v0.58c  videoSYNC デバッグ サブルーチン化 ファイル分離
 * 
 * 
 * 
 */

#include "header.h"


//debug
#define FULL_DEBUG_MODE_no
#ifdef  FULL_DEBUG_MODE
debugger_mode_sour_t    debuggerMode = FULL_DEBUG;
#else
debugger_mode_sour_t    debuggerMode = NONE;
#endif

//PT1 delay TEST
#define PT1_DELAY_TEST//_no                 //PT1無線の遅れ時間測定テスト


//Global
uint16_t        ringPos = 0;            //ログデータポインタ
uint8_t         sensorCnt;              //センサ入力順番のカウント  ////////////////////////
float           targetY0Offset = 0;     //ターゲットYオフセット
pt1con_sor_t    pt1ConWiFi = WIRED_LAN; 


//local
const uint8_t fw_ver[] = "0.58";    //firmware version
bool        pt1Esp_Flag = 0;        //PT1(無線)割込
bool        pt1_Flag = 0;           //PT1(有線)割込
bool        timer1secFlag = 0;      //RTCC 1秒割込
uint8_t     timerCount = 0;         //タイマーカウンタ
uint8_t     ledTimer = 0;           //着弾後のLED表示時間


#ifdef  PT1_DELAY_TEST
uint32_t    pt1TimerLogStart = 0;      //PT1ディレイ測定タイマー
uint32_t    pt1TimerLogEnd = 0;
#endif


//--- callback ----------------------------------------------------------

void pt1Esp_callback(EXTERNAL_INT_PIN pin, uintptr_t context){
    //無線もしくはV0センサー接続なし(TARGET ONLY)の時のPT1入力
    pt1Esp_Flag = 1;
    if (pt1ConWiFi == WIRELESS_WIFI){
        TMR2 = 0;
        videoSync_Start();      //PWMスタート
    }
}


void pt1Lan_callback(EXTERNAL_INT_PIN pin, uintptr_t context){
    //有線のときのPT1入力
    pt1_Flag = 1;
    if (pt1ConWiFi == WIRED_LAN){
        TMR2 = 0;
    }
}


void timer1sec_callback(uintptr_t context){
    timer1secFlag = 1;
}



//--- main ---------------------------------------------------------------

int main ( void ){
    
    uint16_t            videoFps;           //ビデオフレームレート

    
    //Initialize all modules
    SYS_Initialize ( NULL );
    
    //power on
    ESP_POWER_Set();        //ESP32 5V LoadSwitch
    ANALOG_POWER_Set();     //Analog 3.3V LDO
    
    //Pin Interrupt
    ICAP1_CallbackRegister(detectSensor1, 0);
    ICAP2_CallbackRegister(detectSensor2, 0);
    ICAP3_CallbackRegister(detectSensor3, 0);
    ICAP4_CallbackRegister(detectSensor4, 0);
    ICAP5_CallbackRegister(detectSensor5, 0);

    //Main SW interrupt INT4
    EVIC_ExternalInterruptCallbackRegister(EXTERNAL_INT_4, mainSwOn_callback, 0);
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_4);
    //PT1_ESP(wifi) interrupt INT2
    EVIC_ExternalInterruptCallbackRegister(EXTERNAL_INT_2, pt1Esp_callback, 0);
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_2);
    //PT1(wired) interrupt INT3
    EVIC_ExternalInterruptCallbackRegister(EXTERNAL_INT_3, pt1Lan_callback, 0);
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_3);
    
    //RTCC1秒ごと割込
    RTCC_CallbackRegister(timer1sec_callback, 0);
    
    //I2C Init
    I2C1_CallbackRegister(MyI2CCallback, 0);    //NULL);
    
    //UART
    uartCommand_Init();
    
    //pwm_test();

    //start up
    CORETIMER_DelayMs(1000);
    
    printf("\n");
    printf("********************\n");
    printf("* electric TARGET  *\n");
    printf("*  #2  PIC32MK     *\n");
    printf("*        ver.%s  *\n", fw_ver);
    printf("********************\n");
#ifdef  FULL_DEBUG_MODE
    printf(">full debug mode\n");
#else
    printf(">no output mode\n");
#endif
    printf("mode change .... R\n");
    printf("target clear ... C\n");
    printf("--- INIT -----------\n");
    
    LED_BLUE_Clear();
    CORETIMER_DelayMs(200);
    LED_BLUE_Set();
    CORETIMER_DelayMs(100);
    LED_BLUE_Clear();
    CORETIMER_DelayMs(200);
    LED_BLUE_Set();
    CORETIMER_DelayMs(100);
    LED_BLUE_Clear();
    
    //I2Cセンサ類初期設定
    ip5306_Init();      //Li-ion Battery charger&booster
    BME280_Init();      //BMP280 temp&pressure
    ESP32slave_Init();  //LCD&WiFi
    PCF8574_Init();     //IOexpander LED
    
    //comparator DAC
    uint16_t    compVth = 100;//mV  CDAconverter/////////////////////////////////////////
    uint16_t    val_12bit;
    val_12bit = 4096 / 3300 * compVth;
    printf("Vth=%5dmV:%03x(12bit)\n", compVth, val_12bit);
    CDAC2_DataWrite(val_12bit);
    
    //batV init
    batteryVolt(1);    //initialize
    
   //comparator ON
    CMP_1_CompareEnable();
    CMP_2_CompareEnable();
    CMP_3_CompareEnable();
    CMP_4_CompareEnable();
    CMP_5_CompareEnable(); 
      
    //video SYNC init
    VIDEO_SYNC_PWM();        //startup - WiFi無線接続
    videoFps = 30;
    videoSync_Init(videoFps);
    
    printf("--------------------\n");
    printf("\n");
    ////  hardware init complete ///////////////////////////////
    
    //uart buffer clear
    while(UART1_ReceiverIsReady()) {
        UART1_ReadByte();
    }
    
    measureInit();
    clearData(); 
    pt1Esp_Flag = 0;
    pt1_Flag = 0;
 
    
//**** MAIN LOOP ********************************************** 
    while ( true )
    {
        //Maintain state machines of all polled MPLAB Harmony modules.
        SYS_Tasks ( );
        
        impact();   //着弾処理

        
        
        if (TMR2 >= 180000000U){    //60MHz x 3sec = 180,000,000count
            TMR2 = 0;       //3秒以上たったらタイマクリア。タイマは4秒までカウント
            pt1_Flag = 0;
            pt1Esp_Flag = 0;
        }
            
        
        //玉発射を検出していない時
        if (!pt1_Flag && !pt1Esp_Flag){ 
            uartComandCheck();
            oneSecRoutine();
            mainSwPush();
        }
        
    }
    /* Execution should not come here during normal operation */
    return ( EXIT_FAILURE );
    
}



//  subroutine ///

void impact(void){
    //着弾後の処理
    uint8_t             cnt;
    meas_stat_sor_t     measStat;
    uint16_t            shotCnt = 0;        //ショットカウントは1から。0は入力無し
 
    if (sensorCnt == 0){
        //センサー入力なし
        return;
    }
    
    //着弾
    LED_BLUE_Set();
    
    //測定完了待ち
    cnt = 80;
    while(cnt > 0){
        CORETIMER_DelayUs(10);      //つづいての入力信号を待つ時間 10us x 80 = 800usec
        cnt--;
        if (sensorCnt >= NUM_SENSOR){
            break;
        }
    }
    
    //測定停止
    ICAP1_Disable();
    ICAP2_Disable();
    ICAP3_Disable();
    ICAP4_Disable();
    ICAP5_Disable();        //入力がなかった時もあるはずなので止める
    TMR2_Stop();
    videoSync_Stop();       //PWM stop
    impact_PT4_Off();       //着弾センサ出力オフ->ESP経由WiFiでタマモニへいく信号

    //データ処理
    shotCnt++;
    ringPos++;
    if (ringPos > LOG_MEM){
        //ログメモリポインタ制限
        ringPos = 0;
    }

    measStat = measureMain(shotCnt);
    serialPrintResult(shotCnt, measStat, debuggerMode);
    //log_data_make(shot_count);    //////////////////////////////////////////////////////////////////////
#ifdef PT1_DELAY_TEST
    printf("PT1 ESP-NOW delay %6dusec\n", (pt1TimerLogEnd - pt1TimerLogStart) * 1000000 / TMR2_FrequencyGet());
#endif    
            
    //次の測定のための準備
    clearData();
    pt1_Flag = 0;
    pt1Esp_Flag = 0;
    ledTimer = 1;   //消灯までのタイマースタート

    //処理完了
    LED_BLUE_Clear();

}


void oneSecRoutine(void){
#define LED_INDICATE_TIME   6   //sec
    
    
    if (!timer1secFlag){
        return;
    }
    
    //1秒ごとカウント　0~7
    timerCount++;
    if (timerCount > 7){
        timerCount = 0;
    } 
    
    switch (timerCount){    //0 ~ 7
        case 0:
            LED_BLUE_Set();
            ESP32slave_SendBatData();
            LED_BLUE_Clear();
            break;
            
        case 1:
            if(POWERSAVING_NORMAL == sleepStat){
                BME280_ReadoutSM();
            }
            break;
            
        case 3:
            //PT1接続チェック ... LAN or WiFi切替
            if (PT1_Get() == 1){
                //H:有線
                if (pt1ConWiFi != WIRED_LAN){
                    printf("PT1-Hi: Wired LAN\n");
                    VIDEO_SYNC_Wired();
                }
            }else{
                //L:無線
                if (pt1ConWiFi != WIRELESS_WIFI){
                    printf("PT1-Lo: WiFi ESP\n");
                    VIDEO_SYNC_PWM();
                }
            }
            break;
    } 
    
    //着弾表示LED消灯タイマー
    if (ledTimer){
        ledTimer++;
        if (ledTimer >= LED_INDICATE_TIME){
            //正面LEDを消灯
            ledLightOff(LED_BLUE | LED_YELLOW | LED_PINK);
            ledTimer = 0;   //停止
        }
    }
      
}


/*******************************************************************************
 End of File
*/

