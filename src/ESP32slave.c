/*
 * File:   ESP32slave.c
 * Comments: PIC32MK-MCJ Harmony
 * 
 * ESP32S3 Yellow Board LCD�@�@I2C slave
 * 
 * 3.3V I2C
 * 
 * Revision history: 
 * 2024.01.03
 * 
 */

#include "header.h"
#include "ESP32slave.h"

//DEBUGger printf (Global...header.h)
//#define DEBUG_ESP_SLAVE_0_no    //Debug�pprintf(�G���[�֘A)
//#define DEBUG_ESP_SLAVE_2_no    //Debug�pprintf(�o�b�e���d��)
//#define DEBUG_ESP_SLAVE_3_no    //Debug�pprintf(���M�f�[�^)
    

#define ESP_SLAVE_ID      0x25  //I2C ID ESP32S3 Yellow Board LCD&WiFi
//register address
typedef enum  {
    REG_ID_CHECK        = 0x01, //ID�m�F�@�ԓ�"ESP"
    ID_REPLY            = 0x32, //�ԓ��f�[�^    
    REG_TARGET_CLEAR    = 0x08, //�^�[�Q�b�g�N���A�R�}���h
    REG_TARGET_DEFAULT  = 0x09, //�^�[�Q�b�g�f�t�H���g�Z�b�g�R�}���h
    REG_TARGET_OFFSET   = 0x0a, //�^�[�Q�b�gY�I�t�Z�b�g�R�}���h
    REG_TARGET_AIMPOINT = 0x0b, //�^�[�Q�b�g�_�_�Z�b�g�R�}���h
    REG_TARGET_BRIGHT   = 0x0c, //�^�[�Q�b�g�P�x�Z�b�g�R�}���h
    REG_TARGET_SLEEP    = 0x0d, //�^�[�Q�b�g�X���[�v�R�}���h
    DATA_SLEEP_KEY      = 0x99, //�X���[�v��key
            
    REG_DATA_IMPACT     = 0x20, //���e�f�[�^�𑗂�
    REG_DATA_PT1CON     = 0x2c, //PT1�L���A�������؂�ւ�������ɑ���
    REG_DATA_TEMP       = 0x2d, //���x�f�[�^�𑗂�
    REG_DATA_BAT        = 0x2f, //�o�b�e���[�f�[�^�𑗂�
            
    REG_DATA_MESSAGE    = 0x50, //�e�L�X�g�f�[�^�𑗂�
 
} esp_register_t;


bool ESP32slave_Init(void)
{
    //ESP32 I2C slave ID check   
#define DEBUG32_1//_no
    
    uint8_t i2cRxData [4] = {0,0,0,0};  //�G���h�}�[�N���܂�
    uint8_t reply[] = "ESP";
    
    printf("ESP32S3 init ");
    if(i2c1_ESP32ReadDataBlock(ESP_SLAVE_ID, 0x01, i2cRxData, 3))
    {
        printf("error!\n");
        return ERROR;
    }
    
    if (strcmp((char*)i2cRxData, (char*)reply) != 0)
    {
        //�ԓ��s��v
        printf("error!\n");
        return ERROR;
    }
#ifdef  DEBUG32_1
    printf("\n(detect '%s').", i2cRxData);   
#endif
    printf("OK\n"); 
    return OK;
}


//**** TARGET command **************************************************

bool ESP32slave_ClearCommand(void)
{  
    //ESP32�փ^�[�Q�b�g�N���A
    printf("> target clear\n");
    if (i2c1_Write1byteRegister(ESP_SLAVE_ID, REG_TARGET_CLEAR, 0))
    {
#ifdef DEBUG_ESP_SLAVE_0
        printf("ESPslave error!\n");
#endif
        return ERROR;
    }
    return OK;
}


