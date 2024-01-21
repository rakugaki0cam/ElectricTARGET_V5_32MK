/*
 * computeEpicenterV5.c
 * 
 * �d�q�^�[�Q�b�g
 * ���W�̌v�Z
 * 
 *  2022.03.24�@calc_locate.c���
 * V2_edition
 *  2022.05.11  1-3�Ԗڂɓ��͂����Z���T����v�Z����
 * 
 * V4�Qedition
 *  2023.08.26  4�ʂ�̌v�Z�l����G���[�����o
 *              1�̃Z���T�[�l���ُ�̏ꍇ�A4�̍��W���o���o���ɂȂ�B
 *              �� ���������W�ُ͈�l�𔲂�������3�̃Z���T���狁�߂�1�����̍��W�B�ǂꂪ���������W�Ȃ̂��̔��肪����B
 * 
 * V5�Qedition
 *  2024.01.05  5�̃Z���T����10�Ƃ���̌v�Z�l�B�Z���T��1������4�̒l�̂΂������ԏ������Z���T�g�ݍ��킹��I������
 *  2024.01.20  �v�Z�ԈႢ�C���@z[1]�̂Ƃ���z[0]�ƊԈႦ�Ă�������cc��900�قǈ���Ă��Čv�Z���W������Ă����B
 *  2024.01.21  �t���f�o�b�O���[�h..�v�Z�o�߂�ǉ�
 *  2024.01.21  �Z���T3���꒼���ɕ���ł��鎞(e=0)�̌v�Z����ǉ��B
 * 
 * 
 */

#include "header.h"
#include "computeEpicenterV5.h"

//DEBUG    
#define DEBUG_APO_no        //���W���Z���̃f�o�b�Oprintf�\��


//GLOBAL
impact_result_t calcValue[NUM_CAL];     //�v�Z���l
result_ave_t    groupVari[NUM_GROUP];   //���ʔ���p�΂���̌v�Z
impact_result_t calcResult;             //�ŏI�v�Z����


//LOCAL
sensor_data_t   tmp3SensorFactor[3];  //�Z���T�f�[�^�󂯓n���p�Z���T3��1�̌v�Z����

//�v�Z���ʂ̐��@10�Ƃ���
//#define     NUM_CAL      10   //GLOBAL
#define     CAL1         0      //�v�Z���ʂ̔ԍ��ƃv���O�������̓Y��
#define     CAL2         1
#define     CAL3         2       
#define     CAL4         3       
#define     CAL5         4
#define     CAL6         5       
#define     CAL7         6       
#define     CAL8         7       
#define     CAL9         8       
#define     CAL10        9

//5�̃Z���T����3�̃Z���T��I������10�̃p�^�[��
uint8_t sensor3outOf5[NUM_CAL][3] = {    
    { SENSOR1, SENSOR2, SENSOR3},   //CAL1�[�v�Z����1
    { SENSOR1, SENSOR2, SENSOR4},   //CAL2
    { SENSOR1, SENSOR2, SENSOR5},   //CAL3
    { SENSOR1, SENSOR3, SENSOR4},   //CAL4
    { SENSOR1, SENSOR3, SENSOR5},   //CAL5
    { SENSOR1, SENSOR4, SENSOR5},   //CAL6
    { SENSOR2, SENSOR3, SENSOR4},   //CAL7
    { SENSOR2, SENSOR3, SENSOR5},   //CAL8
    { SENSOR2, SENSOR4, SENSOR5},   //CAL9
    { SENSOR3, SENSOR4, SENSOR5},   //CAL10
};


