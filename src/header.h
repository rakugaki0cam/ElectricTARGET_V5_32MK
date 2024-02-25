/*
 * Electric Target #2  V5 - PIC32MK MCJ
 * 
 *  header.h
 * 
 * 
 *  2023.12.22
 * 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef _HEADER_H    /* Guard against multiple inclusion */
#define _HEADER_H


//
#include <xc.h>
#include <stdint.h>
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <string.h>
#include <math.h>

//
#include "BME280v3.h"
#include "computeEpicenterV5.h"
#include "dataDispSendV5.h"
#include "ESP32slave.h"
#include "iP5306.h"
#include "log.h"
#include "measureTimeV5.h"
#include "MyI2C1.h"
#include "PCF8574.h"


//return value
#define OK      0
#define ERROR   1

//DEBUG
//calculation error -> LED Yellow   //DEBUG �v�Z���̌y���ȃG���[�̎����FLED��_����
#define DEBUG_LED_no
#ifdef DEBUG_LED
    #define LED_CAUTION   LED_YELLOW
#else
    #define LED_CAUTION   NO_OUTPUT
#endif


//���e�^�C�~���O�̃t�H�[���G�b�W��ESP32�ƃ^�}���j�ɑ���
#define     impact_PT4_On()     PT4_Clear()     //���e�Z���T�M���o��
#define     impact_PT4_Off()    PT4_Set()       //���e�Z���T�M���N���A



//Global
extern uint8_t      sensorCnt;                  //�Z���T���͏��Ԃ̃J�E���g
extern uint16_t     ringPos;                    //���O�f�[�^�|�C���^
extern debugger_mode_sour_t    debuggerMode;    //DEBUGger�\�����[�h


//callback
void mainSwOn_callback(EXTERNAL_INT_PIN, uintptr_t);
void timer1sec_callback(uintptr_t);
//sub
void debuggerComand(void);


#endif //_HEADER_H

/* *****************************************************************************
 End of File
*/
