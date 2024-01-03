/*
 * File:   iP5306.c
 * Comments: PIC32MK-MCJ Harmony
 * 
 * iP5306�@���`�E���C�I���o�b�e���[�d�A�ی�A5V�u�[�X�g�o�́A�X�C�b�`�@�@I2C
 * 
 * BatV - 3.3V convert  I2C
 * 
 * Revision history: 
 * 2024.01.03
 * 
 */


#include "header.h"
#include "iP5306.h"


bool ip5306_Init(void){
    //I2C iP5306 init
#define DEBUG_no
    
#define IP5306_SLAVE_ID     0x75    //iP5306 Li battery charger & booster
#define REG_SYS_CTL0        0x00    //default 0x35 -> 0x31
    uint8_t boost = 0b1;            //[bit5]�u�[�X�g�o��     1:����, 0:���Ȃ� 
    uint8_t charge = 0b1;           //[bit4]�[�d�@�\        1:�L��, 0:����,
    uint8_t autoOn = 0b0;           //[bit2]�I�[�g�I��      1:����, 0:���Ȃ�       ----*
    uint8_t alwaysBoost = 0b0;      //[bit1]�펞�u�[�X�g     0:���Ȃ�, 1:�I��
    uint8_t pushSwOff = 0b1;        //[bit0]�v�b�V��SW�I�t   1:��, 0:�s��
#define REG_SYS_CTL1        0x01    //default 0x1D -> 0xD9
    uint8_t boostLongPush = 0b1;    //[bit7]�u�[�X�g�I�t     0:�_�u��, 1:������     ----*
    uint8_t leddoubleclick = 0b1;   //[bit6]LED���C�g       0:������, 1:�_�u������ ----* 
    uint8_t shortPushOff = 0b0;     //[bit5]�Z�����I�t       0:���Ȃ�, 1:����
    uint8_t usbCutBoost = 0b0;      //[bit2]USB�����u�[�X�g�o�� 1:�I��, 0:�I�t�@ (��.�u�₵�Ă��܂�)
    uint8_t lowVoltage = 0b1;       //[bit0]3V�ȉ��I�t       1:����@0:���̂܂� 
#define REG_SYS_CTL2        0x02    //default 0x64 -> 0x74
    uint8_t longPushTime = 0b1;     //[bit4]���������� �@�@�@�@0:2sec, 1:3sec      ----*
    uint8_t autoOffTime = 0b01;     //[bit3:2]�I�[�g�I�t����  01:32sec, 00:8s, 10:16s, 11:64s
    
    uint8_t ip5306TxData[4];
    uint8_t ip5306RxData[3];

    //�f�t�H���g�l�ǂݏo��
    printf("iP5306 init ");
    if (i2c1_ReadDataBlock(IP5306_SLAVE_ID, REG_SYS_CTL0, ip5306RxData, 3)){
        printf("error!\n");
        return ERROR;
    }
#ifdef DEBUG
    printf("\n(I2C 0x%02X reg 0x%02X\n", IP5306_SLAVE_ID, REG_SYS_CTL0);
    printf("read %02X %02X %02X) ", ip5306RxData[0], ip5306RxData[1], ip5306RxData[2]);
#endif

    //iP5306 initialize
    ip5306TxData[0] = (boost << 5) + (charge << 4) + (autoOn << 2) + (alwaysBoost << 1) + pushSwOff;   
    ip5306TxData[1] = (boostLongPush << 7) + (leddoubleclick << 6) + (shortPushOff << 5) + (usbCutBoost << 2) + lowVoltage;
    ip5306TxData[2] = (longPushTime << 4) + (autoOffTime << 2);
                                        
    if (i2c1_WriteDataBlock(IP5306_SLAVE_ID, REG_SYS_CTL0, &ip5306TxData[0], 3)){
        printf("error!\n");
        return ERROR;
    }
    printf("OK\n");
    return OK;
}


bool ip5306_ReadStatus(uint8_t* data0x70){
    //I2C read
#define REG_STATUS  0x70
    uint8_t i2cRxData [9];
    uint8_t charge_enable;  //0x70
    uint8_t charge_status;  //0x71
    uint8_t bat_level;      //0x78
    uint8_t batZan = 100;   //%
    
    if (i2c1_ReadDataBlock(IP5306_SLAVE_ID, REG_STATUS, i2cRxData, 9)){
        printf("error!\n");
        return ERROR;
    }
    
    //
    charge_enable = i2cRxData[0];
    charge_status = i2cRxData[1];
    //key_status = i2cRxData[7];
    bat_level = i2cRxData[8];

    //BAT level
    if (bat_level & 0b10000000){
        batZan = 75;
    }
    if (bat_level & 0b01000000){
        batZan = 50;
    }
    if (bat_level & 0b00100000){
        batZan = 25;
    }
    printf("(%3d%%)\n", batZan);
    data0x70[0] = (batZan / 25);        //[bit2:0]
    
#if DEBUG1
    printf("cE %02X ", charge_enable);
    printf("cS %02X ", charge_status);
    printf("bV %02X ", bat_level);
    printf("\n");
#endif
    
    if (bat_level & 0b00010000){
        printf("batV < 3.2V \n");
        data0x70[0] |= 0b00001000;      //[bit3]
    }
    if (bat_level & 0b00000010){
        printf("batV < 3.0V \n");
        data0x70[0] |= 0b00010000;      //[bit4] 
    }

    //Charge status change
    if (charge_enable & 0b00001000){
        //USB IN
        data0x70[0] |= 0b00100000;  //[bit5] 
        if (usb_in_flag == 0){
            printf("USB IN-");
            usb_in_flag = 1;
        }
    }else{
        CHARGE_LED_RED_Clear();
        if (usb_in_flag == 1){
            printf("USB Out\n");
        }
        usb_in_flag = 0;
    }

    if (usb_in_flag == 1){
        if (charge_status & 0b00001000){
            printf("Full\n");
            data0x70[0] |= 0b01000000;  //[bit6] 
            CHARGE_LED_RED_Clear();
            if (sleep_flag == POWERSAVING_SLEEP){
                sleep_flag = POWERSAVING_DEEPSLEEP;
                LED_BLUE_Clear();
                RTCC_InterruptDisable(RTCC_INT_ALARM);//RTCC ���荞�ݒ�~
                printf("RTCC Interrupt off\n");
                printf("-- PIC DEEP SLEEP --\n");
                CORETIMER_DelayMs(500);
                deep_sleep();
                ///////////////////////// DEEP SLEEP ////////////////////////////////
            }
        }else{
            printf("CHARGE\n");
            CHARGE_LED_RED_Set();
        }
    }
    return OK;
}

