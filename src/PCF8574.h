/* 
 * File:   PCF8574.h
 * 
 * 2024.01.14
 * 
 */
#pragma once

#include "header.h"


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
void    ledLightOn(uint8_t);
void    ledLightOff(uint8_t);
//input
uint8_t readInputRejister(void);
bool    switchStatus(uint8_t currentRegister, uint8_t swName);
//TEST
void    ioExpanderTest(void);