//1�Z���T�𔲂���5�O���[�v�ɕ���     
//#define     NUM_GROUP       5
#define     GROUP1          0
#define     GROUP2          1
#define     GROUP3          2
#define     GROUP4          3
#define     GROUP5          4
//4�̃Z���T����̌v�Z����4�Ƃ���
uint8_t calcValue4[NUM_GROUP][4] = {
    { CAL7, CAL8, CAL9, CAL10},     //GROUP1  SENSOR2,3,4,5�@�[�@�Z���T1������
    { CAL4, CAL5, CAL6, CAL10},     //GROUP2  SENSOR1,3,4,5�@�[�@�Z���T2������
    { CAL2, CAL3, CAL6, CAL9},      //GROUP3  SENSOR1,2,4,5�@�[�@�Z���T3������
    { CAL1, CAL3, CAL5, CAL8},      //GROUP4  SENSOR1,2,3,5�@�[�@�Z���T4������
    { CAL1, CAL2, CAL4, CAL7},      //GROUP5  SENSOR1,2,3,4�@�[�@�Z���T5������
};


//
void    clearResult(uint8_t n){
    //�v�Z���ʂ��N���A
    calcValue[n].pattern = sensorOrderPattern(n);
    calcValue[n].impact_pos_x_mm = 0;
    calcValue[n].impact_pos_y_mm = 0;
    calcValue[n].radius0_mm = 0;
    calcValue[n].delay_time0_msec = 0;
    calcValue[n].status = 0;
}


void    resultError999(uint8_t n, calc_stat_sor_t status){
    //�G���[�̎��Aresult�ɃG���[���ʐ��l����������
    //calcValue[n].impact_pos_x_mm = 999.99;/////////////////////////////////////////���Z�̂��߃I�t
    //calcValue[n].impact_pos_y_mm = 999.99;
    //calcValue[n].radius0_mm = 999.99;
    calcValue[n].delay_time0_msec = 0;        //�^�}���j�ł̃G���[����Ɏg�p
    calcValue[n].status = status;
}


