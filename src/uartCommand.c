/*
 * File:   uartCommand.c
 * 
 * �L���ڑ����̃^�}���j����̃R�}���h
 * UART�o�R
 * 
 * 
 * Revision history: 
 * 2024.03.15   main.c��蕪��
 * 
 */
#include "header.h"
#include "uartCommand.h"

//uint32_t    uartBaudrate = 9600;  //RS485�{�[���[�g
uint32_t    uartBaudrate = 115200;

void uartCommand_Init(void){
    UART_SERIAL_SETUP   rs485set;
    
    rs485set.baudRate = uartBaudrate;
    rs485set.parity = UART_PARITY_NONE;
    rs485set.dataWidth = UART_DATA_8_BIT;
    rs485set.stopBits = UART_STOP_1_BIT;
    UART1_SerialSetup(&rs485set, CPU_CLOCK_FREQUENCY >> 1);  //PBCLK2:60MHz
}
    

void uartComandCheck(void){
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
    
    debuggerComand(buf);
    tamamoniCommandCheck(buf);
    
    UART1_ErrorGet();
    //buffer clear
    for(i = 0; i < BUF_NUM; i++){
        buf[i] = 0;
    }
    
}
    
    
void debuggerComand(uint8_t* buf){
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
        printf("> invalid command!\n");
    }

}


/* *****************************************************************************
 End of File
 */
