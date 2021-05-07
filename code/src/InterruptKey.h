
#ifndef _INTERRUPTKEYS_H_
#define _INTERRUPTKEYS_H_

#include <Arduino.h>
#include <EEPROM.h>
//#include <DebounceEvent.h>

#define KEYCODE_Enter 0
#define KEYCODE_RIGHT 1
#define KEYCODE_LEFT 2
#define KEYCODE_UP 3
#define KEYCODE_DOWN 4

typedef struct KeyStatus
{
    bool status;
    bool ready = false;
    bool reset_count = true;
    unsigned long event_start;
    unsigned long event_length;
    unsigned char event_count = 0;
};

void onKeyPress(uint8_t key, uint8_t event, uint8_t count, uint16_t length);
void keyLoop();
void checkKey(uint8_t pin, KeyStatus &status);
void handleMove(int keycode);
void handleEnter();
void handleEnterLongPress();

//************************ 主界面 ************************
void zjm_sw_key();
void homeMove(int keycode);
//************************ 设置界面 ************************
void settings_sw_key();
void settingsMove(int keycode);
//************************ PID设置界面 ************************
void pid_sw_key();

void pidMove(int keycode);
//************************ 休眠设置界面 ************************
void sleep_sw_key();
void sleepMove(int keycode);
//************************ OLED亮度设置界面 ************************
void oled_sw_key();

void oledMove(int keycode);
//************************ 电源校准界面 ************************
void power_sw_key();
void powerMove(int keycode);

//************************ 烙铁界面 ************************
void laotie_sw_key();
void solderMove(int keycode);
//************************ 校准界面 ************************
void jiaozhun_sw_key();
void calibrationMove(int keycode);
#endif