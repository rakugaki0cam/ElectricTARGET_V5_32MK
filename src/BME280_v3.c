/*
 * File:   BME280_v3.c
 * 
 * Comments: PIC32MK-MCJ Harmony
 * 
 * BOSCH BME/BMP280 ���x�@���x�@�C���Z���T�[�@���W���[���@I2C
 * BMP280 (���x����)���W���[���ɑΉ�
 * MCC -> Harmony�ɑΉ�
 * 
 * 3.3V I2C
 * 
 * Revision history: 
 * 2024.01.02   _v3 BME280_v2���h�� �^�}���j�ŗL����(�\����)�����͏���
 * 
 */


#include "header.h"
#include "BME280_v3.h"


//BME/BMP280 I2C�ڑ�(CSB:VDD -> I2C)
#define     BME280_ID       0x76    //SDO:GND -> 0x76, VDD -> 0x77

//ascii code
#define     DEG             0xdf    // deg = '�'  �x�̔��p�J�i  
#define     DEG_C           0x8b    // degC = '��'  �̔��p�J�i  

//local
uint32_t    Temp_OUT_raw, Pres_OUT_raw, Humi_OUT_raw;
int32_t     Data_Temp_32;
uint32_t    Data_Pres_32,Data_Humi_32;
//float       Pr,Hu,Te;

typedef enum{
    BMP280,
    BME280,
} bm_type_t;
bm_type_t   bmeFlag = BMP280;
    
//
int32_t     t_fine;                 //�����C���@�C���␳�ɂ��g�p
uint16_t    dig_T1;                 //�Z���T�[�␳�l
int16_t     dig_T2;
int16_t     dig_T3;
 
uint16_t    dig_P1;
int16_t     dig_P2;
int16_t     dig_P3;
int16_t     dig_P4;
int16_t     dig_P5;
int16_t     dig_P6;
int16_t     dig_P7;
int16_t     dig_P8;
int16_t     dig_P9;
 
uint8_t     dig_H1;
int16_t     dig_H2;
uint8_t     dig_H3;
int16_t     dig_H4;   //12bit
int16_t     dig_H5;   //12bit
int8_t      dig_H6;


bool BME280_Init(void){
    //initialize
    //ret value: 1:error, 0:OK
#define DEBUG
    
#define REG_ID_CHECK    0xd0
#define ID_BME280_REP   0x60    //BME(P,T,H) return value
#define ID_BMP280_REP   0x58    //BMP(P,T) return value
    
    uint8_t i2cTxData [2];
    uint8_t i2cRxData [1] = {0};
    
    if(i2c1_Read1byteRegister(BME280_ID, REG_ID_CHECK, i2cRxData)){
        printf("BME/P280 error!\n");
        return ERROR;
    }
    switch(i2cRxData[0]){
        case ID_BME280_REP:
            printf("BME280 init ");
            bmeFlag = BME280;
            break;
        case ID_BMP280_REP:
            printf("BMP280 init ");
            bmeFlag = BMP280;
            break;
        default:
            printf("error!\n");
            return ERROR;
            break;
    }
    
    //config
#define REG_CTRL_HUM    0xF2    //Oversampling--BME280 only
    uint8_t osrs_h = 0b001;     //[bit2:0]Hum: 000:skip, 001:x1, 010:x2, 011:x4, 100:x8, 101:x16
#define REG_STAT        0xF3    //Status(Read only)[bit3]measurering, [bit0]im_update
#define REG_CTRL_MEAS   0xF4    //Oversampling & Mode
    uint8_t osrs_t = 0b010;     //[bit7:5]Temp: 000:skip, 001:x1, 010:x2, 011:x4, 100:x8, 101:x16
    uint8_t osrs_p = 0b011;     //[bit4:2]Press: 000:skip, 001:x1, 010:x2, 011:x4, 100:x8, 101:x16
    uint8_t mode = 0b11;        //[bit1:0]mode: 00:Sleep, 01:ForceMode, 11:NormalMode
#define REG_CONFIG      0xF5    //Config
    uint8_t t_sb = 0b101;       //[bit7:5]standby period(NormalMode) conversion time�͊܂܂Ȃ��B5(x1)~40(o.s. x16)msec������
                                // 000:0.5ms, 001:62.5ms 010:125ms, 011:250ms, 100:500ms, 101:1000ms,
                                //(BME) 110:10ms, 111:20ms
                                //(BMP) 110:2000ms, 111:4000ms
    uint8_t filter = 0b010;     //[bit4:2]IIRfilter 000:off, 001:2, 010:4, 011:8, 100:16
 
    //data
    i2cTxData[0] = (uint8_t)((osrs_t << 5) + (osrs_p << 2) + mode);
    i2cTxData[1] = (uint8_t)((t_sb << 5) + (filter << 2));    
    
    if (BME280 == bmeFlag){
        if (i2c1_Write1byteRegister(BME280_ID, REG_CTRL_HUM, osrs_h)){
            printf("error!\n");
            return ERROR; 
        }
    }
    if(i2c1_WriteDataBlock(BME280_ID, REG_CTRL_MEAS, &i2cTxData[0], 2)){
        printf("error!\n");
        return ERROR;
    }
    if (BME280_TrimRead()){
        printf("error!\n");
        return ERROR;
    }
    printf("OK\n");
    return OK;
}
           

