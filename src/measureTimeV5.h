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
#ifndef MEASURE_H
#define	MEASURE_H

//??#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>


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


//�Z���T�[�X�e�[�^�X
typedef enum {
    SENSOR_STATUS_OK,       //OK
    SENSOR_STATUS_INVALID,  //�f�[�^����     
    SENSOR_STATUS_ERROR,    //�G���[  
    SENSOR_STAT_SOURCE_NUM
} sensor_stat_sor_t;


//�Z���T���Ƒ���l
typedef struct {
    uint8_t             sensor_num;         //�Z���T�ԍ�
    uint8_t             input_order;        //�Z���T�f�[�^���͏��� 0~5
    float               sensor_x_mm;        //�Z���T���W
    float               sensor_y_mm;
    float               sensor_z_mm;
    uint32_t            timer_cnt;          //�^�C�}����l
    uint32_t            delay_cnt;          //�^�C�}���l
    float               delay_time_usec;    //���B���ԍ�����l
    float               comp_delay_usec;    //�R���p���[�^�����x�ꎞ��(����)
    float               d_time_corr_usec;   //���B���ԍ��␳��
    float               distance_mm;        //������
    sensor_stat_sor_t   status;             //���
} sensor_data_t;
extern sensor_data_t    sensor5Measure[];  //�Z���T�[�f�[�^�A����l


//����X�e�[�^�X
typedef enum {
    MEASURE_STATUS_OK,
    MEASURE_STATUS_NOT_ENOUGH,       
    MEASURE_STATUS_ERROR,
    MEASURE_STATUS_SOURCE_NUM
} meas_stat_sor_t;

//�O���[�v���̕��U�̌v�Z
typedef struct {
    uint16_t        pattern;            //�Z���T�I���p�^�[�� �\�i�@�e�����Z���T�ԍ�
    uint8_t         sample_n;           //���σT���v����
    float           average_pos_x_mm;
    float           average_pos_y_mm;
    float           average_radius0_mm;
    float           dist1_mm2;          //�΍� ������2�� dx^2+dy^2
    float           dist2_mm2;
    float           dist3_mm2;
    float           dist4_mm2;
    float           variance;           //�΍��̑��a�̕��ρ@=�@���U(��^2�l)
    uint8_t         order;              //�΂���̏���
    calc_stat_sor_t status;
} result_ave_t;

extern result_ave_t    groupVari[];




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


#endif	//MEASURE_H