/*
bool ESP32slave_DefaultSetCommand(void)
{  
    //ESP32�փ^�[�Q�b�g�f�t�H���g�Z�b�e�B���O
    printf("> target default set \n");
    if (i2c1_Write1byteRegister(ESP_SLAVE_ID, REG_TARGET_DEFAULT, 0))
    {
#ifdef DEBUG_ESP_SLAVE_0
        printf("ESPslave error!\n");
#endif
        return ERROR;
    }
    return OK;
}


bool ESP32slave_OffSetCommand(float offSet)
{  
    //ESP32�փI�t�Z�b�g�R�}���h
    uint8_t val;
    
    if ((offSet < -40) || (offSet > 35))
    {
        printf("error (-40~35) \n");
        return ERROR;
    }
    //1�o�C�g���l�ɕϊ�
    val = (uint8_t)(offSet + 128);
    printf("> target Y offset %4d \n", val - 128);
    if (i2c1_Write1byteRegister(ESP_SLAVE_ID, REG_TARGET_OFFSET, val))
    {
#ifdef DEBUG_ESP_SLAVE_0
        printf("ESPslave error!\n");
#endif
        return ERROR;
    }
    return OK;
}


bool ESP32slave_AimpointCommand(float aimPoint)
{  
    //ESP32�֑_�_�Z�b�g�R�}���h
    if ((aimPoint < 30) || (aimPoint > 120))
    {
        printf("error (30~120) \n");
        return ERROR;
    }
    //1�o�C�g���l�ɕϊ�
    printf("> aimpoint set %3.0f \n", aimPoint);
    if (i2c1_Write1byteRegister(ESP_SLAVE_ID, REG_TARGET_AIMPOINT, (uint8_t)aimPoint))
    {
#ifdef DEBUG_ESP_SLAVE_0
        printf("ESPslave error!\n");
#endif
        return ERROR;
    }
    return OK;   
}


bool ESP32slave_BrightnessCommand(float brightness)
{  
    //ESP32�֋P�x�Z�b�g�R�}���h
    if ((brightness < 0) || (brightness > 250))
    {
        printf("error (0~250) \n");
        return ERROR;
    }
    printf("> LCD backlight brightness set %3.0f \n", brightness);
    if (i2c1_Write1byteRegister(ESP_SLAVE_ID, REG_TARGET_BRIGHT, (uint8_t)brightness))
    {
#ifdef DEBUG_ESP_SLAVE_0
        printf("ESPslave error!\n");
#endif
        return ERROR;
    }
    return OK;   
    
    
}
*/


//**** DATA send **************************************************

bool ESP32slave_SendImpactData(uint8_t* dataToEsp, uint8_t len)
{
    //���e�f�[�^�𑗐M
    //len:�o�C�g��
    //����O�̕ϐ��̌^�͌Ăяo���̍�(uint8_t*)�ŃL���X�g����΁@�Ȃ�ł�OK�B

    if (i2c1_WriteDataBlock(ESP_SLAVE_ID, REG_DATA_IMPACT, &dataToEsp[0], len))
    {
#ifdef DEBUG_ESP_SLAVE_0
        printf("ESPslave error!\n");
#endif
        return ERROR;
    }
    return OK;
}
    
/*  �|�C���^�ϐ��𗧂ĂȂ��Ă��ł���̂ŁA���̂����͖��ʂ����ǃ|�C���^�𗝉�����̂ɂ͗ǂ������B
bool ESP32slave_SendImpactData(float* impactData, uint8_t len){
    //���e�f�[�^�𑗐M
    //float 4�o�C�g�ϐ���1�o�C�g���Ƃ̔z��ɕϊ�
    uint8_t     dataToEsp[len];
    uint8_t*    pointerByte;    //�|�C���^�ϐ� - uint8_t�Ȃ̂�1�o�C�g���i��
    uint8_t     i;
    
    pointerByte = (uint8_t*)&impactData[0];
    for (i = 0; i < len; i++){
        dataToEsp[i] = pointerByte[i];
    }

    if (i2c1_WriteDataBlock(ESP_SLAVE_ID, REG_DATA_IMPACT, &dataToEsp[0], len)){
        printf("ESPslave error!\n");
        return ERROR;
    }
    return OK;
}
*/