bool BME280_TrimRead(void){
    //�␳�l�̓ǂݏo��
    
//�g�����p�����[�^�@�A�h���X
#define     TRIM_PARA_ADD1  0x88    //0x88~0x8d T1~T3:���x, 0x8e~0x9f P1~P9:�C��
//BME�̂�
#define     TRIM_PARA_ADD2  0xA1    //0xa1 H1:���x    
#define     TRIM_PARA_ADD3  0xE1    //0xe1�`0xe7 H2~H7:���x
    
    uint8_t     i2cRxData[24];
            
    if (i2c1_ReadDataBlock(BME280_ID, TRIM_PARA_ADD1, i2cRxData, 24)){
        printf("error\n");
        return ERROR;
    }
    dig_T1 = ((uint16_t)i2cRxData[1] << 8) | i2cRxData[0];          //0x88/0x89 -> dig_T1 [7:0]/[15:8]
    dig_T2 = ((int16_t)i2cRxData[3] << 8) | i2cRxData[2];           //0x8A/0x8B -> dig_T2 [7:0]/[15:8]
    dig_T3 = ((int16_t)i2cRxData[5] << 8) | i2cRxData[4];           //0x8C/0x8D -> dig_T3 [7:0]/[15:8]
    
    dig_P1 = ((uint16_t)i2cRxData[7] << 8) | i2cRxData[6];          //0x8E/0x8F -> dig_P1 [7:0]/[15:8]
    dig_P2 = ((int16_t)i2cRxData[9] << 8) | i2cRxData[8];           //0x90/0x91 -> dig_P2 [7:0]/[15:8]
    dig_P3 = ((int16_t)i2cRxData[11]<< 8) | i2cRxData[10];          //0x92/0x93 -> dig_P3 [7:0]/[15:8]
    dig_P4 = ((int16_t)i2cRxData[13]<< 8) | i2cRxData[12];          //0x94/0x95 -> dig_P3 [7:0]/[15:8]
    dig_P5 = ((int16_t)i2cRxData[15]<< 8) | i2cRxData[14];          //0x96/0x97 -> dig_P3 [7:0]/[15:8]
    dig_P6 = ((int16_t)i2cRxData[17]<< 8) | i2cRxData[16];          //0x98/0x99 -> dig_P3 [7:0]/[15:8]
    dig_P7 = ((int16_t)i2cRxData[19]<< 8) | i2cRxData[18];          //0x9A/0x9B -> dig_P3 [7:0]/[15:8]
    dig_P8 = ((int16_t)i2cRxData[21]<< 8) | i2cRxData[20];          //0x9C/0x9D -> dig_P3 [7:0]/[15:8]
    dig_P9 = ((int16_t)i2cRxData[23]<< 8) | i2cRxData[22];          //0x9E/0x9F -> dig_P3 [7:0]/[15:8]
    
    if (BMP280 == bmeFlag){
        return OK;
    }
    //Humid BME280only
    if (i2c1_Read1byteRegister(BME280_ID, TRIM_PARA_ADD2, i2cRxData)){
        printf("error!\n");
        return ERROR;
    }
    dig_H1 = (uint8_t)i2cRxData[0];                                         //0xA1           -> dig_H1 [7:0]
    
    if (i2c1_ReadDataBlock(BME280_ID, TRIM_PARA_ADD2, i2cRxData, 7)){
        printf("error!\n");
        return ERROR;
    }
    dig_H2 = ((int16_t)i2cRxData[1] << 8) | i2cRxData[0];                   //0xE1/0xE2      -> dig_H2 [7:0]/[15:8]
    dig_H3 = (uint8_t)i2cRxData[2];                                         //0xE3           -> dig_H3 [7:0]
    dig_H4 = ((int16_t)i2cRxData[3] << 4) | (i2cRxData[4] & 0x0F);          //0xE4/0xE5[3:0] -> dig_H4 [11:4]/[3:0] 12bit
    dig_H5 = ((int16_t)i2cRxData[5] << 4) | ((i2cRxData[4] & 0xF0) >> 4);   //0xE5[7:4]/0xE6 -> dig_H5 [3:0]/[11:4] 12bit
    dig_H6 = (int8_t)i2cRxData[6];                                          //0xE7           -> dig_H6 [7:0]
    
    return OK;
}