//
calc_stat_sor_t computeEpicenter(void){
    //���e���W�̌v�Z
    //�o�́@result[]:�v�Z���ʍ��Wx,y,r  �O���[�o��
    //return value calc_stat_sor_t�ɂ��
    
#define     DEV_XY      3.0     //�΍� ����l�@xy
#define     NUM_RES     4       //1�O���[�v���̕΍��v�Z�l�̐�
    
    calc_stat_sor_t  calcStat = CALC_STATUS_OK;
    uint8_t     i;
    uint8_t     calcNum;
    uint8_t     grNum;
    uint8_t     cnt;                //���ϒl�T���v�����̃J�E���g,���ʂ̃J�E���g
    float       tmpX;
    float       tmpY;
    float       tmpR;
    float       dist2[NUM_RES];     //������2��
    
    
    //�v�Z���ʃN���A
    for (i = 0; i < NUM_CAL; i++){
        clearResult(i);
    }
    calcResult.pattern = 0xffff;    //���ϒl���ʃ}�[�N

    
    //5�̃Z���T����3��I��Ōv�Z����@10�Ƃ���
    for (i = 0; i < NUM_CAL; i++){
        calcStat = computeXY(i);    //���W���v�Z
    }
    
    //------result�Ɍv�Z���ʂ�����[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
        
    //***** V5_edition�@****************************************************
    //VERIANCE CHECK�@���U�`�F�b�N
    //�Z���T�[4�I����5�O���[�v�ɂ��ā@4�̌v�Z���ʐ��l�̂΂�����v�Z
    
    for (grNum = 0; grNum < NUM_GROUP; grNum++){
        calcStat = CALC_STATUS_OK;                          //error clear
        groupVari[grNum].pattern = sensorGroupePattern(grNum);
        
        //4�̌v�Z���ʂ̕���
        cnt = 0;
        tmpX = 0;
        tmpY = 0;
        tmpR = 0;
        for(i = 0; i < NUM_RES; i++){
            calcNum = calcValue4[grNum][i];   //�v�Z�i���o�[
            if (CALC_STATUS_OK == calcValue[calcNum].status){
                tmpX += calcValue[calcNum].impact_pos_x_mm;
                tmpY += calcValue[calcNum].impact_pos_y_mm; 
                tmpR += calcValue[calcNum].radius0_mm;
                cnt ++;
            }else {
                //�v�Z�l���s�������ꍇ���όv�Z�Ɋ܂߂Ȃ�
                calcStat = CALC_STATUS_AVERAGE_ERR;
                ledLightOn(LED_CAUTION);
                printf("group%1d.d%1d: ave sample error!\n", (grNum + 1), (i + 1));

            }
        } // for-i loop 
        
        groupVari[grNum].sample_n = cnt;       //���ϒl�̃T���v����:����Ȃ�4
        if (cnt == 0){
            //�T���v����0�̎�
            groupVari[grNum].average_pos_x_mm   = 999.99;
            groupVari[grNum].average_pos_y_mm   = 999.99; 
            groupVari[grNum].average_radius0_mm = 999.99;
            calcStat = CALC_STATUS_AVERAGE_ERR;
            ledLightOn(LED_CAUTION);
            printf("group%1d No sample!\n", (grNum + 1));

        }else{
            //���ϒl
            groupVari[grNum].average_pos_x_mm   = tmpX / cnt;
            groupVari[grNum].average_pos_y_mm   = tmpY / cnt; 
            groupVari[grNum].average_radius0_mm = tmpR / cnt; 
        }
        
        //4�v�Z���ʂ̕��U
        for(i = 0; i < NUM_RES; i++){
            calcNum = calcValue4[grNum][i];
            if (CALC_STATUS_OK == calcValue[calcNum].status){
                //�v�ZOK
                tmpX = calcValue[calcNum].impact_pos_x_mm - groupVari[grNum].average_pos_x_mm;
                tmpY = calcValue[calcNum].impact_pos_y_mm - groupVari[grNum].average_pos_y_mm;
                dist2[i] = tmpX * tmpX + tmpY * tmpY;   //����(�΍�)��2��

                if (dist2[i] > (DEV_XY * DEV_XY)){      //�΂�����͈͂𒴂��Ă���
                    calcStat = CALC_STATUS_DEV_OVER;
                    ledLightOn(LED_CAUTION);
                    printf("group%1d.d%1d: dev over!\n", (grNum + 1), (i + 1));
                }
                groupVari[grNum].status = calcStat;
            }else{
                calcStat = CALC_STATUS_CAL_ERROR;
                ledLightOn(LED_CAUTION);
                printf("group%1d.d%1d: dev calc error!\n", (grNum + 1), (i + 1));
                dist2[i] = 99.999;
            }
            
        } // for-i loop 
        
        groupVari[grNum].dist1_mm2 = dist2[0];          //����(�΍�)��2������[
        groupVari[grNum].dist2_mm2 = dist2[1];
        groupVari[grNum].dist3_mm2 = dist2[2];
        groupVari[grNum].dist4_mm2 = dist2[3];
        
        tmpR = 0;
        for (i = 0; i < NUM_RES; i++){
            tmpR += dist2[i];
        }
        groupVari[grNum].variance = tmpR / NUM_RES;     //���U
        groupVari[grNum].status = calcStat;
        
        
    } //for-groupe loop
    //----------���ʔ���p�̕��U��groupeVari�ɓ���------------------
    
    
    //*************************�΂���̏��ʂ̌v�Z**********************************
    for (grNum = 0; grNum < NUM_GROUP; grNum++){
        cnt = 0;
        for (i = 0; i < NUM_GROUP; i++){
            if (groupVari[grNum].sample_n < 4){
                //�T���v���������Ȃ���
                cnt = 8;                ////���� -> 9�̕\�����o��悤��8�ɂ���B
                continue;
            }
            if (groupVari[grNum].variance > groupVari[i].variance){
                //�����̒l��菬�����l�����������J�E���g����
                cnt++;
                //�����l�����������͓������ł��Ă��܂�......������////////////////////
            }
        }
        groupVari[grNum].order = cnt;
        if (cnt == 0){
            calcNum = grNum;        //calcNum:�΂�����ŏ��̃O���[�v�ԍ�  ->�@�̗p�l
        }
    }

    //���ʂ̎��[
    calcResult.impact_pos_x_mm  = groupVari[calcNum].average_pos_x_mm;
    calcResult.impact_pos_y_mm  = groupVari[calcNum].average_pos_y_mm;
    calcResult.radius0_mm       = groupVari[calcNum].average_radius0_mm;
    calcResult.delay_time0_msec = impact_time_msec(calcResult.radius0_mm);    //���e����Z���T�I���܂ł̒x�ꎞ��(���e�����v�Z�p)
    calcResult.status           = calcStat;
    
    return calcStat;
}


