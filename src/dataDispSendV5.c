/*
 * dataDispSendV5.c
 * 
 * ����l��\���AESP �^�}���j�L���@���M
 * 
 * measure_V5.c��蕪��
 *  2024.1.8
 *  2024.01.21  �t���f�o�b�O���[�h..�v�Z�o�߂�ǉ�
 * 
 *  
 */

#include "header.h"
#include "dataDispSendV5.h"



void serialPrintResult(uint16_t shotCount, uint8_t meaStat, uint8_t mode){ 
    //���茋�ʁA�v�Z���ʂ̕\��

    dataSendToESP32();  //���e�\���𑁂�����
    
    //CORETIMER_DelayMs(100); //RS485�֑��M�̑O�ɊԂ��J����B�^�}���j����f�o�b�K��printf���Ă��邩������Ȃ�����/////////////////////////
    dataSendToTAMAMONI();   //LAN
    
    //CORETIMER_DelayMs(30);  //�f�[�^���M�̊Ԃ�����/////////////////////////////////////////////////////////////////////////
    switch(mode){
        case NONE:
            break;
        case SINGLE_LINE:
            printSingleLine(shotCount, meaStat);
            break;
        case MEAS_CALC:
            printMeasCalc(shotCount, meaStat);
            break;
        case FULL_DEBUG:
            printFullDebug(shotCount, meaStat);
            break;
        case CSV_DATA:
            printDataCSV(shotCount);
            break;
        default:
            //error
            break;
    }    
}


void dataSendToTAMAMONI(void){
    //�^�}���j�֍��W�f�[�^����L�����M
    //�I�t�Z�b�g�����Z���Ȃ��Ƃ����Ȃ�
    printf("\n[TARGET#2->]\n");//////////////////
    printf("BINX0Y0dT %8.3f %8.3f %8.4f END ,", calcResult.impact_pos_x_mm, calcResult.impact_pos_y_mm, calcResult.delay_time0_msec);
}


void dataSendToESP32(void){
    //target LCD(ESP32)�֍��W�f�[�^����I2C�ő��M
    //ESP32�͂���i2c�f�[�^���󂯂Ƃ�A�^�[�Q�b�g�\���ƃf�[�^�ϊ����ă^�}���j��WiFi���M(ESP-NOW)������B
    
    //float 4�o�C�g�ϐ���1�o�C�g���Ƃ̔z��ɂ��Ă͂߂đ��� = �|�C���^�n��
#define FLOAT_BYTES 4   //float��4�o�C�g  
    float   tmp[3];

    tmp[0] = calcResult.impact_pos_x_mm;
    tmp[1] = calcResult.impact_pos_y_mm;
    tmp[2] = calcResult.delay_time0_msec;
    ESP32slave_SendImpactData((uint8_t*)tmp, 3 * FLOAT_BYTES);   //1�o�C�g���Ƃ̃f�[�^�Ƃ��đ��邽�߂�(uint8_t*)�ŃL���X�g����B
}



//DEBUGger  serial debug

void printSingleLine(uint16_t shotCount, meas_stat_sor_t measStat){
//void single_line(uint16_t shot_count, meas_stat_sor_t meas_stat){ /////////////////�Ȃ���������meas_stat_sor_t���͂��Ă��Ȃ�????
    //���W�̂݁@1�s�\��
    calc_stat_sor_t      calc_stat;

    if (MEASURE_STATUS_OK != measStat){
        printf("%02d:measurement error\n", shotCount);
        return;
    }
    calc_stat = calcResult.status;     //���ϒl�̏��
    if (CALC_STATUS_AVERAGE_ERR != calc_stat){
        printf("%02d:%6.1fx %6.1fy\n", shotCount, calcResult.impact_pos_x_mm, calcResult.impact_pos_y_mm);
    }else {
        printf("%02d:calc error\n", shotCount);
    }
}