uint8_t BME280_Readout(void){
    //�ǂݏo���ƕ\��
    //ret: status
    
#define REG_PRESS   0xF7    //0xF7:[19:12], 0xF8:[11:4], 0xF9[bit7:4]:[3:0]
#define REG_TEMP    0xFA    //0xFA:[19:12], 0xFB:[11:4], 0xFC[bit7:4]:[3:0]
#define REG_HUMID   0xFD    //0xFD:[15:8], 0xFE:[7:0]
    
    uint8_t     i2cRxData[8];
    uint8_t     rxLen;
    
    typedef enum {
        BME_SM_IDLE,
        BME_SM_READ,
        BME_SM_TEMP,
        BME_SM_PRESS,
        BME_SM_HUMID,
        BME_SM_ESPSEND,
    } bme_status_t;
    static bme_status_t  status = BME_SM_IDLE;
    
    switch (status){
        case BME_SM_IDLE:
            
        case BME_SM_READ:
            //�f�[�^�ǂݍ���
            if (BME280 == bmeFlag){
                rxLen = 8;
            }else{
                rxLen = 6;
            }
            //printf("BME read");
            if(i2c1_ReadDataBlock(BME280_ID, REG_PRESS, i2cRxData, rxLen)){
                printf("error!\n");
                return ERROR;
            }
            //printf("OK\n");
            
            //�C���@�C���@���x�@�̏�
            Pres_OUT_raw = ((uint32_t)i2cRxData[0] << 12) | ((uint16_t)i2cRxData[1] << 4) | (i2cRxData[2] >> 4);    //�^�����Ă����Ȃ��ƁA�r�b�g�V�t�g�����ۂɂ���񂬂�Ă���
            Temp_OUT_raw = ((uint32_t)i2cRxData[3] << 12) | ((uint16_t)i2cRxData[4] << 4) | (i2cRxData[5] >> 4);
            if (BME280 == bmeFlag){
                Humi_OUT_raw = ((uint16_t) i2cRxData[6] << 8 ) | i2cRxData[7];
            }
            status = BME_SM_TEMP;
            break;
            
        case BME_SM_TEMP:
            //���x�̌v�Z
            Data_Temp_32 = BME280_CompensateT((int32_t)Temp_OUT_raw);
            // x0.01deg -40~85deg (-4000~0~8500 = 0xf060~0xffff,0x0~0x2134)
            printf("\ntemp %5.1f%cC\n", ((float)Data_Temp_32 / 100), DEG);
            status = BME_SM_PRESS;
            break;
            
        case BME_SM_PRESS:
            //�C���̌v�Z
            Data_Pres_32 = BME280_CompensateP((int32_t)Pres_OUT_raw);             ///user
            // x0.01hPa 300~1100hPa (0~110000 = 0x00~0x1adb0)
            printf("atm %6.1fhPa\n", ((float)Data_Pres_32 / 100));
            status = BME_SM_HUMID;
            break;
            
        case BME_SM_HUMID:
            //���x�̌v�Z
            if (BME280 == bmeFlag){
                Data_Humi_32 = BME280_CompensateH((int32_t)Humi_OUT_raw);             ///user
                // /1024 %RH  0~100%RH  (0~102400 = 0x0~0x19000)
                //printf("hum %3.0f%%\n", ((float)Data_Humi_32 / 1024));
            }
            status = BME_SM_ESPSEND;
            break;
            
        case BME_SM_ESPSEND:
            if (ESP32slave_WriteTempData(Data_Temp_32)){
                printf("Temp data send error!\n");
            }
            status = BME_SM_IDLE;
            break;
            
        default:
            break;
            
    }

#ifdef  TIMING_LOG              //debug
        DEBUG_timing_log(17);   //BMP280 return
#endif
        
    return status;
}


