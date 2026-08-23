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
#include "PCF8574.h"


#define     PCF8574_ID      0x20        //0x20~0x27
//レジスタは1つだけなので、アドレスは無い

#define     IO_SETTING      0b11100000  //入力ポート:1を書込(100uAのプルアップが有効になる)


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
    ledLightOn(LED_BLUE | LED_YELLOW | LED_PINK); //LED全灯
    printf("OK\n");
    
    return OK;
}
    

//
void    ledLightOn(uint8_t color)
{
    //LEDをオン
    //複数一度にやりたい時はorで並べる
    uint8_t err;
            
    if (NO_OUTPUT == color)
    {
        //何もオンしない時
        return;
    }
    uint8_t currentRegister = 0;
    err = i2c1_ReadRegister(PCF8574_ID, &currentRegister);
    if (err)
    {
      printf("I2C read error(%d)!\n", err);
      return;
    }
    uint8_t tmpRegister = (currentRegister | color) | IO_SETTING;
    err = i2c1_WriteRegister(PCF8574_ID, tmpRegister);
    if (err)
    {
      printf("I2C write error(%d)!\n", err);
    }
}


void    ledLightOff(uint8_t color)
{
    //LEDをオフ
    uint8_t err;
    
    if (NO_OUTPUT == color)
    {
        //何もオフしない時
        return;
    }
    uint8_t currentRegister = 0;
    err = i2c1_ReadRegister(PCF8574_ID, &currentRegister);
    if (err)
    {
      printf("I2C read error(%d)!\n", err);
      return;
    }
    uint8_t tmpRegister = (currentRegister | (uint8_t)(~color)) | IO_SETTING;   //~ビット反転 ~演算時32ビットに拡張されるとまずい場合があるので、キャストする
    err = i2c1_WriteRegister(PCF8574_ID, tmpRegister);
    if (err)
    {
      printf("I2C write error(%d)!\n", err);
    }
}


//input
uint8_t readInputRejister(void)
{   //レジスタを読み取り
    uint8_t d = 0xff;
    static uint8_t lastSwState = IO_SETTING; 

    uint8_t err = i2c1_ReadRegister(PCF8574_ID, &d);
    if (err)
    {
        printf("I2C read error(%d)!\n", err);
        return lastSwState; //前回値を返す
    }

    uint8_t currentSwState = d & IO_SETTING;//入力ピンだけを抜き出し
    if (currentSwState != lastSwState)
    { // スイッチの状態が変わったときだけ、改行されて綺麗にログが出る
        printf("SW changed - 0x%02x (Raw:0x%02x)\n", currentSwState, d);
        lastSwState = currentSwState;
    }
    return d;
}


bool switchStatus(uint8_t currentRegister,uint8_t swName)
{   //スイッチ入力の検出
    //スイッチはプルアップ接続。スイッチオンでデータはLになる
    //戻り値はスイッチオンでH、オフでL。
    return !(currentRegister & swName);  
}


//---- TEST --------------------------
void ioExpanderTest(void)
{ //SW1,2,3でLED青、黄、ピンク点灯
    printf("SW1....LED BLUE ON\n");
    printf("SW2....LED YELLOW ON\n");
    printf("SW3....LED PINK ON\n");

    while(1)
    {
        uint8_t reg = readInputRejister();
        if (switchStatus(reg, SW1))
        {
            ledLightOn(LED_BLUE);
        }
        else
        {
            ledLightOff(LED_BLUE);
        }
        if (switchStatus(reg, SW2))
        {
            ledLightOn(LED_YELLOW);
        }
        else
        {
            ledLightOff(LED_YELLOW);
        }
        if (switchStatus(reg, SW3))
        {
            ledLightOn(LED_PINK);
        }
        else
        {
            ledLightOff(LED_PINK);
        }
        CORETIMER_DelayMs(100);
    }
}


