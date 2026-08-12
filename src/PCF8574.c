/*
 * File:   PCF8574.c
 * 
 * PCF8574 I2C I/O Expander 
 * 
 * 3.3V I2C 8ch
 * 
 * Revision history: 
 * 2024.01.14   
 * 2026.08.11 inputを追加
 * 
 * 
 */
#include "header.h"
#include "PCF8574.h"


#define     PCF8574_ID      0x20        //0x20~0x27

//レジスタは1つだけなので、アドレスは無い

#define     IO_SETTING      0b11100000  //入力ポート:1を書込(100uAのプルアップが有効になる)

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
    ledLightOff((port_name_t)(LED_BLUE | LED_YELLOW | LED_PINK)); //LED消灯
    printf("OK\n");
    
    return OK;
}
    

//
void    ledLightOn(port_name_t color)
{
    //LEDをオン
    //複数一度にやりたい時はorで並べる
    if (NO_OUTPUT == color)
    {
        //何もオンしない時
        return;
    }
    
    portRegister = portRegister | color;
    i2c1_WriteRegister(PCF8574_ID, portRegister);
    
}


void    ledLightOff(port_name_t color)
{
    //LEDをオフ
    if (NO_OUTPUT == color)
    {
        //何もオフしない時
        return;
    }
    
    portRegister = portRegister & (~color); //~ビット反転
    i2c1_WriteRegister(PCF8574_ID, portRegister);
    
}


//input
uint8_t     exIoRead(void)
{   //レジスタを読み取り
    uint8_t d = 0xff;
    static uint8_t _d = 0;
    i2c1_ReadRegister(PCF8574_ID, &d);
    printf("portReg:%02x  new read:%02x\n", portRegister, d);

    if (d != _d)
    {
      printf("read - 0x%02x\n", d);
      _d = d;

    }
    return d;
}


bool switchStatus(uint8_t currentRegister, port_name_t swName)
{ //スイッチ入力の検出
  //スイッチはプルアップ接続。スイッチオンでデータはLになる
  //戻り値はスイッチオンでH、オフでL。
  return !(currentRegister & swName);  
}