void printMeasCalc(uint16_t shotCount, meas_stat_sor_t measStat){
    //�f�o�b�O�p�t���\��
    sensor_stat_sor_t   sens_stat;
    calc_stat_sor_t     calc_stat;
    uint8_t             i, n;
    printf("\n\n");
    printf("> shot%03d ***%5.1f%cC\n", shotCount, air_temp_degree_c, 0xdf);
    
#ifndef  SENSOR_DATA_DEBUG_TIMER_VALUE
    //���͏��^�C�}�[�J�E���g�\�� 
    for (i = 0; i < NUM_SENSOR; i++){
        for (n = 0; n < NUM_SENSOR; n++){
            if (sensor5Measure[n].input_order == i){
            //���͏��ԍ�����v����܂ő���
                break;
            }
        }
        printf("t%1d %08x (", i + 1, sensor5Measure[n].timer_cnt);  //���͏��^�C�}�l 

        if (n == NUM_SENSOR){
            printf("--)\n");    //���͂Ȃ�

        }else {
            printf("s%1d)\n", n + 1);  //�Z���T#
        }
    }
#endif         
    for (i = 0; i < NUM_SENSOR; i++){
        sens_stat = sensor5Measure[i].status;
        if (sens_stat == SENSOR_STATUS_OK){
            printf("s%1d %6.2fus %6.2fmm\n", i + 1, sensor5Measure[i].delay_time_usec, sensor5Measure[i].distance_mm);
        }else{
            //���o���Ă��Ȃ���
            printf("s%1d  ---.--us\n", i + 1);
        }
    }
    if (MEASURE_STATUS_OK != measStat){
        printf("> measurement error\n");
        return;
    }
    
    //�p�^�[�����̌v�Z���ʂƕ���
    printf("   x0     y0     r0 \n");
    for (i = 0; i < NUM_CAL; i++){
        calc_stat = calcValue[i].status;
        switch(calc_stat){
            case CALC_STATUS_OK:
                printf("%6.1f %6.1f %6.1f\n", calcValue[i].impact_pos_x_mm, calcValue[i].impact_pos_y_mm, calcValue[i].radius0_mm);
                break;
            case CALC_STATUS_NOT_ENOUGH :
                printf(" c%2d error\n", i + 1);
                break;
            case CALC_STATUS_CAL_ERROR ... CALC_STATUS_QUAD_F:
                printf(" c%2d calc error\n", i + 1);
                break;
            case CALC_STATUS_X0_ERR:
                printf(" c%2d x error\n", i + 1);
                break;
            case CALC_STATUS_Y0_ERR:
                printf(" c%2d y error\n", i + 1);
                break;
            case CALC_STATUS_R0_ERR:
                printf(" c%2d r error\n", i + 1);
                break;
            case CALC_STATUS_AVERAGE_ERR:
                printf(" c%2d average error\n", i + 1);
                break;
            case CALC_STATUS_DEV_OVER:
                printf(" c%2d dev over\n", i + 1);
                break;
            default:
                break;
        }

    }
    calc_stat = calcResult.status;     //���ϒl�̏��
    if (CALC_STATUS_AVERAGE_ERR != calc_stat){
        printf("----- result�@----\n");
        printf("%6.1f %6.1f %6.1f\n", calcResult.impact_pos_x_mm, calcResult.impact_pos_y_mm, calcResult.radius0_mm);
    }else {
        printf("> No average\n");
    }
    
    printf("\n");

}


