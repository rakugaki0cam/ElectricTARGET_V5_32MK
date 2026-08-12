/* 
 * File:   PCF8574.h
 * 
 * 2024.01.14
 * 
 */

#ifndef PCF8574_H
#define	PCF8574_H


//Global
typedef enum 
{
    NO_OUTPUT   = 0,            //‰½‚àƒIƒ“‚µ‚È‚¢
    LED_BLUE    = 0b00000001,   //P0
    LED_YELLOW  = 0b00000010,   //P1
    LED_PINK    = 0b00000100,   //P2
    P3          = 0b00001000,
    P4          = 0b00010000,
    SW3         = 0b00100000,   //P5
    SW2         = 0b01000000,   //P6
    SW1         = 0b10000000,   //P7
} port_name_t;


bool    PCF8574_Init(void);
//output
void    ledLightOn(port_name_t);
void    ledLightOff(port_name_t);
//input
uint8_t exIoRead(void);
bool    switchStatus(uint8_t currentRegister, port_name_t swName);

#endif //PCF8574_H