/*
 * File:   videoSYNC.c
 * 
 * ���˃^�C�~���O�A�r�f�I10�R�}���ƃ}�[�NLED��_��
 * 
 * OCMP���W���[���ɂ��PWM�o��
 * 
 * Revision history: 
 * 2024.03.15   main.c��蕪��
 * 2024.05.25   �X���[�v�O�ɃI�t����B
 * 
 */
#include "header.h"
#include "videoSYNC.h"


//local
uint32_t    tmr4Pr;                 //Timer4 Priod - PWM reset


void    videoSync_Init(uint8_t rate_fps){
    //�t���[�����[�g�̐ݒ�
    uint32_t d;
    
    if ((rate_fps <= 0) || (rate_fps > 100000)){
        printf("video %dfps error! --> 30fps", rate_fps);
        rate_fps = 30;
    }
    
    TMR4_Initialize();          //30fps 10flame = 330msec
    tmr4Pr = TMR4_PeriodGet() * 30 / rate_fps;
    OCMP1_Initialize();         //1flame = 33msec
    d = OCMP1_CompareSecondaryValueGet () * 30 / rate_fps;
    
    OCMP1_Enable();
    TMR4_PeriodSet(tmr4Pr);
    OCMP1_CompareSecondaryValueSet(d);
    videoSync_Ready();
    
}


void    videoSync_Off(void){
    //����I�t����...�X���[�v��
    OCMP1_Disable();
    TMR4_Stop();
}


void    videoSync_Ready(void){
    //�^�C�}�J�E���^�l���Z�b�g�@�ҋ@��L�A�X�^�[�g��H�ɂȂ�悤�ɂ��邽��
    TMR4 = tmr4Pr;
}


void    videoSync_Start(void){
    //LED�_�œ_���J�n
    TMR4_Start();       //OCMP(PWM)start
}


void    videoSync_Stop(void){
    //LED����
    uint32_t    cnt = tmr4Pr << 1;
    
    while(VIDEO_SYNC_OUT_Get()){
        //L�ɂȂ�̂�҂��Ď~�߂�
        cnt--;
        if (cnt <= 0){
            printf("PWMstop timeover!\n");
            break;
        }
    }
    TMR4_Stop();       //OCMP(PWM)stop
    videoSync_Ready();
}
    

//�ڑ��o�H�ؑ�
void    VIDEO_SYNC_Wired(void){
    //Wired(LAN.8) TamamoniPWM
    pt1ConWiFi = WIRED_LAN;
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_3);   //PT1 interrupt Enable
    CLC3_Enable(0);     //CLC2INA --> CLCO2 
}


void    VIDEO_SYNC_PWM(void){
    //OCMP1(PWM)
    pt1ConWiFi = WIRELESS_WIFI;
    EVIC_ExternalInterruptDisable(EXTERNAL_INT_3);  //PT1 interrupt Disable
    CLC3_Enable(1);     //OCMP1(PWM) --> CLCO2
}



////  T E S T  /////////////////////////

void pwm_test(void){
    
    printf("\n\n#### PWM TEST #####\n");
    bool st;
    st = CLC3CONbits.LCOUT;
    printf("startup -> CLC3CON.EN:%d\n", st);
    
#define WIRED
#ifdef  WIRED
    VIDEO_SYNC_Wired();
    st = CLC3CONbits.LCOUT;
    printf("Wired -> CLC3CON.EN:%d\n", st);
#else
    VIDEO_SYNC_PWM();        //startup - WiFi�����ڑ�
    st = CLC3CONbits.LCOUT;
    printf("Wireless -> CLC3CON.EN:%d\n", st);
#endif
    
    OCMP1_Enable ();    ////////////////////////////////
    uint32_t p = TMR4_PeriodGet();  //PWM period
    p = p * 3;  //1sec
    TMR4_PeriodSet(p);
    TMR4 = p;
    
    uint32_t  d = OCMP1_CompareSecondaryValueGet ();    //PWM On time
    d = d * 5 * 3;    //Duty 50%
    OCMP1_CompareSecondaryValueSet (d);
    
    printf("wait 2sec  \n");
    //TMR4 = 0;
    CORETIMER_DelayMs(2000);
    
    printf("PWM start\n");
    videoSync_Start();
    bool toggle = 1;
    
    while(1){
        if (PT1_Get()){
            CORETIMER_DelayMs(5);
            //�`���^�����O
            
            //�g�O��
            printf("pt1:Hi...");
            if (toggle == 1){
                while(VIDEO_SYNC_OUT_Get());        //�~�߂����͕K��L�ɂȂ�̂�҂�
                videoSync_Stop();
                printf("PWM stop  (TMR4 = %d)\n", TMR4_CounterGet());
                //init
                TMR4 = p;   //�^�C�}�J�E���^�l���Z�b�g
                toggle = 0;
            }else{
                videoSync_Start();
                printf("PWM start\n");
                toggle = 1;
            }
            CORETIMER_DelayMs(500);
        }
    }
    
}
            


/* *****************************************************************************
 End of File
 */