void printFullDebug(uint16_t shotCount, meas_stat_sor_t measStat){
    //�f�o�b�O�p�t���\��
    
    uint8_t             i;
    uint8_t             selGroup = 0;   //�΂�����ŏ��̃O���[�v�̔ԍ�
    float               stdDev = 0;     //�W���΍�
    
    printf("\n");
    printf("--- SHOT #%d --------------------------------------------------------------------------------------------------------------------\n", shotCount);
    printf("5Sensor Measure results\n");
    for (i = 0; i < NUM_SENSOR; i++){
        printf("%1d-(sens%1d)in# -> %1d  ", (i + 1), sensor5Measure[i].sensor_num, (sensor5Measure[i].input_order + 1));
        printf("[ x:%6.1f y:%6.1f z:%6.1f ] - ", sensor5Measure[i].sensor_x_mm, sensor5Measure[i].sensor_y_mm, sensor5Measure[i].sensor_z_mm);
        printf("0x%08x dt(0x%08x)  ", sensor5Measure[i].timer_cnt, sensor5Measure[i].delay_cnt);

        if (SENSOR_STATUS_OK == sensor5Measure[i].status){
            printf("%7.2fus  %6.2fmm  ", sensor5Measure[i].delay_time_usec, sensor5Measure[i].distance_mm);
        }else{
            //���o���Ă��Ȃ���
            printf("----.--us  ---.--mm  ");
        }
        printf("err:%2d\n", sensor5Measure[i].status);
    }
    printf("\n");
    
    printf("10Pattern - Epicenter of 3sensors\n");
    for (i = 0; i < NUM_CAL; i++){
        printf("%2d-(sens%03X) ", (i + 1), calcValue[i].pattern);
        printf("x:%7.2f  y:%7.2f  r0:%7.2f  ", calcValue[i].impact_pos_x_mm, calcValue[i].impact_pos_y_mm, calcValue[i].radius0_mm);
        printf("dt:%7.3f  err:%2d\n", calcValue[i].delay_time0_msec, calcValue[i].status);
    }
    printf("\n");
    
    printf("5Group - Variance of 4sensors\n");
    for (i = 0; i < NUM_GROUP; i++){
        printf("%2d - (calc%04X)  ", (i + 1), vari5Groupe[i].pattern);
        printf("ave[ n=%1d - x:%8.3f  y:%8.3f  r0:%8.3f ]  ", vari5Groupe[i].sample_n, vari5Groupe[i].average_pos_x_mm, vari5Groupe[i].average_pos_y_mm, vari5Groupe[i].average_radius0_mm);
        printf("dev[ d1:%7.3f  d2:%7.3f  d3:%7.3f  d4:%7.3f ]   ", vari5Groupe[i].dist1_mm2, vari5Groupe[i].dist2_mm2, vari5Groupe[i].dist3_mm2, vari5Groupe[i].dist4_mm2);
        printf("variance:%8.4f [%d]  err:%2d\n", vari5Groupe[i].variance, (vari5Groupe[i].order + 1), vari5Groupe[i].status);
        
        if (vari5Groupe[i].order == 0){
            selGroup = i;
            stdDev = sqrt(vari5Groupe[i].variance);
        }
    }
    selGroup++;
    printf("--- Result: [ Group: %1d (exclude sensor%1d): stdev: %6.3fmm ] -------------------------------\n", selGroup, selGroup, stdDev);
    printf("\n\n");

}


void    printDataCSVtitle(void){
    //�\�v�Z�p��CSV�@�^�C�g����
    uint8_t i;
    
    //1�s��
    printf("                                         ,");   //�^�}���j�f�[�^����
    printf("shot#,x ave  ,y ave  ,r ave  ,status");
    for (i = 1; i <= NUM_CAL; i++){
        printf(",x p%1d  ,y p%1d  ,r p%1d  ", i, i, i);
    }
    for (i = 1; i <= NUM_SENSOR; i++){
        printf(",dt sen%1d,sen%1d_d ,sen%1d_r", i, i, i);
    }
    printf(",temp\n");
    //2�s��
    printf("                                         ,");   //�^�}���j�f�[�^����
    printf("     ,mm     ,mm     ,mm     ,      ");
    for (i = 1; i <= NUM_CAL; i++){
        printf(",mm    ,mm    ,mm    ");
    }
    for (i = 1; i <= NUM_SENSOR; i++){
        printf(",usec   ,usec   ,mm    ");
    }
    printf(",deg  \n");
}


void    printDataCSV(uint16_t shotCount){
    //�\�v�Z�p��CSV�f�[�^�@�R���}��؂�
    uint8_t i;
    uint8_t stat;
    
    //�ԍ�
    printf("  %03d", shotCount);
    //����x,y,r
    printf(",%7.2f,%7.2f,%7.2f", calcResult.impact_pos_x_mm, calcResult.impact_pos_y_mm, calcResult.radius0_mm);
    //�X�e�[�^�X
    stat = calcResult.status;
     if (CALC_STATUS_OK == stat){
        printf(",OK    ");
    }else{
        printf(",error ");
    }
    
    //4�p�^�[����x,y,r
    for (i = 0; i < NUM_CAL; i++){
        printf(",%6.1f,%6.1f,%6.1f", calcValue[i].impact_pos_x_mm, calcValue[i].impact_pos_y_mm, calcValue[i].radius0_mm);
    }
    //�Z���T�[���@t,dt,dr
    for (i = 0; i < NUM_SENSOR; i++){
        printf(",%7.2f,%7.2f,%6.1f", sensor5Measure[i].delay_time_usec, sensor5Measure[i].comp_delay_usec, sensor5Measure[i].distance_mm);
    }
    //���x
    printf(",%5.1f", air_temp_degree_c);
    
    printf("\n");
    
}


