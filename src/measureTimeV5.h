/*
 * measureTimeV5.h
 * 
 * ����ƕ\��
 * 
 *  2022.04.16 measure.h
 * V5_edition
 *  2024.1.5
 *  
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MEASURE_TIME_V5_H
#define	MEASURE_TIME_V5_H


//sensor number
#define     NUM_SENSOR      5       //�Z���T�[��
#define     SENSOR1         0       //�Z���T�Ăєԍ��ƃv���O�������̓Y��
#define     SENSOR2         1
#define     SENSOR3         2
#define     SENSOR4         3
#define     SENSOR5         4

//sensor mic position     - �Z���T�[�}�C�N��LCD�𔠂Ɉ�̉�
#define SENSOR_HORIZONTAL_SPACING   69.0        //�Z���T�[����      138.0mm
#define SENSOR_VERTICAL_SPACING     87.0        //�Z���T�[�c��      174.0mm
#define SENSOR_CENTER_SPACING       0.0         //�Z���T�[���S����    0.0mm
#define SENSOR_DEPTH_OFFSET         30//15.5        //���r��t2�@���̔������͉��r�̗��ʂƂ���

//target sheet
#define     TARGET_WIDTH_HALF       150//105         //�}�g������1/2
#define     TARGET_HEIGHT_HALF      210//120         //�}�g�c������1/2
#define     R_MAX                   260//340         //�قڑΊp��


//GLOBAL
extern const float      delay_a;        //�Z���T�[�x�ꎞ�Ԃ̌v�Z�W��
extern const float      delay_b;
extern sensor_data_t    sensor5Measure[];  //�Z���T�[�f�[�^�A����l


//
void    measureInit(void);
void    correctSensorOffset(float, float);
uint8_t measureMain(uint16_t);
//
uint8_t checkInputOrder(void);
uint8_t assignMeasureData(void);
//
void    clearData(void);
//time
float   dist_delay_mm(float);
float   v_air_mps(void);
float   delay_time_usec(uint32_t);
float   delay_comparator_usec(float);
float   impact_time_msec(float);
    
//sensor interrupt
void    detectSensor1(uintptr_t);
void    detectSensor2(uintptr_t);
void    detectSensor3(uintptr_t);
void    detectSensor4(uintptr_t);
void    detectSensor5(uintptr_t);


#endif	//MEASURE_TIME_V5_H