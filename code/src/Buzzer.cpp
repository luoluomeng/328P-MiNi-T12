//********** 控制蜂鸣器发声的频率和时间，系统延时法，占用系统速度大 **********
#include "defs.h"
#include "buzzer.h"
void buzzer(uint16_t timems)
{
    buzzer_state = 1;
    buzzer_time = millis() + timems;
}

void buzzer_run()
{
    pinMode(buzzer_pin, OUTPUT);
    if (buzzer_state == 1 && millis() < buzzer_time)
    {
        digitalWrite(buzzer_pin, 1);
    }
    else
    {
        digitalWrite(buzzer_pin, 0);
    }

    //错误提示报警音
    if (vin_error == 1 || t12_adc_error == 1)
    {
        if (millis() - error_buzzer_db_time > 1000)
        {
            buzzer(200);
            error_buzzer_db_time = millis();
        }
    }
}

void startTune()
{
    pinMode(buzzer_pin, OUTPUT);

    digitalWrite(buzzer_pin, 1);
    delay(50);
    digitalWrite(buzzer_pin, 0);
    delay(200);
    digitalWrite(buzzer_pin, 1);
    delay(50);
    digitalWrite(buzzer_pin, 0);
}