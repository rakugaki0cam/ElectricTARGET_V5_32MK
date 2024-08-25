/*
 * measureTimeV5.c
 * 
 * �d�q�^�[�Q�b�g
 * ����ƕ\��
 * 
 *  2022.04.16 measure.c
 * V2_edition
 *  2022.05.11  1-3�Ԗڂɓ��͂����Z���T����v�Z����
 * V3_edition
 *  2023.01.20  wifi�ǉ�
 * V5_edition
 *  2024.01.5   �Z���T5��
 *  
 */

#include "header.h"
#include "measureTimeV5.h"

//DEBUG
#define COMP_DELAY_TIME_OFF //�v�Z�̌��Z���̓R���p���[�^�f�B���C�^�C���̍��Z�͖���
//DEBUG (global)
//#define DEBUG_MEAS_no       //�f�o�b�Oprintf�\��(�G���[�n)   


//GLOBAL
const float   delay_a = (5.0 / 300);        //�R���p���[�^�I���̒x�ꎞ�Ԃ̌v�Z�W�� usec / mm
const float   delay_b = 10;                 //usec
sensor_data_t   sensor5Measure[NUM_SENSOR]= 
{
    {SENSOR1, 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //����
    {SENSOR2, 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //�E��
    {SENSOR3, 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //����
    {SENSOR4, 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //�E��
    {SENSOR5, 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //�E��
};


//LOCAL
typedef enum 
{
    INPUT_ORDER_STATUS_OK,
    INPUT_ORDER_STATUS_NO_INPUT,
    INPUT_ORDER_STATUS_FIXED,       
    INPUT_ORDER_STATUS_ERROR, 
    INPUT_ORDER_STATUS_SOURCE_NUM
} input_order_stat_sor_t;


void measureInit(void)
{
    //�����ݒ�
    float       center_offset_x = 0.0;     //LCD�Z���^�[�ƃZ���T�[�z�u�̃Y��
    float       center_offset_y = 0.0;
    
    correctSensorOffset(center_offset_x, center_offset_y);
    //log_title();  ////////////////////////////////////////////////////////////////////////
    
    //�^�[�Q�b�gLCD�N���A�R�}���h���M
    ESP32slave_ClearCommand();
    
}


void    correctSensorOffset(float center_dx, float center_dy)
{
    //�}�g�̃Z���^�[���C���ƃZ���T�[�ʒu�̃Y����␳
    //�Z���T�[�̕����ʒu�̃Y���␳
    float   sensor1_offset_x = 0;
    float   sensor1_offset_y = 0;

    float   sensor2_offset_x = 0;
    float   sensor2_offset_y = 0;

    float   sensor3_offset_x = 0;
    float   sensor3_offset_y = 0;

    float   sensor4_offset_x = 0;
    float   sensor4_offset_y = 0;
    
    float   sensor5_offset_x = 0;//������������ƈ꒼���ɂȂ�Ȃ��̂Ōv�Z�����O�p�@�̂܂܂ɂȂ�B
    float   sensor5_offset_y = 0;
    
    float   sensor_offset_z = 0;    //z�ʒu�͓��ꕽ�ʏ�Ƃ��Čv�Z��������Ă���̂ŁA�Z���T�[���Ƃ̐ݒ�͕s�B

    //x
    sensor5Measure[0].sensor_x_mm = -SENSOR_HORIZONTAL_SPACING + sensor1_offset_x + center_dx;
    sensor5Measure[1].sensor_x_mm =  SENSOR_HORIZONTAL_SPACING + sensor2_offset_x + center_dx;
    sensor5Measure[2].sensor_x_mm = -SENSOR_HORIZONTAL_SPACING + sensor3_offset_x + center_dx;
    sensor5Measure[3].sensor_x_mm =  SENSOR_HORIZONTAL_SPACING + sensor4_offset_x + center_dx;
    sensor5Measure[4].sensor_x_mm =  SENSOR_HORIZONTAL_SPACING + sensor5_offset_x + center_dx;  //�O�p�z�u�̎�e���[���ɂ��Ȃ����߂ɂ͂�����+0.1����B�v�Z���ʂ͂͏����ς��B
    //y        
    sensor5Measure[0].sensor_y_mm = -SENSOR_VERTICAL_SPACING + sensor1_offset_y + center_dy;
    sensor5Measure[1].sensor_y_mm = -SENSOR_VERTICAL_SPACING + sensor2_offset_y + center_dy;
    sensor5Measure[2].sensor_y_mm =  SENSOR_VERTICAL_SPACING + sensor3_offset_y + center_dy;
    sensor5Measure[3].sensor_y_mm =  SENSOR_VERTICAL_SPACING + sensor4_offset_y + center_dy;
    sensor5Measure[4].sensor_y_mm =  SENSOR_CENTER_SPACING   + sensor5_offset_y + center_dy;
    //z
    sensor5Measure[0].sensor_z_mm =  SENSOR_DEPTH_OFFSET + sensor_offset_z;
    sensor5Measure[1].sensor_z_mm =  SENSOR_DEPTH_OFFSET + sensor_offset_z;
    sensor5Measure[2].sensor_z_mm =  SENSOR_DEPTH_OFFSET + sensor_offset_z;
    sensor5Measure[3].sensor_z_mm =  SENSOR_DEPTH_OFFSET + sensor_offset_z;
    sensor5Measure[4].sensor_z_mm =  SENSOR_DEPTH_OFFSET + sensor_offset_z;

}



uint8_t measureMain(uint16_t shotCnt)
{
    //���胁�C���`�v�Z
    //shotCnt:�V���b�g�ԍ�
    meas_stat_sor_t     measStat = MEASURE_STATUS_OK;
    calc_stat_sor_t     calcStat = CALC_STATUS_OK;
    
    //����f�[�^�����m�F�C��
    checkInputOrder();
    //����f�[�^���v�Z�A���
    if (assignMeasureData() < 3)
    {
        //���萔������Ȃ���(�v�Z�ɂ�3�ȏ�̃f�[�^���K�v)
        calcStat = CALC_STATUS_NOT_ENOUGH;
        resultError999(NUM_CAL, calcStat);
        
        measStat = MEASURE_STATUS_NOT_ENOUGH;
        ledLightOn(LED_ERROR);
        uint8_t mes[] = "calc err: num of data not enough";  //40����
        ESP32slave_SendMessage(mes);
        return measStat;
    }

    //���W�̌v�Z
    calcStat = computeEpicenter();
    //printf("calc status:%d", ans);    
    
    if (CALC_STATUS_OK == calcStat)
    {       
        //����OK
        ledLightOn(LED_BLUE);
        
    }
    else
    {
        //�G���[�̎�
        measStat = MEASURE_STATUS_ERROR;
        ledLightOn(LED_CAUTION);

    }
    
    return measStat;
}


//
uint8_t checkInputOrder(void)
{
    //���͏����̍Ċm�F�C��
    //���͎��Ԃ��߂��ꍇ�Ɋ���������������ւ�邱�Ƃ�����...�����ۗ̕�����������ꍇ�A��������IRQ#���ɂȂ邽��
    input_order_stat_sor_t status = INPUT_ORDER_STATUS_OK;
    uint8_t     i;
    uint8_t     sensor_number;             
    uint8_t     input_order[NUM_SENSOR];      //���͏��v�Z����
    
    for (sensor_number = 0; sensor_number < NUM_SENSOR; sensor_number++)
    {
        input_order[sensor_number] = 0;
        
        for (i = 0; i < NUM_SENSOR; i++)
        {
            if (sensor5Measure[sensor_number].timer_cnt > sensor5Measure[i].timer_cnt)
            {
                //�����̒l��菬�����l�����������J�E���g����
                input_order[sensor_number]++;
                //�����l�����������͓������ł��Ă��܂�......������////////////////////
            }
        }
        
        if (sensor5Measure[sensor_number].input_order == 0xff)
        {
            //�����o�Z���T�͖����o�̂܂�
#ifdef DEBUG_MEAS
            printf("Sensor%d is NO input!\n", (sensor_number + 1));   
#endif
            status = INPUT_ORDER_STATUS_NO_INPUT;
            ledLightOn(LED_CAUTION);
            
        }
        else if (sensor5Measure[sensor_number].input_order != input_order[sensor_number])
        {
            //�C�����������ꍇ
#ifdef DEBUG_MEAS
            printf("(S%d) input Order (%d -> %d) is changed correctly!\n", sensor_number, sensor5Measure[sensor_number].input_order, input_order[sensor_number]);
#endif
            status = INPUT_ORDER_STATUS_FIXED;
            ledLightOn(LED_CAUTION);
            sensor5Measure[sensor_number].input_order = input_order[sensor_number];
        }
        
    }
    return status;
}


uint8_t assignMeasureData(void)
{
    //����f�[�^����
    //�o��: �L������Z���T�[��
    meas_stat_sor_t   sensorStat = SENSOR_STATUS_OK;
    uint8_t     SensNum;                //�Z���T�ԍ� 0-4
    uint8_t     firstSensor = 0;        //�ŏ��ɃI�������Z���T�[�̔ԍ�
    uint8_t     validSensorCount = 0;   //�L������Z���T�[���̃J�E���g
    
    for (SensNum = 0; SensNum < NUM_SENSOR; SensNum++)
    {
        switch(sensor5Measure[SensNum].input_order)
        {
            case 0:
                //�ŏ��ɃI�������Z���T�[�ԍ�
                firstSensor = SensNum;
                
            case 1 ... (NUM_SENSOR - 1):
                validSensorCount ++;
                sensorStat = SENSOR_STATUS_OK;
                break;
                
            case 0xff:
                sensorStat = SENSOR_STATUS_INVALID;
                ledLightOn(LED_CAUTION);
                break;
                
            default:
                sensorStat = SENSOR_STATUS_ERROR;
                ledLightOn(LED_CAUTION);
               break;
        }
        sensor5Measure[SensNum].status = sensorStat;
    }
    
    //�^�C�}�J�E���g���A���ԍ��A�����̌v�Z�Ƒ��
    for (SensNum = 0; SensNum < NUM_SENSOR; SensNum++)
    {
        if (SENSOR_STATUS_OK == sensor5Measure[SensNum].status)
        {
            //�X�e�[�^�XOK�̂Ƃ������v�Z�A���
            sensor5Measure[SensNum].delay_cnt       = sensor5Measure[SensNum].timer_cnt - sensor5Measure[firstSensor].timer_cnt;                         //�J�E���g��
            sensor5Measure[SensNum].delay_time_usec = delay_time_usec(sensor5Measure[SensNum].delay_cnt);                                               //�J�E���g�l������
            sensor5Measure[SensNum].distance_mm     = dist_delay_mm(sensor5Measure[SensNum].delay_time_usec - sensor5Measure[SensNum].comp_delay_usec); //���ԁ�����
            sensor5Measure[SensNum].comp_delay_usec = delay_comparator_usec(sensor5Measure[SensNum].delay_time_usec);                                   //�R���p���[�^�����x��ȈՌv�Z        
        }
    }

    return validSensorCount;
}


//
void clearData(void)
{
    //����f�[�^���N���A
    uint8_t i;
    
    sensorCnt = 0;
    
    //����l�N���A
    for (i = 0; i < NUM_SENSOR; i++)
    {
        sensor5Measure[i].input_order = 0xff;      //�����͔���p��0�ł͂Ȃ���0xff
        sensor5Measure[i].timer_cnt = 0xffffffff;  //�^�C���������鎞�̂��߂ɍő�̒l�ɂ��Ă���
        sensor5Measure[i].delay_cnt = 0;
        sensor5Measure[i].delay_time_usec = 0;
        sensor5Measure[i].comp_delay_usec = 0;
        sensor5Measure[i].d_time_corr_usec = 0;
        sensor5Measure[i].distance_mm = 0;
        sensor5Measure[i].status = 0;
    }
   
    ICAP1_Enable();
    ICAP2_Enable();
    ICAP3_Enable();
    ICAP4_Enable();
    ICAP5_Enable();
    
    TMR2 = 0;       //clear
    TMR2_Start(); 
    
    videoSync_Stop();
    
}



//*****time *******

float   dist_delay_mm(float time_usec)
{
    //���Ԃ��������狗���Ɍv�Z
    return  time_usec * v_air_mps() / 1000;
}


float   v_air_mps(void)
{
    //����m/sec�����߂� 
    return 331.5 + 0.61 * air_temp_degree_c;
}


float   delay_time_usec(uint32_t timer_count)
{
    //�^�C�}�[�J�E���g�l��������usec�ɕϊ�
    return (float)timer_count / (TMR2_FrequencyGet() / 1000000);
}


float   delay_comparator_usec(float delay_time)
{
    //�R���p���[�^�����x�ꎞ�ԁ@�[�@�����ɂ���ĉ����������Ȃ�x���
    //�Z���T�[�������E���Ă���R���p���[�^���I������܂ł̒x�ꎞ��usec
    //delay_time:���e�`�Z���T�I���܂ł̎��Ԃő�p�B�ȗ���
    //�{���̓Z���T�`���e�_�̋����ɂ�邯��ǁA������ɂ͋����v�Z��ɂ�����x�␳���ċ������v�Z�������Ȃ��Ƃ����Ȃ�
    
    float   correct_time;
    
//DEBUG   
#ifndef COMP_DELAY_TIME_OFF
    correct_time = delay_a * delay_time + delay_b;  //�␳�W���@dt��10�`16���炢time��0�`300usec
#else
    correct_time = 0;
#endif    
    
    return correct_time;
}


float   impact_time_msec(float r0_mm)
{
    //r0���璅�e�����̉����x�ꕪ�𐄒�v�Z
    //���e�����͍ŏ��̃Z���T�I�����O�̎���
    //���r��t2�̒���`��鎞��(�\�ɋʂ�������A���ʂɓ`��鎞��)���v���X����
#define TARGET_PLATE_T_MM   2.0     //����r�̃}�g�̌��� mm
#define V_PLATE_MPS         2300    //����r���̉��� m/sec(= mm/msec)
    float   dt_t2_msec;
    
    dt_t2_msec = (float)TARGET_PLATE_T_MM / V_PLATE_MPS;    //msec
    
    return  -(r0_mm / v_air_mps() + dt_t2_msec);      //msec
}



//***** sensor interrupt ********************************************************

void detectSensor1(uintptr_t context)
{
    //�Z���T1����
    impact_PT4_On();                                               //����s�v�F�ŏ��̃Z���T�I���ŃZ�b�g�����̂�2�ڈȍ~�̃Z���T�ōăZ�b�g����Ă��Ƃ��ɉe���͖���
    sensor5Measure[SENSOR1].timer_cnt = ICAP1_CaptureBufferRead();   //�ǂݏo�������Ȃ��Ɗ����t���O���N���A�ł����A�Ċ����������Ă��܂�
    ICAP1_Disable();
    sensor5Measure[SENSOR1].input_order = sensorCnt;
    sensorCnt++;   
}


void detectSensor2(uintptr_t context)
{
    //�Z���T2����
    impact_PT4_On();
    sensor5Measure[SENSOR2].timer_cnt = ICAP2_CaptureBufferRead();
    ICAP2_Disable();
    sensor5Measure[SENSOR2].input_order = sensorCnt;
    sensorCnt++;
}


void detectSensor3(uintptr_t context)
{
    //�Z���T3����
    impact_PT4_On();
    sensor5Measure[SENSOR3].timer_cnt = ICAP3_CaptureBufferRead();
    ICAP3_Disable();
    sensor5Measure[SENSOR3].input_order = sensorCnt;
    sensorCnt++;
}


void detectSensor4(uintptr_t context)
{
    //�Z���T4����
    impact_PT4_On();
    sensor5Measure[SENSOR4].timer_cnt = ICAP4_CaptureBufferRead();
    ICAP4_Disable();
    sensor5Measure[SENSOR4].input_order = sensorCnt;
    sensorCnt++;
}


void detectSensor5(uintptr_t context)
{
    //�Z���T5����
    impact_PT4_On();
    sensor5Measure[SENSOR5].timer_cnt = ICAP5_CaptureBufferRead();
    ICAP5_Disable();
    sensor5Measure[SENSOR5].input_order = sensorCnt;
    sensorCnt++;
}



/*******************************************************************************
 End of File
*/