calc_stat_sor_t computeXY(uint8_t calNum){
    //tmp3sensor��3�Z���T�f�[�^������W�l���v�Z
    //Input  calNum:3�̃Z���T��I������p�^�[���ԍ�  
    //Output result:�v�Z�l
    //       calc_stat:��ԁ@0-OK,
    calc_stat_sor_t  calcStat = CALC_STATUS_OK;
    
    //tmp3sensor�Ɍv�Z�p�f�[�^���
    if (select3sensor(calNum)){
        //�������f�[�^���_���Ȏ�
        calcStat = CALC_STATUS_NOT_ENOUGH;
        resultError999(calNum, calcStat);
        ledLightOn(LED_CAUTION);
        printf("sensor data select error!\n");
        return calcStat;
    }

    //�g�p�Z���T�[�̔ԍ��𐔎���
    calcValue[calNum].pattern = sensorOrderPattern(calNum);
    
    //���W�̌v�Z
    if (apollonius3circleXYR(calNum) != 0){
        //�v�Z���_���ȂƂ�
        calcStat = CALC_STATUS_CAL_ERROR;
        resultError999(calNum, calcStat);
        ledLightOn(LED_CAUTION);
        printf("- calculation error!\n");
        return calcStat;
    }
    
    //�v�Z���ʂ̔���
    if (calcValue[calNum].radius0_mm > R_MAX){
        //r���傫������
        calcStat = CALC_STATUS_R0_ERR;
        resultError999(calNum, calcStat);
        ledLightOn(LED_CAUTION);
        printf(" r0:%f ", calcValue[calNum].radius0_mm);        
        printf("CAL%1d:r0 is too large!\n", (calNum + 1));
        return calcStat;
    }
    if ((calcValue[calNum].impact_pos_x_mm < -TARGET_WIDTH_HALF) || (calcValue[calNum].impact_pos_x_mm > TARGET_WIDTH_HALF)){
        //x���傫������
        calcStat = CALC_STATUS_X0_ERR;
        resultError999(calNum, calcStat);
        ledLightOn(LED_CAUTION);
        printf(" x0:%f ", calcValue[calNum].impact_pos_x_mm);        
        printf("CAL%1d:x0 is too large!\n", (calNum + 1));
        return calcStat;
    }
    if ((calcValue[calNum].impact_pos_y_mm < -TARGET_HEIGHT_HALF) || (calcValue[calNum].impact_pos_y_mm > TARGET_HEIGHT_HALF)){
        //y���傫������
        calcStat = CALC_STATUS_Y0_ERR;
        resultError999(calNum, calcStat);
        ledLightOn(LED_CAUTION);
        printf(" y0:%f ", calcValue[calNum].impact_pos_y_mm);        
        printf("CAL%1d:y0 is too large!\n", (calNum + 1));
        return calcStat;
    }
    
    calcValue[calNum].delay_time0_msec = impact_time_msec(calcValue[calNum].radius0_mm);    //���e����Z���T�I���܂ł̒x�ꎞ��(���e�����v�Z�p)

    return calcStat;
}


