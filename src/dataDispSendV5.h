/*
 * dataDispSendV5.h
 * 
 * ����l��\���AESP �^�}���j�L���@���M
 * 
 * measure_V5.h��蕪��
 *  2024.1.8
 *  
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef DATADISPSENDV5_H
#define	DATADISPSENDV5_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


//��ʕ\�����[�h
typedef enum {
    NONE,                   //�o�͂Ȃ�
    SINGLE_LINE,            //��ʕ\��:1�s/1��
    MEAS_CALC,              //����l�ƌv�Z���ʂ܂Ƃ�
    FULL_DEBUG,             //��ʕ\��:�f�o�b�O�p�v�Z�o�߂��t���\��
    CSV_DATA,               //CSV�f�[�^�o���[�\�v�Z�p
    DEBUGGER_MODE_SOURCE_NUM
} debugger_mode_sour_t;



    
void    serialPrintResult(uint16_t, uint8_t, uint8_t);
void    dataSendToTAMAMONI(void);
void    dataSendToESP32(void);

//debugger
void    printSingleLine(uint16_t, uint8_t);
void    printMeasCalc(uint16_t, uint8_t);
void    printFullDebug(uint16_t, uint8_t);
void    printDataCSVtitle(void);
void    printDataCSV(uint16_t);



#endif	//DATADISPSENDV5_H