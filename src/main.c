/*******************************************************************************
 * Electric TARGET #2 V5
 *      PIC32MK0512MCJ064  3.3V  120MHz
 *          Xtal 8MHz       2nd Xtal 32.768kHz
 * 
 * V5   2���@
 *      �}�C�N5��
 *      Comparator: 5 used
 *      InputCapture: 5 used
 *      �f�[�^��I2C�}�X�^�[�Ƃ��āAESP32S3(7"LCD Yellow board)�@slave�֑��M
 * 
 *      5�̃Z���T����A3�̃Z���T��I��ō��W���v�Z -> 10�Ƃ���̌���
 *      �Z���T1�𔲂������̌��ʂ�4�Ƃ��� -> 4�_�̂΂�����v�Z�B
 *      ���l�ɃZ���T2�𔲂������̂΂���A
 *      �B�B�B�Z���T5�𔲂������̂΂���A
 *      ��5�Ƃ���̂����̈�Ԃ΂�������������̂��̗p����B
 * 
 * DEBUGGer: 9600bps�@-> 115200bps (2024.01.17 ~)
 * �^�}���j�L���̂��߁@�܂�9600bps�� (2024.02.25 ~)
 *                 �܂� -> 115200bps�� (2024.02.28 ~)
 *
 * 
 * Main.c
 * 
 * 2023.12.17
 *  
 * 2023.12.20   v0.10   USB�[�d���̒������I�t�Ɍ�����悤�ɃX���[�v���[�h��ݒ�
 * 2023.12.24   v0.12   I2C�̏C���A�o�b�e���[�Ə[�d���X���[�v�@�\�̐���
 * 2023.12.24   v0.13   ESP32����̓ǂݏo���p�֐�
 * 2024.01.03   v0.14   BMP280�C���C���Z���T�ǉ�
 *                       Compiler optimization level -0  for DEBUG
 * 2024.01.06   v0.20   �d���܂�芮���@(esp�X���[�v)
 * 2024.01.08   v0.21   �R���p���[�^���́@���蕔�̏C��
 * 2024.01.12   v0.22   ���U�̌v�Z�B�΂���ŏ��O���[�v���̗p�B
 * 2024.01.14   v0.23   ����LED�C���W�P�[�^�̂���I2C I/O�G�L�X�p���_�ǉ��B
 * 2024.01.17   v0.24   �}�gLCD�ւ̕\�������BUART�f�o�b�K�o�͂�9600->115200bps��
 * 2024.01.19   v0.25   Harmony -> MCC��
 * 2024.01.20   v0.30   �v�Z�ԈႢ�C���@z[1]�̂Ƃ���z[0]�ƊԈႦ�Ă�������cc��900�قǈ���Ă��Čv�Z���W������Ă����B(1���@_V4���C���ς�)
 * 2024.01.21   v0.31   �t���f�o�b�O���[�h..�v�Z�o�߂�ǉ�
 * 2024.01.21   v0.40   �Z���T3���꒼���ɕ���ł��鎞(e=0)�̌v�Z����ǉ��B
 * 2024.01.27   v0.41   �Z���T�꒼���̎�x0��2���܂�̂ŁA���莮��ǉ��B�Z���T1�̋�r#1+r0�̒������ق�(x0,y0)��(x#1,y#1)�̋����ɓ������A���Ƃ��獷������������I������B
 * 2024.01.30   v0.50   ��������
 * 2024.02.25   v0.51   �L�������̍쐬:
 *                       �u�����`v051-wired-connect
 * 2024.02.25   v0.52   �L���ڑ��̎��A�^�[�Q�b�g�I�t�Z�b�g�����^�}���j�ւ̃f�[�^�ɉ��Z���Ȃ��Ă͂����Ȃ��B(�I�t�Z�b�g�f�[�^�������Ă��Ȃ�)
 *                       �L���ڑ��̎��A�^�}���j����̃^�[�Q�b�g�R�}���h(UART)�����߂���ESP��I2C�R�}���h�ő���B
 *                       �ʐM�s�ǂ̓^�}���j�̃G���b�^�̂����������B�΍�̔z�����Ԉ���Ă����B
 * 2024.02.28   v0.53   �^�}���j�L���ڑ�&DEBUGger �@�@9600bps -> 115200bps
 * 2024.03.10   v0.55   PT1_ESP (�Z���T1 ESP-NOW�������) --> EXT2/PT1_ESP
 *                       VideoSYNC --> CLC2 CLC3EN�Ł@�L���Ɩ���(OCMP PWM)�؂�ւ�  
 * 2024.03.12   v0.56   pt1 �����̎��̒x�ꎞ�ԑ��� PT1_DELAY_TEST
 * 2024.03.13   v0.57   OCMP(PWM)�e�X�g
 * 2024.03.14   v0.58d  videoSYNC �f�o�b�O �T�u���[�`���� �t�@�C������ PT1�ʔ��ˌ��o
 * 2024.03.15   v0.59   PT1�^�C���A�E�g�����AvideoSYNC�^�C���A�E�g����
 * 2024.03.16   v0.60   PT1ESP�����ڑ����̒x�ꎞ�ԑ��� PT1_DELAY_TEST
 * 2024.03.17   v0.61   PT1�ڑ���f�[�^��ESP32��I2C���M�B(�R�}���h���W�X�^�ǉ�)
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
#define PT1_DELAY_TEST//_no                 //PT1�����̒x�ꎞ�ԑ���e�X�g


//Global
uint16_t        ringPos = 0;            //���O�f�[�^�|�C���^
uint8_t         sensorCnt;              //�Z���T���͏��Ԃ̃J�E���g  ////////////////////////
float           targetY0Offset = 0;     //�^�[�Q�b�gY�I�t�Z�b�g
pt1con_sor_t    pt1ConWiFi = UNKNOWN; 


//local
const uint8_t fw_ver[] = "0.61";    //firmware version
bool        pt1Esp_Flag = 0;        //PT1(����)����
bool        pt1_Flag = 0;           //PT1(�L��)����
bool        timer1secFlag = 0;      //RTCC 1�b����
#define     LED_INDICATE_TIME   3   //+1sec
uint8_t     ledTimer = 0;           //���e���LED�\������


#ifdef  PT1_DELAY_TEST
uint32_t    pt1EspDelayLog = 0;     //PT1�f�B���C����^�C�}�[
#endif


//--- callback ----------------------------------------------------------

void pt1Esp_callback(EXTERNAL_INT_PIN pin, uintptr_t context){
    //������������V0�Z���T�[�ڑ��Ȃ�(TARGET ONLY)�̎���PT1����
    pt1Esp_Flag = 1;
    if (pt1ConWiFi == WIRELESS_WIFI){
        TMR2 = 0;
        videoSync_Start();      //PWM�X�^�[�g
    }
#ifdef  PT1_DELAY_TEST
    pt1EspDelayLog = TMR2;
#endif
}


void pt1Lan_callback(EXTERNAL_INT_PIN pin, uintptr_t context){
    //�L���̂Ƃ���PT1����
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
    
    uint16_t            videoFps;           //�r�f�I�t���[�����[�g

    
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
    
    //RTCC1�b���Ɗ���
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
    
    //I2C�Z���T�ޏ����ݒ�
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
    
    //ext int
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_4);
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_2);
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_3);
      
    //video SYNC init
    VIDEO_SYNC_PWM();        //startup - WiFi�����ڑ�
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
        
        impact();   //���e����

        //3�b���ƂɃ^�C�}�[���Z�b�g
        if (TMR2 >= 180000000U){    //60MHz x 3sec = 180,000,000count
            TMR2 = 0;       //3�b�ȏソ������^�C�}�N���A�B�^�C�}��4�b�܂ŃJ�E���g
            videoSync_Stop();
            pt1_Flag = 0;
            pt1Esp_Flag = 0;
        }
            
        //�ʔ��˂����o���Ă��Ȃ���
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
    //���e��̏���
    uint8_t             cnt;
    meas_stat_sor_t     measStat;
    uint16_t            shotCnt = 0;        //�V���b�g�J�E���g��1����B0�͓��͖���
 
    if (sensorCnt == 0){
        //�Z���T�[���͂Ȃ�
        return;
    }
    
    //���e
    LED_BLUE_Set();
    
    //���芮���҂�
    cnt = 80;
    while(cnt > 0){
        CORETIMER_DelayUs(10);      //�Â��Ă̓��͐M����҂��� 10us x 80 = 800usec
        cnt--;
        if (sensorCnt >= NUM_SENSOR){
            break;
        }
    }
    
    //�����~
    ICAP1_Disable();
    ICAP2_Disable();
    ICAP3_Disable();
    ICAP4_Disable();
    ICAP5_Disable();        //���͂��Ȃ�������������͂��Ȃ̂Ŏ~�߂�
    TMR2_Stop();
    videoSync_Stop();       //PWM stop
    impact_PT4_Off();       //���e�Z���T�o�̓I�t->ESP�o�RWiFi�Ń^�}���j�ւ����M��

    //�f�[�^����
    shotCnt++;
    ringPos++;
    if (ringPos > LOG_MEM){
        //���O�������|�C���^����
        ringPos = 0;
    }

    measStat = measureMain(shotCnt);
    serialPrintResult(shotCnt, measStat, debuggerMode);
    //log_data_make(shot_count);    //////////////////////////////////////////////////////////////////////
#ifdef PT1_DELAY_TEST
    if (pt1ConWiFi == WIRED_LAN){
        CORETIMER_DelayMs(200);     //���e�f�[�^�Əd�Ȃ��ă^�}���j��Rx�G���[�ɂȂ�
        printf("\n");
        printf("PT1->ESP-NOW delay %9.3fusec\n", (float)pt1EspDelayLog * 1000000 / TMR2_FrequencyGet());
        CORETIMER_DelayMs(500);
    }
#endif    
            
    //���̑���̂��߂̏���
    clearData();
    pt1_Flag = 0;
    pt1Esp_Flag = 0;
    ledTimer = LED_INDICATE_TIME   //sec
;   //�����܂ł̃^�C�}�[�X�^�[�g

    //��������
    LED_BLUE_Clear();

}


void oneSecRoutine(void){
    static uint8_t  timerCount = 0;         //�^�C�}�[�J�E���^

    if (!timer1secFlag){
        return;
    }
    
    //1�b���ƃJ�E���g�@0~7
    timerCount++;
    timer1secFlag = 0;
    if (timerCount > 7){
        timerCount = 0;
    } 
    
    switch (timerCount){    //0 ~ 7
        case 0:
            LED_BLUE_Set();
            ESP32slave_SendBatData();
            LED_BLUE_Clear();
            break;
        case 3:
            pt1ConnectCheck();
            break;
        case 4:
            break;
        case 7:
            pt1ConnectCheck();
            break;
        default:    //= 1,2,5,6
            if(POWERSAVING_NORMAL == sleepStat){
                BME280_ReadoutSM();
            }
            break;
                
    } 
    
    //���e�\��LED�����^�C�}�[
    if (ledTimer){
        ledTimer--;
        if (ledTimer <= 0){
            //����LED������
            ledLightOff(LED_BLUE | LED_YELLOW | LED_PINK);
        }
    }
      
}


void pt1ConnectCheck(void){
    //PT1�ڑ��`�F�b�N ... LAN or WiFi�؂�ւ�
    if (PT1_Get() == 1){    
        //H:�L�� (idle High)
        if (pt1ConWiFi != WIRED_LAN){
            printf("PT1-Hi: Wired LAN\n");
            VIDEO_SYNC_Wired();
            ESP32slave_SendPT1Status((uint8_t)pt1ConWiFi);   //LCD�ւ̕\��
        }
    }else{
        //L:���� (�ڑ��Ȃ�)
        if (pt1ConWiFi != WIRELESS_WIFI){
            printf("PT1-Lo: WiFi ESP\n");
            VIDEO_SYNC_PWM();
            ESP32slave_SendPT1Status((uint8_t)pt1ConWiFi);   //LCD�ւ̕\��
        }
    }
}

/*******************************************************************************
 End of File
*/