int32_t BME280_CompensateT(int32_t adc_T){
    //���x�␳�v�Z
    //"5123" = 51.23 deg C

    int32_t     var1, var2, T;
    
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;               //Fine Temperature (Global value)
    T = (t_fine * 5 + 128) >> 8;
    return T;

/*  
    double var1, var2, T; 
    var1 = (((double)adc_T) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
    var2 = ((((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0) * (((double)adc_T) / 131072.0 - ((double) dig_T1) / 8192.0)) * ((double)dig_T3);
    t_fine = (int32_t)(var1 + var2);
    T = (var1 + var2) / 5120.0;
    printf("%6.3f", T);
    return (int32_t)(T*100); 
*/     
}

uint32_t BME280_CompensateP(int32_t adc_P){
    //�C���̕␳�v�Z (t_fine���K�v)
    //"96386" = 963.86hPa
    int32_t     var1, var2;
    uint32_t    p;
    
    var1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)dig_P6);
    var2 = var2 + ((var1 * ((int32_t)dig_P5)) << 1);
    var2 = (var2 >> 2) + (((int32_t)dig_P4) << 16);
    var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)dig_P2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((int32_t)dig_P1)) >> 15);
    if (var1 == 0){
        //0�Ŋ��邱�ƂɂȂ����ꍇ �G���[
        return 000000;
    }    
    p = (((uint32_t)(((int32_t)1048576) - adc_P) - (uint32_t)(var2 >> 12))) * 3125;       //user
    if (p < 0x80000000){
        p = (p << 1) / ((uint32_t) var1);   
    } else {
        p = (p / (uint32_t)var1) * 2;    
    }
    var1 = (((int32_t)dig_P9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
    var2 = (((int32_t)(p >> 2)) * ((int32_t)dig_P8)) >> 13;
    p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));
    return p;
}

uint32_t BME280_CompensateH(int32_t adc_H){
    //���x�̕␳�v�Z (t_fine���K�v)
    //"47445" = 47445/1024 = 46.333%RH
    int32_t     v_x1;
    
    v_x1 = (t_fine - ((int32_t)76800));
    v_x1 = (((((adc_H << 14) -(((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1)) + 
              ((int32_t) 16384)) >> 15) * (((((((v_x1 * ((int32_t)dig_H6)) >> 10) * 
              (((v_x1 * ((int32_t)dig_H3)) >> 11) + ((int32_t) 32768))) >> 10) + ((int32_t) 2097152)) * 
              ((int32_t) dig_H2) + 8192) >> 14));
   v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
   
   // [(����)? a : b]���Z�q�̓R���p�C������Ȃ��悤�Ȃ̂�if���ɓW�J
   //v_x1 = (v_x1 < 0 ? 0 : v_x1);
   if (v_x1 < 0){
        v_x1 = 0;
   }
   //v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);
   if (v_x1 > 419430400){
        v_x1 = 419430400;
   }
   
   return (uint32_t)(v_x1 >> 12);   
}



