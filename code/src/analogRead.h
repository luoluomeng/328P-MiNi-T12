#ifndef _ANALOG_READ_H_
#define _ANALOG_READ_H_
//********** 读取电压、电流、温度 **********
#include <arduino.h>
void onPIDTimer();
void processT12();

void readTemp();

bool checkIsT12Connected();

void readVin();

void readEnvTemp();
void calcTempRange();
uint16_t denoiseAnalog(byte port);
#endif
