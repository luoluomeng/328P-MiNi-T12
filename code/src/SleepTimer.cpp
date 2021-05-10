#include "defs.h"
#include "pins.h"
#include "SleepTimer.h"
#include "buzzer.h"
#include <EEPROM.h>

boolean enable_reed_switch = false;

void checkSleep()
{
    if (sleep_time == 999)
    {
        enable_reed_switch = true;
        return;
    }
    else
    {
        enable_reed_switch = false;
    }
    if (last_move_time == 0)
    {
        last_move_time = millis() / 1000;
    }
    time_past = millis() / 1000 - last_move_time;
    if (isMoved())
    {
        last_move_time = millis() / 1000;
        cancelSleep();
    }
    else
    {
        processSystemSleep(); //系统休眠决策
        processStopHeat();
        processSleep();
    }
}

void processSleep()
{
    if (t12_switch == 1) //烙铁启动，打开休眠判断决策
    {
        if (enable_reed_switch && zjm_sleep_ts == 0)
        {
            if (!(digitalRead(t12_sleep_pin)))
            {
                triggerSleep();
            }
            return;
        }
        if (time_past >= sleep_time && zjm_sleep_ts == 0)
        {
            triggerSleep();
        }
    }
    // else if (t12_switch == 0)
    // {

    //}
}

void processStopHeat()
{
    if (time_past > HEAT_STOP_TIME) //10分钟没操作，关闭加热
    {
        stopHeat();
    }
}
bool isMoved()
{
    if (enable_reed_switch)
    {
        if (digitalRead(t12_sleep_pin))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    if (sleep_state != digitalRead(t12_sleep_pin)) //烙铁动了，计数清零
    {
        sleep_state = !sleep_state; //状态取反

        return true;
    }
    return false;
}
//********** 系统休眠判断决策 **********
void processSystemSleep()
{
    //Serial.println("processSystemSleep");
    //Serial.println(time_past);
    if (time_past >= SCTRRN_SAVER_TIME) //无加热无操作超过180秒就显示息屏界面
    {

        if (millis() / 1000 - screen_saver_past >= 3) //每隔3秒刷新一次位置
        {
            display_count = HOME; //退出至主界面
            xp_state = 1;         //标记息屏状态
            xp_x = random(0, 90);
            xp_y = random(11, 31);
            screen_saver_past = millis() / 1000;
        }
    }
}

void cancelSleep()
{

    last_move_time = millis() / 1000;

    time_past = 0;

    zjm_sleep_ts = 0; //提示主界面退出显示休眠状态
    if (sleep_temp_cache != 0)
    {
        set_temp = sleep_temp_cache;
    }
    sleep_temp_cache = 0;
    if (display_count == HOME)
    {
        t12_adc_error = 0; //清除温度超出测量范围提示
    }
    xp_state = 0;                  //结束息屏
    buzzer_temp_average_state = 0; //准备好到达温度提示声
    buzzer(200);
}

void triggerSleep()
{
    EEPROM.put(set_temp_eeprom, set_temp); //自动保存当前设置温度
    zjm_sleep_ts = 1;                      //提示主界面显示休眠中
    buzzer(500);

    if (set_temp > sleep_temp) //大于设置的休眠温度
    {
        sleep_temp_cache = set_temp; //休眠前将温度记录下来，记录和调温
        set_temp = sleep_temp;       //将温度调至休眠温度
    }
    else if (set_temp <= sleep_temp) //小于等于设置的休眠温度，只记录不调温
    {
        sleep_temp_cache = set_temp;
    } //休眠前将温度记录下来
}

void stopHeat()
{
    set_temp = sleep_temp_cache;
    sleep_temp_cache = 0;
    t12_switch = 0;
    zjm_sleep_ts = 0; //提示主界面退出休眠
    buzzer(1000);
}
