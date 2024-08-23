/*
 * File:   PCF8574.c
 * 
 * PCF8574 I2C I/O Expander 
 * 
 * 3.3V I2C 8ch
 * 
 * Revision history: 
 * 2024.01.14   
 * 
 * 
 */
#include "header.h"
#include "PCF8574.h"


#define     PCF8574_ID      0x20        //0x20~0x27

//���W�X�^��1�����Ȃ̂ŁA�A�h���X�͖���

#define     IO_SETTING      0b00000000  //���̓|�[�g:1������(100uA�̃v���A�b�v���L���ɂȂ�)

//local
static  uint8_t portRegister = IO_SETTING;



bool    PCF8574_Init(void)
{
    //initialize
    //ret value: 1:error, 0:OK
    
    //input port set
    printf("PCF8574 init ");

    if (i2c1_WriteRegister(PCF8574_ID, IO_SETTING))
    {
        printf("error!\n");
        return ERROR;
    }
    printf("OK\n");
    
    return OK;
}
    

//
void    ledLightOn(port_name_t color)
{
    //LED���I��
    //������x�ɂ�肽������or�ŕ��ׂ�
    uint8_t tmp;
    if (NO_OUTPUT == color)
    {
        //�����I�����Ȃ���
        return;
    }
    
    tmp = portRegister | color;
    i2c1_WriteRegister(PCF8574_ID, tmp);
    
}


void    ledLightOff(port_name_t color)
{
    //LED���I�t
    uint8_t tmp;
    if (NO_OUTPUT == color)
    {
        //�����I�t���Ȃ���
        return;
    }
    
    tmp = portRegister & (!color);
    i2c1_WriteRegister(PCF8574_ID, tmp);
    
}


//****** sub ************************************************


