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
 * 2024.03.14   v0.58   videoSYNC デバッグ サブルーチン化
 * 
 * 
 * 
 */

#include "header.h"


//debug
debugger_mode_sour_t  debuggerMode = NONE;
//debugger_mode_sour_t    debuggerMode = FULL_DEBUG;
//PT1 delay TEST
#define PT1_DELAY_TEST//_no                 //PT1無線の遅れ時間測定テスト


//Global
uint16_t    ringPos = 0;            //ログデータポインタ
uint8_t     sensorCnt;              //センサ入力順番のカウント  ////////////////////////
float       targetY0Offset = 0;     //ターゲットYオフセット

//videoSYNC
#define     WiredLAN        1       
#define     WirelessWiFi    0

//local
const uint8_t fw_ver[] = "0.58";    //firmware version
bool        mainSwFlag = 0;         //メインスイッチ割込
bool        pt1Esp_Flag = 0;        //PT1(無線)割込
bool        pt1_Flag = 0;           //PT1(有線)割込
bool        timer1secFlag = 0;      //RTCC 1秒割込
bool        pt1Connect = 0;         //PT1..0:有線接続, 1:無線接続
//uint32_t    uartBaudrate = 9600;  //RS485ボーレート
uint32_t    uartBaudrate = 115200;
uint32_t    periodPWM;              //PWM period
uint32_t    onTimePWM;              //PWM On time
uint32_t    tmr4Pr;                 //Timer4 Priod - PWM reset

#ifdef  PT1_DELAY_TEST
uint32_t    pt1TimerLogStart = 0;      //PT1ディレイ測定タイマー
uint32_t    pt1TimerLogEnd = 0;
#endif


//--- callback ----------------------------------------------------------

void mainSwOn_callback(EXTERNAL_INT_PIN pin, uintptr_t context){
    //メインスイッチ
    mainSwFlag = 1;
}

void pt1Esp_callback(EXTERNAL_INT_PIN pin, uintptr_t context){
    //無線もしくはV0センサー接続なし(TARGET ONLY)の時のPT1入力
    pt1Esp_Flag = 1;
    if (pt1Connect == WirelessWiFi){
        videoSync_Start();      //PWMスタート
#ifdef PT1_DELAY_TEST
        pt1TimerLogEnd = TMR2;
#endif
    }
}


void pt1Lan_callback(EXTERNAL_INT_PIN pin, uintptr_t context){
    //有線のときのPT1入力
    pt1_Flag = 1;
#ifdef PT1_DELAY_TEST
    pt1TimerLogStart = TMR2;
#endif
}


void timer1sec_callback(uintptr_t context){
    timer1secFlag = 1;
}



//--- main ---------------------------------------------------------------

