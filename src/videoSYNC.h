/* 
 * File:   videoSYNC.h
 * 
 * 2024.03.15
 * 
 */

#ifndef VIDEOSYNC_H
#define	VIDEOSYNC_H


//
void    videoSync_Init(uint8_t);
void    videoSync_Ready(void);
void    videoSync_Start(void);
void    videoSync_Stop(void);
void    VIDEO_SYNC_Wired(void);
void    VIDEO_SYNC_PWM(void);

//test
void pwm_test(void);


#endif //VIDEOSYNC_H