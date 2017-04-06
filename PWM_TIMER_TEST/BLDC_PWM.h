/*
 * Radio tranfered stats RVG stucture
 */
#ifndef BLDC_PWM_h
#define BLDC_PWM_h
#include <Arduino.h>

 #define AH  6
 #define AL  5
 #define BH  9
 #define BL  10
 #define CH  11
 #define CL  3
 #define EN  4

int8_t BLDC_CYCLE( bool STATE1, bool STATE2, bool STATE3);
void BLDC_INIT();


#endif