uint8_t     select3sensor(uint8_t calNum){
    //�Z���T3�P��I�����đ��
    //�o��tmp3SensorFactor  -�v�Z�p��3�g�̃f�[�^�z��
    //stat 0:OK, 1:����s��
    uint8_t i;
    calc_stat_sor_t  calcStat = CALC_STATUS_OK;
   
    //����l���
    for (i = 0; i < 3; i++){
        if (SENSOR_STATUS_OK == sensor5Measure[sensor3outOf5[calNum][i]].status){
            tmp3SensorFactor[i] = sensor5Measure[sensor3outOf5[calNum][i]];       ///////////////////////////////�����̔z����T�u�Ɏw��--global�ɂ��Ȃ��悤�ɂł��Ȃ���?
        //�ꊇ��� .sensor_x, .sensor_y, .sensor_z, .distance_mm
        } else{
            //�f�[�^���_���Ȏ�
            calcStat = CALC_STATUS_NOT_ENOUGH;
            calcValue[calNum].status = calcStat;
            ledLightOn(LED_CAUTION);
            printf("sensor%2d data error!\n", (sensor3outOf5[calNum][i] + 1));
            return calcStat;
        }
    }
    
    return calcStat;
}


uint16_t    sensorOrderPattern(uint8_t calNum){
    //�g�p�Z���T�i���o�[��3����BCD�����ɂ��ċL�^
    return (sensor3outOf5[calNum][0] + 1) * 0x100 +
           (sensor3outOf5[calNum][1] + 1) * 0x10 +
           (sensor3outOf5[calNum][2] + 1);
}


uint16_t    sensorGroupePattern(uint8_t calNum){
    //�g�p�Z���T�i���o�[��4����BCD�����ɂ��ċL�^
    //123456789A �@A=10
    return (calcValue4[calNum][0] + 1) * 0x1000 +
           (calcValue4[calNum][1] + 1) * 0x100 + 
           (calcValue4[calNum][2] + 1) * 0x10 +
           (calcValue4[calNum][3] + 1);
}


uint8_t firstSensor(void){
    //�ŏ��̓��̓Z���T�[�̔ԍ�
    uint8_t i;
    for(i = SENSOR1; i < NUM_SENSOR; i++){
        if(sensor5Measure[i].input_order == 0){
            break;
        }
    }
    return i;
}



//*** calculation sub *****************

