#ifndef _DISPLAY_MAIN_H_
#define _DISPLAY_MAIN_H_
#include <arduino.h>
void homePage();

void zjm_vin_ec_temp_state();
void zjm_error1();
void zjm_error2();
void zjm_error3();

uint8_t get_num_digit(uint32_t num);
#endif