int main ( void ){
    uint8_t cnt;
    meas_stat_sor_t     measStat;
    uint16_t            shotCnt = 0;        //ショットカウントは1から。0は入力無し
    uint8_t             dispTimer = 0;
    uint8_t             ledTimer = 0;       //ステータスLEDを消すまでのタイマー
    uint8_t             pt1Timer = 4;       //有線接続チェックタイマー
    UART_SERIAL_SETUP   rs485set;
    uint16_t            videoFps;                //ビデオフレームレート

    
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
    rs485set.baudRate = uartBaudrate;
    rs485set.parity = UART_PARITY_NONE;
    rs485set.dataWidth = UART_DATA_8_BIT;
    rs485set.stopBits = UART_STOP_1_BIT;
    UART1_SerialSetup(&rs485set, CPU_CLOCK_FREQUENCY >> 1);  //PBCLK2:60MHz
    
    
    //video SYNC LED(Blue)  TEST//////////////////////////////////////////////////
    periodPWM = TMR4_PeriodGet();                      //333msec = 30fps
    onTimePWM = OCMP1_CompareSecondaryValueGet ();     //33msec - 10コマごと
    VIDEO_SYNC_PWM();        //startup - WiFi無線接続
    videoFps = 30;
    videoSync_Init(videoFps);


    pwm_test();

    //start up
    CORETIMER_DelayMs(1000);
    LED_BLUE_Clear();
    CORETIMER_DelayMs(200);
    LED_BLUE_Set();
    CORETIMER_DelayMs(100);
    LED_BLUE_Clear();
    CORETIMER_DelayMs(200);
    LED_BLUE_Set();
    CORETIMER_DelayMs(100);
    LED_BLUE_Clear();
    
    printf("\n");
    printf("********************\n");
    printf("* electric TARGET  *\n");
    printf("*  #2  PIC32MK     *\n");
    printf("*        ver.%s  *\n", fw_ver);
    printf("********************\n");
    printf(">full debug mode\n");    /////////デバッグ中
    printf("mode change .... R\n");
    printf("target clear ... C\n");
    printf("--- INIT -----------\n");

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
    
    printf("--------------------\n");
    printf("\n");
    //hardware init complete
    
    //uart buffer clear
    while(UART1_ReceiverIsReady()) {
        UART1_ReadByte();
    }
    
    //
    measureInit();
    clearData(); 
    pt1Esp_Flag = 0;
    pt1_Flag = 0;
    
//**** MAIN LOOP ********************************************** 
    while ( true )
    {
        //Maintain state machines of all polled MPLAB Harmony modules.
        SYS_Tasks ( );
        
        //
        // ((pt1_Flag    != 0) || (pt1Esp_Flag != 0))//PT1オン　= 玉発射 ---> タイムオーバー判定必要
        
        if (sensorCnt != 0){
            //センサー入力あり = 測定中
            LED_BLUE_Set();
            
            cnt = 80;
            while(cnt > 0){
                CORETIMER_DelayUs(10);      //つづいての入力信号を待つ時間 10us x 80 = 800usec
                cnt--;
                if (sensorCnt >= NUM_SENSOR){
                    break;
                }
            }
            
            ICAP1_Disable();
            ICAP2_Disable();
            ICAP3_Disable();
            ICAP4_Disable();
            ICAP5_Disable();        //入力がなかった時もあるはずなので止める
            TMR2_Stop();
            videoSync_Stop();       //PWM stop
            impact_PT4_Off();       //着弾センサ出力オフ->ESP経由WiFiでタマモニへいく信号
            
            shotCnt++;
            ringPos++;
            if (ringPos > LOG_MEM){
                //ログメモリポインタ制限
                ringPos = 0;
            }
            
            measStat = measureMain(shotCnt);
            serialPrintResult(shotCnt, measStat, debuggerMode);
            //log_data_make(shot_count);    //////////////////////////////////////////////////////////////////////
            
            //CORETIMER_DelayMs(100);     //printf表示処理が残っていて再トリガしてしまうので待つ////////////フルオート対応できない//////////////////////
            clearData();
            ledTimer = 0;
            pt1_Flag = 0;
            pt1Esp_Flag = 0;
            
            LED_BLUE_Clear();
            
        }else{       
            //(sensorCnt == 0)測定中ではない時
            
            uartComandCheck();
            
            //
            if (timer1secFlag ){
                //1秒ごと処理
                TMR2 = 0;       //1秒ごとタイマクリア。タイマは2秒までカウント。測定中はクリアされない。
                TMR2_Start();
                //
                timer1secFlag = 0;
                dispTimer++;
                ledTimer++;
                pt1Timer++;

                if ((dispTimer % 8) == 0){     //interval　8sec
                    LED_BLUE_Set();
                    ESP32slave_SendBatData();
                    LED_BLUE_Clear();
                }else if(POWERSAVING_NORMAL == sleepStat){
                    BME280_ReadoutSM();
                }
                
#define LED_INDICATE_TIME   6
                if (ledTimer >= LED_INDICATE_TIME){
                    //正面LEDを消灯
                    ledLightOff(LED_BLUE | LED_YELLOW | LED_PINK);
                }
                
                if ((pt1Timer % 4) == 0){
                    //PT1検出 ... LAN or WiFi切替
                    videoSync_Stop();
                    if (PT1_Get() == 1){
                        //H:有線
                        if (pt1Connect != WiredLAN){
                            printf("PT1-Hi: Wired LAN\n");
                            VIDEO_SYNC_Wired();
                        }
                    }else{
                        //L:無線
                        if (pt1Connect != WirelessWiFi){
                            printf("PT1-Lo: WiFi ESP\n");
                            VIDEO_SYNC_PWM();
                        }
                    }
                }
            }
            
            //
            if (mainSwFlag){
                //INT4　電源スイッチの処理
                mainSwPush();
                mainSwFlag = 0;
            }
#ifdef PT1_DELAY_TEST
            if ((pt1_Flag == 1) && (pt1Esp_Flag == 1)){
                printf("PT1 ESP-NOW delay %6dusec\n", (pt1TimerLogEnd - pt1TimerLogStart) * 1000000 / TMR2_FrequencyGet());
                pt1_Flag = 0;
                pt1Esp_Flag = 0;
            }
#endif    
        }
  
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


//----- sub --------------------------------------------------------------------

void uartComandCheck(void){
    //キー入力で表示モードを切り替え
#define     BUF_NUM     250     //UARTデータ読込バッファ数
    uint8_t buf[BUF_NUM];       //UARTデータ読込バッファ
    uint8_t i;
    
    if (!UART1_ReceiverIsReady()){
        return;
    }
    
    //シリアル(タマモニ、デバッガー)から受信あり
    for(i = 0; i < 5; i++){
        buf[i] = 0;
    }
    i = 0;
    while(UART1_ReceiverIsReady()) {
        buf[i] = UART1_ReadByte();
        if ((buf[i] == ',') || (buf[i] == 0)){ //end mark
            i++;
            buf[i] = 0;
            printf("buf='%s'\n", buf);
            CORETIMER_DelayMs(50);
            break;
        }
        i++;
        if (i > BUF_NUM){
            printf("uart command too long!\n");
            //buffer clear
            return;
        }
        CORETIMER_DelayUs(90);          //受信待ち 115200bps 1バイトデータは約78us
        if (uartBaudrate != 115200){
            CORETIMER_DelayUs(1000);    //受信待ち 9600bps 1バイトデータは約938us
        }
    }
    
    debuggerComand(buf);
    tamamoniCommandCheck(buf);
    
    UART1_ErrorGet();
    //buffer clear
    for(i = 0; i < BUF_NUM; i++){
        buf[i] = 0;
    }
    
}
    
    
void debuggerComand(uint8_t* buf){
    //DEBUGger出力モードの切り替え
    if (((buf[0] == 'R') | (buf[0] == 'r')) && ((buf[1] + buf[2] + buf[3] + buf[4]) == 0)){
        switch(debuggerMode){
            case NONE:
                debuggerMode = SINGLE_LINE;
                printf(">single line mode\n");
                break;
            case SINGLE_LINE:
                debuggerMode = MEAS_CALC;
                printf(">result mode\n");
                break;
            case MEAS_CALC:
                debuggerMode = FULL_DEBUG;
                printf(">full debug mode\n");
                break;
            case FULL_DEBUG:
                debuggerMode = CSV_DATA;
                printf(">csv output mode\n");
                printDataCSVtitle();
                break;
            case CSV_DATA:
                debuggerMode = NONE;
                printf(">no output mode\n");
                break;
            default:
                debuggerMode = NONE;    //failsafe
                break;
        }
    }else if (((buf[0] == 'C') | (buf[0] == 'c')) && ((buf[1] + buf[2] + buf[3] + buf[4]) == 0)){
        //ターゲットクリア
        printf(">target clear\n");
        ESP32slave_ClearCommand();
    }
    
}


// Tamamoni TARGET Command ------------------------------------------------------------------------------------------
void tamamoniCommandCheck(uint8_t* tmp_str) {
    //タマモニからのコマンドを確認し実行
    char command[10] = { 0 };  //9文字まで
    const char clear[] = "CLEAR";
    const char reset[] = "RESET";
    const char defaultSet[] = "DEFAULT";
    const char offset[] = "OFFSET";
    const char aimpoint[] = "AIMPOINT";
    const char brightness[] = "BRIGHT";
    float val = 0;
    uint8_t   num;

    num = sscanf((char*)tmp_str, "TARGET_%s %f END", command, &val);   //valのところの数字は無くても正常に動くみたい
    if (num == 0) {
        //型が合わなかったとき
        printf("'%s' is not command!\n", tmp_str);
        return;
    }                                                                     
    //コマンド
    printf("Detect TamaMoni command(%d) :%s   %f --- ", num, command, val);
    
    if (strcmp(clear, command) == 0) {
        ESP32slave_ClearCommand();
    } else if (strcmp(reset, command) == 0) {
        ESP32slave_ResetCommand();
    } else if (strcmp(defaultSet, command) == 0) {
        ESP32slave_DefaultSetCommand();
    } else if (strcmp(offset, command) == 0) {
        ESP32slave_OffSetCommand(val);
    } else if (strcmp(aimpoint, command) == 0) {
        ESP32slave_AimpointCommand(val);
    } else if (strcmp(brightness, command) == 0) {
        ESP32slave_BrightnessCommand(val);
    } else {
        printf("> invalid command!\n");
    }

}


void    videoSync_Init(uint8_t rate_fps){
    //フレームレートの設定
    //uint32_t tmr4Pr; -> global
    uint32_t d;
    
    if ((rate_fps <= 0) || (rate_fps > 100000)){
        printf("video %dfps error! --> 30fps", rate_fps);
        rate_fps = 30;
    }
    
    tmr4Pr = periodPWM * 30 / rate_fps;
    d = onTimePWM * 30 / rate_fps;
    
    OCMP1_Enable();
    TMR4_PeriodSet(tmr4Pr);
    OCMP1_CompareSecondaryValueSet(d);
    videoSync_Ready();
    
} 


void    videoSync_Ready(void){
    //タイマカウンタ値をセット　待機でL、スタートでHになるようにするため
    TMR4 = tmr4Pr;
}
    
void    videoSync_Start(void){
    //LED点滅点灯開始
    TMR4_Start();       //OCMP(PWM)start
}
    
void    videoSync_Stop(void){
    //LED消灯
    uint32_t    cnt = tmr4Pr << 1;
    
    while(VIDEO_SYNC_OUT_Get()){
        //Lになるのを待って止める
        cnt--;
        if (cnt <= 0){
            printf("PWMstop timeover!\n");
            break;
        }
    }
    TMR4_Stop();       //OCMP(PWM)stop
    videoSync_Ready();
    
}
    
void    VIDEO_SYNC_Wired(void){
    //Wired(LAN.8) TamamoniPWM
    pt1Connect = WiredLAN;
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_3);   //PT1 interrupt Enable
    CLC3_Enable(0);     //CLC2INA --> CLCO2 
}

void    VIDEO_SYNC_PWM(void){
    //OCMP1(PWM)
    pt1Connect = WirelessWiFi;
    EVIC_ExternalInterruptDisable(EXTERNAL_INT_3);  //PT1 interrupt Disable
    CLC3_Enable(1);     //OCMP1(PWM) --> CLCO2
}



////  T E S T  /////////////////////////

void pwm_test(void){
    
    printf("\n\n#### PWM TEST #####\n");
    bool st;
    st = CLC3CONbits.LCOUT;
    printf("CLC3(startup) - %d\n", st);
    
#define WIRED
#ifdef  WIRED
    VIDEO_SYNC_Wired();
    st = CLC3CONbits.LCOUT;
    printf("CLC3(LAN8) - %d\n", st);
#else
    VIDEO_SYNC_PWM();        //startup - WiFi無線接続
    st = CLC3CONbits.LCOUT;
    printf("CLC3(PWM) - %d\n", st);
#endif
    OCMP1_Enable ();    ////////////////////////////////
    uint32_t p = TMR4_PeriodGet();  //PWM period
    p = p * 3;  //1sec
    TMR4_PeriodSet(p);
    TMR4 = p;
    
    uint32_t  d = OCMP1_CompareSecondaryValueGet ();    //PWM On time
    d = d * 5 * 3;    //Duty 50%
    OCMP1_CompareSecondaryValueSet (d);
    
    printf("wait 2sec  \n");
    //TMR4 = 0;
    CORETIMER_DelayMs(2000);
    
    printf("PWM start\n");
    videoSync_Start();
    bool toggle = 1;
    
    while(1){
        if (pt1_Flag){
            //トグル
            printf("pt1:Hi...");
            if (toggle == 1){
                while(VIDEO_SYNC_OUT_Get());        //止めた時は必ずLになるのを待つ
                videoSync_Stop();
                printf("PWM stop  (TMR4 = %d)\n", TMR4_CounterGet());
                //init
                TMR4 = p;   //タイマカウンタ値をセット
                toggle = 0;
            }else{
                videoSync_Start();
                printf("PWM start\n");
                toggle = 1;
            }
            CORETIMER_DelayMs(500);
            pt1_Flag = 0;
        }
    }
    
}
            

/*******************************************************************************
 End of File
*/

