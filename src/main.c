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
 * 2023.12.20   v.0.10  USB�[�d���̒������I�t�Ɍ�����悤�ɃX���[�v���[�h��ݒ�
 * 2023.12.24   v.0.12  I2C�̏C���A�o�b�e���[�Ə[�d���X���[�v�@�\�̐���
 * 2023.12.24   v.0.13  ESP32����̓ǂݏo���p�֐�
 * 2024.01.03   v.0.14  BMP280�C���C���Z���T�ǉ�
 *                      Compiler optimization level -0  for DEBUG
 * 2024.01.06   v.0.20  �d���܂�芮���@(esp�X���[�v)
 * 2024.01.08   v.0.21  �R���p���[�^���́@���蕔�̏C��
 * 2024.01.12   v.0.22  ���U�̌v�Z�B�΂���ŏ��O���[�v���̗p�B
 * 2024.01.14   v.0.23  ����LED�C���W�P�[�^�̂���I2C I/O�G�L�X�p���_�ǉ��B
 * 2024.01.17   v.0.24  �}�gLCD�ւ̕\�������BUART�f�o�b�K�o�͂�9600->115200bps��
 * 2024.01.19   v.0.25  Harmony -> MCC��
 * 2024.01.20   v.0.30  �v�Z�ԈႢ�C���@z[1]�̂Ƃ���z[0]�ƊԈႦ�Ă�������cc��900�قǈ���Ă��Čv�Z���W������Ă����B(1���@_V4���C���ς�)
 * 2024.01.21   v.0.31  �t���f�o�b�O���[�h..�v�Z�o�߂�ǉ�
 * 2024.01.21   v.0.40  �Z���T3���꒼���ɕ���ł��鎞(e=0)�̌v�Z����ǉ��B
 * 2024.01.27   v.0.41  �Z���T�꒼���̎�x0��2���܂�̂ŁA���莮��ǉ��B�Z���T1�̋�r#1+r0�̒������ق�(x0,y0)��(x#1,y#1)�̋����ɓ������A���Ƃ��獷������������I������B
 * 2024.01.30   v.0.50  ��������
 * 2024.02.25   v.0.51  �L�������̍쐬:
 *                      �u�����`v051-wired-connect
 * 2024.02.25   v.0.52  �L���ڑ��̎��A�^�[�Q�b�g�I�t�Z�b�g�����^�}���j�ւ̃f�[�^�ɉ��Z���Ȃ��Ă͂����Ȃ��B(�I�t�Z�b�g�f�[�^�������Ă��Ȃ�)
 *                      �L���ڑ��̎��A�^�}���j����̃^�[�Q�b�g�R�}���h(UART)�����߂���ESP��I2C�R�}���h�ő���B
 *                      �ʐM�s�ǂ̓^�}���j�̃G���b�^�̂����������B�΍�̔z�����Ԉ���Ă����B
 * 2024.02.28   v.0.53  �^�}���j�L���ڑ�&DEBUGger �@�@9600bps -> 115200bps
 * 2024.03.10   v.0.55  PT1_ESP (�Z���T1 ESP-NOW�������) --> EXT2/PT1_ESP
 *                      VideoSYNC --> CLC2 CLC3EN�Ł@�L���Ɩ���(OCMP PWM)�؂�ւ�  
 *  
 * 
 * 
 */

#include "header.h"


//Global
uint16_t    ringPos = 0;            //���O�f�[�^�|�C���^
uint8_t     sensorCnt;              //�Z���T���͏��Ԃ̃J�E���g  ////////////////////////
float       targetY0Offset = 0;     //�^�[�Q�b�gY�I�t�Z�b�g

//debug
debugger_mode_sour_t  debuggerMode = NONE;
//debugger_mode_sour_t    debuggerMode = FULL_DEBUG;


//local
const uint8_t fw_ver[] = "0.55";    //firmware version
bool        mainSwFlag = 0;         //���C���X�C�b�`����
bool        pt1Esp_Flag = 0;        //PT1(����)����
bool        pt1_Flag = 0;           //PT1(�L��)����
bool        timer1secFlag = 0;      //RTCC 1�b����
//uint32_t    uartBaudrate = 9600;    //RS485�{�[���[�g
uint32_t    uartBaudrate = 115200;


//--- callback ----------------------------------------------------------

void mainSwOn_callback(EXTERNAL_INT_PIN pin, uintptr_t context){
    mainSwFlag = 1;
}

void pt1Esp_callback(EXTERNAL_INT_PIN pin, uintptr_t context){
    pt1Esp_Flag = 1;
    if (wifi�R�l�N�g){
        //�����̎�
        TMR4_Start();   //PWM�X�^�[�g
    }
}

void pt1Lan_callback(EXTERNAL_INT_PIN pin, uintptr_t context){
    pt1_Flag = 1;
}


void timer1sec_callback(uintptr_t context){
    timer1secFlag = 1;
}


//--- main ---------------------------------------------------------------

int main ( void ){
    uint8_t cnt;
    meas_stat_sor_t     measStat;
    uint16_t            shotCnt = 0;        //�V���b�g�J�E���g��1����B0�͓��͖���
    uint8_t             dispTimer = 0;
    uint8_t             ledTimer = 0;       //�X�e�[�^�XLED�������܂ł̃^�C�}�[
    uint8_t             pt1Timer = 0;       //�L���ڑ��`�F�b�N�^�C�}�[
    bool                pt1Wifi = 0;        //PT1..0:�L���ڑ�, 1:�����ڑ�
    UART_SERIAL_SETUP   rs485set;

    
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
    rs485set.baudRate = uartBaudrate;
    rs485set.parity = UART_PARITY_NONE;
    rs485set.dataWidth = UART_DATA_8_BIT;
    rs485set.stopBits = UART_STOP_1_BIT;
    UART1_SerialSetup(&rs485set, CPU_CLOCK_FREQUENCY >> 1);  //PBCLK2:60MHz
    
    //video SYNC LED(Blue)              ///////�T�u������//////////////////////////
#define WIRED_LAN       1   //LAN�P�[�u���L���ڑ�
#define WIFI_ESP_NOW    0   //ESP-NOW�����ڑ�
    CLC3_Enable(WIRED_LAN);
            
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
    printf(">full debug mode\n");    /////////�f�o�b�O��
    printf("mode change .... R\n");
    printf("target clear ... C\n");
    printf("--- INIT -----------\n");

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
    
    
//**** MAIN LOOP ********************************************** 
    while ( true )
    {
        //Maintain state machines of all polled MPLAB Harmony modules.
        SYS_Tasks ( );
        
        //
        if (sensorCnt != 0){
            //�Z���T�[���͂��� = ���蒆
            LED_BLUE_Set();
            
            cnt = 80;
            while(cnt > 0){
                CORETIMER_DelayUs(10);        //�Â��Ă̓��͐M����҂��� 10us x 80 = 800usec
                cnt--;
                if (sensorCnt >= NUM_SENSOR){
                    TMR4_Stop();            //PWM off
                    break;
                }
            }
            
            ICAP1_Disable();
            ICAP2_Disable();
            ICAP3_Disable();
            ICAP4_Disable();
            ICAP5_Disable();        //���͂��Ȃ�������������͂��Ȃ̂Ŏ~�߂�
            TMR2_Stop();
            TMR4_Stop();            //PWM stop
            impact_PT4_Off();       //���e�Z���T�o�̓I�t->�^�}���j�ւ����M��
            
            shotCnt++;
            ringPos++;
            if (ringPos > LOG_MEM){
                //���O�������|�C���^����
                ringPos = 0;
            }
            
            measStat = measureMain(shotCnt);
            serialPrintResult(shotCnt, measStat, debuggerMode);
            //log_data_make(shot_count);    //////////////////////////////////////////////////////////////////////
            
            //CORETIMER_DelayMs(100);     //printf�\���������c���Ă��čăg���K���Ă��܂��̂ő҂�////////////�t���I�[�g�Ή��ł��Ȃ�//////////////////////
            clearData();
            ledTimer = 0;
            
            LED_BLUE_Clear();
            
        }else{       
            //(sensorCnt == 0)���蒆�ł͂Ȃ���
            
            debuggerComand();
            
            //
            if (timer1secFlag ){
                //1�b���Ə���
                TMR2 = 0;       //1�b���ƃ^�C�}�N���A�B�^�C�}��2�b�܂ŃJ�E���g�B���蒆�̓N���A����Ȃ��B
                TMR2_Start();
                //
                timer1secFlag = 0;
                dispTimer++;
                ledTimer++;
                pt1Timer++;

                if ((dispTimer % 8) == 0){     //interval�@8sec
                    LED_BLUE_Set();
                    ESP32slave_SendBatData();
                    LED_BLUE_Clear();
                }else if(POWERSAVING_NORMAL == sleepStat){
                    BME280_ReadoutSM();
                }
                
#define LED_INDICATE_TIME   6
                if (ledTimer >= LED_INDICATE_TIME){
                    //����LED������
                    ledLightOff(LED_BLUE | LED_YELLOW | LED_PINK);
                }
                
                if ((pt1Timer % 8) == 4){
                    //PT1...LAN or WiFi
                    if (PT1_Get()){
                        //����
                        pt1Wifi = 0;
                    }else{
                        pt1Wifi = 1;
                    }
                }
            }
            
            //
            if (mainSwFlag){
                //INT4�@�d���X�C�b�`�̏���
                mainSwPush();
                mainSwFlag = 0;
            }
        }
  
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


//----- sub --------------------------------------------------------------------

void debuggerComand(void){
    //�L�[���͂ŕ\�����[�h��؂�ւ�
#define     BUF_NUM     250     //UART�f�[�^�Ǎ��o�b�t�@��
    uint8_t buf[BUF_NUM];       //UART�f�[�^�Ǎ��o�b�t�@
    uint8_t i;
    
    if (!UART1_ReceiverIsReady()){
        return;
    }
    
    //�V���A��(�^�}���j�A�f�o�b�K�[)�����M����
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
        CORETIMER_DelayUs(90);          //��M�҂� 115200bps 1�o�C�g�f�[�^�͖�78us
        if (uartBaudrate != 115200){
            CORETIMER_DelayUs(1000);    //��M�҂� 9600bps 1�o�C�g�f�[�^�͖�938us
        }
    }
    
    
    //DEBUGger�o�̓��[�h�̐؂�ւ�
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
        //�^�[�Q�b�g�N���A
        printf(">target clear\n");
        ESP32slave_ClearCommand();
    }else{
        //�R�}���h���
        tamamoniCommandCheck(buf);

        //printf(">invalid command!\n");
    }
    
    
    UART1_ErrorGet();
    //buffer clear
    for(i = 0; i < BUF_NUM; i++){
        buf[i] = 0;
    }
    
}


// Tamamoni TARGET Command ------------------------------------------------------------------------------------------
void tamamoniCommandCheck(uint8_t* tmp_str) {
    //�^�}���j����̃R�}���h���m�F�����s
    char command[10] = { 0 };  //9�����܂�
    const char clear[] = "CLEAR";
    const char reset[] = "RESET";
    const char defaultSet[] = "DEFAULT";
    const char offset[] = "OFFSET";
    const char aimpoint[] = "AIMPOINT";
    const char brightness[] = "BRIGHT";
    float val = 0;
    uint8_t   num;

    num = sscanf((char*)tmp_str, "TARGET_%s %f END", command, &val);   //val�̂Ƃ���̐����͖����Ă�����ɓ����݂���
    if (num == 0) {
        //�^������Ȃ������Ƃ�
        printf("'%s' is not command!\n", tmp_str);
        return;
    }                                                                     
    //�R�}���h
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
        //
        printf("> invalid command!\n");
    }

}




/*******************************************************************************
 End of File
*/