bool ESP32slave_SendPT1Connect(uint8_t pt1ConnectFrom)
{
    //PT1�ڑ��X�e�[�^�X��ESP32�֑���
    //pt1ConnectFrom..0:�L��LAN, 1:����WIFI (pt1con_sor_t)
    uint8_t     dataToEsp[1];

    dataToEsp[0] = pt1ConnectFrom;
    if (i2c1_WriteDataBlock(ESP_SLAVE_ID, REG_DATA_PT1CON, &dataToEsp[0], 1))
    {
#ifdef DEBUG_ESP_SLAVE_0
        printf("ESPslave error!\n");
#endif
        return ERROR;
    }
    return OK;
}


bool ESP32slave_SendTempData(uint32_t temp)
{
    //�C���f�[�^��ESP32�֑���
    //temp:���x�f�[�^
    uint8_t     dataToEsp[2];

    dataToEsp[0] = (uint8_t)(temp >> 8) & 0xff;
    dataToEsp[1] = (uint8_t)temp & 0xff;

    if (i2c1_WriteDataBlock(ESP_SLAVE_ID, REG_DATA_TEMP, &dataToEsp[0], 2))
    {
#ifdef DEBUG_ESP_SLAVE_0
        printf("ESPslave error!\n");
#endif
        return ERROR;
    }
    return OK;
}


bool ESP32slave_SendBatData(uint8_t* dataToEsp) 
{
    //�o�b�e���d���v����ESP32�֑���
    //init=1:����
    
    float       batV;
        
    batV = batteryVolt(0);
#ifdef DEBUG_ESP_SLAVE_2
    printf("\nBatV:%4.2fV", batV);
#endif
    batV *= 1000;            //1000�{�l

    ip5306_ReadStatus(&dataToEsp[0]);
    
    //write to ESP32slave
    if (sleepStat != POWERSAVING_NORMAL)
    {
        return OK;
    }
    //dataToEsp[0]-> bat status data ip5306_read_status�Ŏ擾
    dataToEsp[1] = ((uint16_t)batV >> 8) & 0xff;
    dataToEsp[2] = (uint16_t)batV & 0xff;

    if (i2c1_WriteDataBlock(ESP_SLAVE_ID, REG_DATA_BAT, &dataToEsp[0], 3))
    {
#ifdef DEBUG_ESP_SLAVE_0
        printf("ESPslave error!\n");
#endif
        return ERROR;
    }
#ifdef DEBUG_ESP_SLAVE_3
    printf("to ESP(%02X)- %02X %02X %02X\n", REG_DATA_BAT, dataToEsp[0], dataToEsp[1], dataToEsp[2]);
#endif
    return OK;
}


bool ESP32slave_SendMessage(uint8_t* textStr) 
{
    //�e�L�X�g���b�Z�[�W��ESP32�֑���BESP����LCD�ɕ\��    
    if (i2c1_WriteDataBlock(ESP_SLAVE_ID, REG_DATA_MESSAGE, textStr, strlen((char*)textStr)))
    {
#ifdef DEBUG_ESP_SLAVE_0
        printf("ESPslave error!\n");
#endif
        return ERROR;
    }

    return OK;   
}



//**** sleep *******************************************************************

bool ESP32slave_SleepCommand(void)
{  
    //ESP32�փX���[�v�v��

    if (i2c1_Write1byteRegister(ESP_SLAVE_ID, REG_TARGET_SLEEP, DATA_SLEEP_KEY))
    {
#ifdef DEBUG_ESP_SLAVE_0
        printf("ESPslave error!\n");
#endif
        return ERROR;
    }
    return OK;
}


