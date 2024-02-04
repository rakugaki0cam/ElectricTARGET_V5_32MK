/*
 * computeEpicenterv5.h
 * 
 * ���W�̌v�Z
 * 
 *  2022.03.24�@calc_locate.h���
 * 
 *  2024.01.05 V5 �Z���T�[5��
 *  
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef CONPUTE_EPICENTER_V5_H
#define	CONPUTE_EPICENTER_V5_H


//GLOBAL
//�v�Z�p�^�[����
#define     NUM_CAL         10      //�Z���T�[5����3�I�ԃp�^�[���̐� 10    5C3 = 5x4x3 / 3x2x1
#define     NUM_GROUP       5       //�Z���T�[4�ł̂΂��������O���[�v���@


//�Z���T�[�X�e�[�^�X
typedef enum {
    SENSOR_STATUS_OK,       //OK
    SENSOR_STATUS_INVALID,  //�f�[�^����     
    SENSOR_STATUS_ERROR,    //�G���[  
    SENSOR_STAT_SOURCE_NUM
} sensor_stat_sor_t;


//����X�e�[�^�X
typedef enum {
    MEASURE_STATUS_OK,
    MEASURE_STATUS_NOT_ENOUGH,       
    MEASURE_STATUS_ERROR,
    MEASURE_STATUS_SOURCE_NUM
} meas_stat_sor_t;


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


//�v�Z�X�e�[�^�X
typedef enum {
    //
    CALC_STATUS_OK,
    CALC_STATUS_NOT_ENOUGH,     //����l���s��
    CALC_STATUS_ERROR,          //�����܂�meas_stat_sor_t�Ɠ���
    //        
    CALC_STATUS_CAL_ERROR,      //�v�Z�G���[����
    CALC_STATUS_E_ZERO,         //����e=0
    CALC_STATUS_QUAD_F,         //���̌����̏����G���[
    CALC_STATUS_R0_UNDER0,      //r0����
    CALC_STATUS_X0_INVALID,     //x0���͈͊O
    //       
    CALC_STATUS_X0_ERR,         //��x0���͈͊O
    CALC_STATUS_Y0_ERR,         //��y0���͈͊O
    CALC_STATUS_R0_ERR,         //��r0���͈͊O
    //        
    CALC_STATUS_AVERAGE_ERR,    //���ϒl���s��
    CALC_STATUS_DEV_OVER,       //�΍�����
    //
    CALC_STATUS_SOURCE_NUM
} calc_stat_sor_t;


//���e�ʒu�v�Z����
typedef struct {
    uint16_t        pattern;            //�Z���T�I���p�^�[�� �\�i�@�e�����Z���T�ԍ�
    float           impact_pos_x_mm;    //���e���W
    float           impact_pos_y_mm;
    float           radius0_mm;         //���e�_����ŏ��̃Z���T�܂ł̋���
    float           delay_time0_msec;   //���e�_����ŏ��̃Z���T�I���܂ł̎���
    calc_stat_sor_t status;             //���
} impact_result_t;

extern impact_result_t  calcValue[];    //���W�̌v�Z
extern impact_result_t  calcResult;     //���W�̌v�Z�̍ŏI����


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

extern result_ave_t     vari5Groupe[];    //���ʔ���p�΂���̌v�Z





//
void            clearResult(uint8_t);
void            resultError999(uint8_t, calc_stat_sor_t);
//
calc_stat_sor_t computeEpicenter(void);
calc_stat_sor_t computeXY(uint8_t);
//
uint8_t         select3sensor(uint8_t, sensor_data_t*);
uint16_t        sensorOrderPattern(uint8_t);
uint16_t        sensorGroupePattern(uint8_t);
uint8_t         firstSensor(void);
uint8_t         check_deviation(void);
//calculation
uint8_t         apollonius3circleXYR(uint8_t, sensor_data_t*);


#endif	//CONPUTE_EPICENTER_V5_H