uint8_t apollonius3circleXYR(uint8_t numResult){
    //���W�̌v�Z
    //���^�[���l�@0:����
    //          1:�G���[
    //���͒l tmp3SensorFactor[3]: �Z���T�[�f�[�^ x�Ay�Az�A�x�ꋗ��
    //�o�͒l calcResult[]: ���Wx�Ay�Ƌ���r0�̌v�Z�l
    //      numResult: ���ʂ�������ԍ�
    
    uint8_t i;
    calc_stat_sor_t  calc_stat = CALC_STATUS_OK;
    
    float   x[4], y[4], z[4], dr[4];    //�Y����1,2,3���g�p
    float   a[4], b[4], c[4], d[4];
    float   e, f[3], g[3];
    float   aa, bb, cc, q;
    float   x0, y0, r0;
    
    //�f�[�^���  ---�z��Y����0���g��Ȃ�
    for (i = 0; i < 3; i++){
        x[i + 1] = tmp3SensorFactor[i].sensor_x_mm;
        y[i + 1] = tmp3SensorFactor[i].sensor_y_mm;
        z[i + 1] = tmp3SensorFactor[i].sensor_z_mm;
        dr[i + 1] = tmp3SensorFactor[i].distance_mm;
#ifdef  DEBUG_APO
    printf("x%1d:%5.1f y%1d:%5.1f z%1d:%5.1f dr%1d:%8.4f\n", i + 1, x[i + 1], i + 1, y[i + 1], i + 1, z[i + 1], i + 1, dr[i + 1]);
#endif
    }
    
    //�萔�̌v�Z
    a[1] = x[2] - x[1];
    a[2] = x[3] - x[2];
    a[3] = x[1] - x[3];
    b[1] = y[2] - y[1];
    b[2] = y[3] - y[2];
    b[3] = y[1] - y[3];
    c[1] = dr[2] - dr[1];   //dr�͑���l�Ȃ̂ł��̓s�x�Ⴄ
    c[2] = dr[3] - dr[2];
    c[3] = dr[1] - dr[3];
    
    e = a[1] * b[2] - a[2] * b[1];
    
#ifdef  DEBUG_APO
    printf("a1:%10.3f a2:%10.3f a3:%10.3f \n", a[1], a[2], a[3]);
    printf("b1:%10.3f b2:%10.3f b3:%10.3f \n", b[1], b[2], b[3]);
    printf("c1:%10.3f c2:%10.3f c3:%10.3f \n", c[1], c[2], c[3]);
    printf("e:%11.3f\n", e);
#endif
    if (e == 0){
        //�Z���T���꒼���ɕ��Ԏ���e=0�ƂȂ�
        //���ꂪ�[���ƂȂ�v�Z�ł��Ȃ�
        printf("CAL%1d: straight line\n", (numResult + 1));
        
        e = b[1] * c[2] - b[2] * c[1];      //�ʂ̌����ŉ���
        
#ifdef  DEBUG_APO
        printf("est:%11.3f\n", e);
#endif
        if (e == 0){
            //�ʌ����ł��_���Ȏ�
            calc_stat = CALC_STATUS_E_ZERO;
            calcValue[numResult].status = calc_stat;
            ledLightOn(LED_CAUTION);
            printf("CAL%1d: Est is also zero! ", numResult);
        }
        d[1] = (0 - y[1] * y[1] + y[2] * y[2] + dr[1] * dr[1] - dr[2] * dr[2]) / 2;
        d[2] = (0 - y[2] * y[2] + y[3] * y[3] + dr[2] * dr[2] - dr[3] * dr[3]) / 2;
        d[3] = (0 - y[3] * y[3] + y[1] * y[1] + dr[3] * dr[3] - dr[1] * dr[1]) / 2;
    
        r0 = (b[1] * d[2] - b[2] * d[1]) / e;
        y0 = (0 - c[1] * r0 + d[1]) / b[1];
        
#ifdef  DEBUG_APO
        printf("d1st:%10.3f d2st:%10.3f d3st:%10.3f \n", d[1], d[2], d[3]);
        printf("r0:%10.3f y0:%10.5f \n", r0, y0);
#endif    
        aa = 1;
        bb = -2 * x[1];
        cc = x[1] * x[1] + (y0 - y[1]) * (y0 - y[1]) - (r0 + dr[1]) * (r0 + dr[1]) + z[1] * z[1];
        
        q = (bb * bb) - (4 * aa * cc);      //���̌����̏���

#ifdef  DEBUG_APO
        printf("A:%10.6f B:%10.5f C:%10.3f \n", aa, bb, cc);
        printf("Q:%10.3f \n", q);
#endif    
        if (q < 0){
            //���̌����̏������݂����Ȃ�
            calc_stat = CALC_STATUS_QUAD_F;
            calcValue[numResult].status = calc_stat;
            ledLightOn(LED_CAUTION);
            printf("CAL%1d: Qst is less than zero! ", (numResult + 1));
            return calc_stat;
        }

        //�񎟕�������������x,y,r�����߂�v�Z
        x0 = (-bb - sqrt(q)) / (2 * aa);

#ifdef  DEBUG_APO
        printf("x0:%10.5f ", x0);
#endif    
        if ((x0 < -TARGET_WIDTH_HALF) || (x0 > TARGET_WIDTH_HALF)){
            //�͈͊O�̎��A�s�̗p
            x0 = (-bb + sqrt(q)) / (2 * aa);
#ifdef  DEBUG_APO
        printf("invalid -> x0:%10.5f ", x0);
#endif  
            if ((x0 < -TARGET_WIDTH_HALF) || (x0 > TARGET_WIDTH_HALF)){
                //2�Ƃ��͈͊O
                calc_stat = CALC_STATUS_X0_INVALID;
                calcValue[numResult].status = calc_stat;
                ledLightOn(LED_CAUTION);
                printf("CAL%1d: x0s is out of range! ", numResult);
#ifdef  DEBUG_APO
                prinf("invalid! \n");
#endif
                return calc_stat;
            }
        }  
     
    }else {     //(e == 0)
        //3�̃Z���T�[���꒼���ł͂Ȃ��O�p�`�z�u�̎�
        
        d[1] = (0 - x[1] * x[1] + x[2] * x[2] - y[1] * y[1] + y[2] * y[2] + dr[1] * dr[1] - dr[2] * dr[2]) / 2;
        d[2] = (0 - x[2] * x[2] + x[3] * x[3] - y[2] * y[2] + y[3] * y[3] + dr[2] * dr[2] - dr[3] * dr[3]) / 2;
        d[3] = (0 - x[3] * x[3] + x[1] * x[1] - y[3] * y[3] + y[1] * y[1] + dr[3] * dr[3] - dr[1] * dr[1]) / 2;

        f[1] = (b[1] * c[2] - b[2] * c[1]) / e;
        f[2] = (a[2] * c[1] - a[1] * c[2]) / e;
        g[1] = (b[2] * d[1] - b[1] * d[2]) / e;
        g[2] = (a[1] * d[2] - a[2] * d[1]) / e;

#ifdef  DEBUG_APO
        printf("d1:%10.3f d2:%10.3f d3:%10.3f \n", d[1], d[2], d[3]);
        printf("f1:%10.6f f2:%10.6f \n", f[1], f[2]);
        printf("g1:%10.6f g2:%10.6f \n", g[1], g[2]);
#endif
        
        //���̌���
        aa = (f[1] * f[1]) + (f[2] * f[2]) - 1;
        bb = (2 * f[1]) * (g[1] - x[1]) + (2 * f[2]) * (g[2] - y[1]) - (2 * dr[1]);
        cc = (g[1] - x[1]) * (g[1] - x[1]) + (g[2] - y[1]) * (g[2] - y[1]) + (z[1] * z[1]) - (dr[1] * dr[1]);

        q = (bb * bb) - (4 * aa * cc);      //���̌����̏���

#ifdef  DEBUG_APO
        printf("A:%10.6f B:%10.5f C:%10.3f \n", aa, bb, cc);
        printf("Q:%10.3f \n", q);
#endif    

        if (q < 0){
            //���̌����̏������݂����Ȃ�
            calc_stat = CALC_STATUS_QUAD_F;
            calcValue[numResult].status = calc_stat;
            ledLightOn(LED_CAUTION);
            printf("CAL%1d: Q is less than zero! ", (numResult + 1));
            return calc_stat;
        }

        //�񎟕�������������x,y,r�����߂�v�Z
        r0 = (-bb - sqrt(q)) / (2 * aa);

#ifdef  DEBUG_APO
        printf("r0:%10.3f ", r0);
#endif    
        if (r0 < 0){
            //���a�����̕��͕s�̗p
            r0 = (-bb + sqrt(q)) / (2 * aa);
#ifdef  DEBUG_APO
            printf("XXX  -> r0:%10.3f ", r0);
#endif  
            if (r0 < 0){
                //2�Ƃ��[��
                calc_stat = CALC_STATUS_R0_UNDER0;
                calcValue[numResult].status = calc_stat;
                ledLightOn(LED_CAUTION);
                printf("CAL%1d: R0 is less than zero! ", numResult);
#ifdef  DEBUG_APO
                printf("XXX  -> r0 is no solution! \n");
#endif  
                return calc_stat;
            }
        }
        
        x0 = f[1] * r0 + g[1];
        y0 = f[2] * r0 + g[2];
        
    }   // (e != 0)

    calcValue[numResult].radius0_mm = r0;
    calcValue[numResult].impact_pos_x_mm = x0;
    calcValue[numResult].impact_pos_y_mm = y0;
    
#ifdef  DEBUG_APO
    printf("x:%10.5f y:%10.5f \n", calcValue[numResult].impact_pos_x_mm, calcValue[numResult].impact_pos_y_mm);
    CORETIMER_DelayMs(2);
#endif
    
    return calc_stat;
}


