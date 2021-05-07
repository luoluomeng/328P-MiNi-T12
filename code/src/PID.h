#ifndef _PID_H_
#define _PID_H_
#include <arduino.h>
//********** PID算法 **********
//算法来至https://blog.csdn.net/foxclever/article/details/105006205 By foxclever
void pid();

void PIDRegulator(float PV);

float get_factor(float ek0, float absmax, float absmin);

void pwm_1b(uint16_t val);
#endif