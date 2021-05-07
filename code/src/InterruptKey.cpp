#include "defs.h"
#include "pins.h"
#include "InterruptKey.h"
#include "SleepTimer.h"
#include "analogRead.h"
#include "display.h"
#include "CurveFitting.h"
void onKeyPress(uint8_t key, uint8_t event, uint8_t count, uint16_t length)
{
    if (event != EVENT_RELEASED)
    {
        return;
    }
    int keycode = 0; /*0:press 1:left 2:right 3:up 4:down*/
    cancelSleep();
    switch (key)
    {
    case SW:
        keycode = KEYCODE_RIGHT;
        break;
    case DT:
    case TX:
        keycode = KEYCODE_LEFT;
        break;
    case CLK:
    case RX:
        keycode = KEYCODE_Enter;

        if (length < 1000)
        {
            handleEnter();
        }
        else
        {
            handleEnterLongPress();
            Serial.print("handleEnterLongPress");
        }
        return;
    default:
        return;
    }

    //buzzer(50);
    switch (display_count)
    {
    case HOME:
        homeMove(keycode);
        cancelSleep();
        break;
    case SETTINGS:
        settingsMove(keycode);
        break;
    case SETTINGS_PID:
        pidMove(keycode);
        break;
    case SETTINGS_SLEEP:
        sleepMove(keycode);
        break;
    case SETTINGS_OLED:
        oledMove(keycode);
        break;
    case SETTINGS_POWER:
        powerMove(keycode);
        break;
    case SETTINGS_T12:
        solderMove(keycode);
        break;
    case SETTINGS_CALIBRATION:
        calibrationMove(keycode);
        break;
    default:
        return;
    }
}

void handleEnter()
{
    //在主界面的按下决策
    if (display_count == HOME)
        zjm_sw_key();
    //在设置界面的按下决策
    else if (display_count == SETTINGS)
        settings_sw_key();
    //在PID界面的按下决策
    else if (display_count == SETTINGS_PID)
        pid_sw_key();
    //在休眠界面的按下决策
    else if (display_count == SETTINGS_SLEEP)
        sleep_sw_key();
    //在OLED亮度界面的按下决策
    else if (display_count == SETTINGS_OLED)
        oled_sw_key();
    //在电源校准界面的按下决策
    else if (display_count == SETTINGS_POWER)
        power_sw_key();
    //在烙铁界面的按下决策
    else if (display_count == SETTINGS_T12)
        laotie_sw_key();
    //在校准界面的按下决策
    else if (display_count == SETTINGS_CALIBRATION)
        jiaozhun_sw_key();
}
void handleEnterLongPress()
{
    if (display_count == HOME)
    {
        display_count = SETTINGS; //在主界面时进入设置界面
        //buzzer(200);
    }
    else if (display_count != SETTINGS_CALIBRATION)
    {
        display_count = HOME; //在其他界面是退出至主界面
        //buzzer(200);
    }
}

//************************ 主界面 ************************
void zjm_sw_key()
{
    if (vin_error == 0 && t12_error == 0) // 启动或停止加热
    {
        zjm_dh_run = 1;                //允许播放动画 ，动画播放完在DisplayMain里切换T12工作状态
        zjm_sleep_ts = 0;              //提示主界面退出显示休眠状态
        buzzer_temp_average_state = 0; //准备好到达温度提示声
    }
}
void homeMove(int keycode)
{
    if (keycode == KEYCODE_LEFT)
        set_temp += 10;
    else if (keycode == KEYCODE_RIGHT)
        set_temp -= 10;
    calcTempRange(); //自动获取设置温度的范围
    if (set_temp > set_temp_max)
        set_temp = set_temp_min;
    else if (set_temp < set_temp_min)
        set_temp = set_temp_max;
}
//************************ 设置界面 ************************
void settings_sw_key() //设置界面 按下
{

    if (settings_kxxk_count == 0)
        display_count = SETTINGS_SLEEP; //进入休眠设置菜单
    else if (settings_kxxk_count == -5 || settings_kxxk_count == 1)
        display_count = SETTINGS_PID; //进入PID设置菜单
    else if (settings_kxxk_count == -1 || settings_kxxk_count == 5)
        display_count = SETTINGS_OLED; //进入OELD亮度设置菜单
    else if (settings_kxxk_count == -2 || settings_kxxk_count == 4)
        display_count = SETTINGS_POWER; //进入电源设置菜单
    else if (settings_kxxk_count == -4 || settings_kxxk_count == 2)
        display_count = SETTINGS_T12; //进入烙铁设置菜单
    else if (settings_kxxk_count == -3 || settings_kxxk_count == 3)
    {
        //使用比较柔和的pid参数，退出校准界面时记得恢复正常的PID策略
        p = p_jz;                             //比例系数
        i = i_jz;                             //积分系数
        d = d_jz;                             //微分系数
        display_count = SETTINGS_CALIBRATION; //进入烙铁设置菜单
    }
}
void settingsMove(int keycode)
{
    if (keycode == KEYCODE_RIGHT)
    {
        if (settings_kxxk_L == 0 && settings_kxxk_R == 0)
        {
            settings_kxxk_L = 1;   //允许播放动画
            settings_kxxk_count++; //下一项
        }
    }
    else if (keycode == KEYCODE_LEFT)
    {
        if (settings_kxxk_L == 0 && settings_kxxk_R == 0)
        {
            settings_kxxk_R = 1;   //允许播放动画
            settings_kxxk_count--; //上一项
        }
    }
}
//************************ PID设置界面 ************************
void pid_sw_key() //设置界面 按下
{

    switch (pidSettings_kxxk_count)
    {
    case 0: //返回至设置界面
        display_count = SETTINGS;
        break;
    case 1: //p 存储
        if (pidSettings_sxxk_count == 0)
            pidSettings_sxxk_count = pidSettings_kxxk_count;
        else if (pidSettings_sxxk_count == pidSettings_kxxk_count && dec_state == 0)
            dec_state = 1;
        else if (pidSettings_sxxk_count == pidSettings_kxxk_count && dec_state == 1)
        {
            EEPROM.put(p_eeprom, p);
            dec_state = 0;
            pidSettings_sxxk_count = 0;
        }
        break;
    case 2: //i 存储
        if (pidSettings_sxxk_count == 0)
            pidSettings_sxxk_count = pidSettings_kxxk_count;
        else if (pidSettings_sxxk_count == pidSettings_kxxk_count && dec_state == 0)
            dec_state = 1;
        else if (pidSettings_sxxk_count == pidSettings_kxxk_count && dec_state == 1)
        {
            EEPROM.put(i_eeprom, i);
            dec_state = 0;
            pidSettings_sxxk_count = 0;
        }
        break;
    case 3: //d 存储
        if (pidSettings_sxxk_count == 0)
            pidSettings_sxxk_count = pidSettings_kxxk_count;
        else if (pidSettings_sxxk_count == pidSettings_kxxk_count && dec_state == 0)
            dec_state = 1;
        else if (pidSettings_sxxk_count == pidSettings_kxxk_count && dec_state == 1)
        {
            EEPROM.put(d_eeprom, d);
            dec_state = 0;
            pidSettings_sxxk_count = 0;
        }
        break;
    }
}

void pidMove(int keycode)
{
    float symbol = 1;
    float unit = 1;
    if (keycode == KEYCODE_LEFT)
    {
        symbol = 1;
    }
    if (keycode == KEYCODE_RIGHT)
    {
        symbol = -1;
    }
    if (dec_state == 0)
    {
        unit = 1;
    }
    if (dec_state == 1)
    {
        unit = 0.1;
    }
    switch (pidSettings_sxxk_count)
    {

    case 0:
        pidSettings_kxxk_count += symbol;
        break;
    case 1:
        p += symbol * unit;
        break;
    case 2:
        i += symbol * unit;
        break;
    case 3:
        i += symbol * unit;
        break;
    }
    if (pidSettings_kxxk_count > 3)
        pidSettings_kxxk_count = 0;
    else if (pidSettings_kxxk_count < 0)
        pidSettings_kxxk_count = 3;
}
//************************ 休眠设置界面 ************************
void sleep_sw_key() //设置界面 按下
{

    switch (sleepSettings_kxxk_count)
    {
    case 0: //返回至设置界面
        display_count = SETTINGS;
        break;
    case 1: //休眠时间 存储
        if (sleepSettings_sxxk_count == 0)
            sleepSettings_sxxk_count = sleepSettings_kxxk_count;
        else if (sleepSettings_sxxk_count == sleepSettings_kxxk_count)
        {
            EEPROM.put(sleep_time_eeprom, sleep_time); //休眠时间
            sleepSettings_sxxk_count = 0;
        }
        break;
    case 2: //休眠温度 存储
        if (sleepSettings_sxxk_count == 0)
            sleepSettings_sxxk_count = sleepSettings_kxxk_count;
        else if (sleepSettings_sxxk_count == sleepSettings_kxxk_count)
        {
            EEPROM.put(sleep_temp_eeprom, sleep_temp); //休眠温度
            sleepSettings_sxxk_count = 0;
        }
        break;
    }
}
void sleepMove(int keycode) //设置界面 旋转
{
    if (sleepSettings_sxxk_count == 0) //选框移动
    {
        if (keycode == KEYCODE_LEFT)
            sleepSettings_kxxk_count++;
        else if (keycode == KEYCODE_RIGHT)
            sleepSettings_kxxk_count--;
    }
    else if (sleepSettings_sxxk_count == 1) //休眠时间加减
    {
        if (keycode == KEYCODE_LEFT)
            sleep_time += 30;
        else if (keycode == KEYCODE_RIGHT)
            sleep_time -= 30;
    }
    else if (sleepSettings_sxxk_count == 2) //休眠温度加减
    {
        if (keycode == KEYCODE_LEFT)
            sleep_temp += 1;
        else if (keycode == KEYCODE_RIGHT)
            sleep_temp -= 1;
    }
    if (sleepSettings_kxxk_count > 2)
        sleepSettings_kxxk_count = 0;
    else if (sleepSettings_kxxk_count < 0)
        sleepSettings_kxxk_count = 2;
    else if (sleep_time > 999)
        sleep_time = 30;
    else if (sleep_time < 30)
        sleep_time = 999;
}
//************************ OLED亮度设置界面 ************************
void oled_sw_key() //设置界面 按下
{

    switch (oledSettings_kxxk_count)
    {
    case 0: //返回至设置界面
        display_count = SETTINGS;
        break;
    case 1: //OLED亮度 存储
        if (oledSettings_sxxk_count == 0)
            oledSettings_sxxk_count = oledSettings_kxxk_count;
        else if (oledSettings_sxxk_count == oledSettings_kxxk_count)
        {
            EEPROM.put(oled_ld_eeprom, oled_ld); //OLED亮度
            oledSettings_sxxk_count = 0;
        }
        break;
    case 2: //OLED亮度 存储
        if (oledSettings_sxxk_count == 0)
            oledSettings_sxxk_count = oledSettings_kxxk_count;
        else if (oledSettings_sxxk_count == oledSettings_kxxk_count)
        {
            EEPROM.put(oled_fx_eeprom, oled_fx); //OLED方向
            if (oled_fx == 1)
                u8g2.setDisplayRotation(U8G2_R2); //旋转180度
            else
                u8g2.setDisplayRotation(U8G2_R0); //0度，默认方向
            oledSettings_sxxk_count = 0;
        }
        break;
    case 3: //旋转编码器方向 存储
        if (oledSettings_sxxk_count == 0)
        {
            xzbmq_fx = !xzbmq_fx;
            EEPROM.put(xzbmq_fx_eeprom, xzbmq_fx); //OLED方向
            EEPROM.get(xzbmq_fx_eeprom, xzbmq_fx); //OLED方向
        }
        break;
    }
}

void oledMove(int keycode) //设置界面 旋转
{
    if (oledSettings_sxxk_count == 0) //选框移动
    {
        if (keycode == KEYCODE_LEFT)
            oledSettings_kxxk_count++;
        else if (keycode == KEYCODE_RIGHT)
            oledSettings_kxxk_count--;
        if (oledSettings_kxxk_count > 3)
            oledSettings_kxxk_count = 0;
        else if (oledSettings_kxxk_count < 0)
            oledSettings_kxxk_count = 3;
    }
    else if (oledSettings_sxxk_count == 1) //OLED亮度 加减
    {
        if (keycode == KEYCODE_LEFT)
            oled_ld += 5;
        else if (keycode == KEYCODE_RIGHT)
            oled_ld -= 5;
        if (oled_ld > 1 && oled_ld < 5)
            oled_ld = 5;
        else if (oled_ld == 0)
            oled_ld = 255;
    }
    else if (oledSettings_sxxk_count == 2) //OLED方向 状态切换
    {
        if (keycode == KEYCODE_LEFT)
            oled_fx = !oled_fx;
        else if (keycode == KEYCODE_RIGHT)
            oled_fx = !oled_fx;
    }
}
//************************ 电源校准界面 ************************
void power_sw_key() //设置界面 按下
{

    switch (powerSettings_kxxk_count)
    {
    case 0: //返回至设置界面
        display_count = SETTINGS;
        break;
    case 1: //基准电压 存储
        if (powerSettings_sxxk_count == 0)
            powerSettings_sxxk_count = powerSettings_kxxk_count;
        else if (powerSettings_sxxk_count == powerSettings_kxxk_count)
        {
            EEPROM.put(vcc_refer_eeprom, vcc_refer);
            powerSettings_sxxk_count = 0;
        }
        break;
    case 2: //电源电压校准值 存储
        if (powerSettings_sxxk_count == 0)
            powerSettings_sxxk_count = powerSettings_kxxk_count;
        else if (powerSettings_sxxk_count == powerSettings_kxxk_count)
        {
            EEPROM.put(vin_refer_eeprom, vin_refer);
            powerSettings_sxxk_count = 0;
        }
        break;
    case 3: //低压报警阈值 存储
        if (powerSettings_sxxk_count == 0)
            powerSettings_sxxk_count = powerSettings_kxxk_count;
        else if (powerSettings_sxxk_count == powerSettings_kxxk_count)
        {
            EEPROM.put(vin_low_eeprom, vin_low);
            powerSettings_sxxk_count = 0;
        }
        break;
    }
}

void powerMove(int keycode) //设置界面 旋转
{
    if (powerSettings_sxxk_count == 0) //选框移动
    {
        if (keycode == KEYCODE_LEFT)
            powerSettings_kxxk_count++;
        else if (keycode == KEYCODE_RIGHT)
            powerSettings_kxxk_count--;
        if (powerSettings_kxxk_count > 3)
            powerSettings_kxxk_count = 0;
        else if (powerSettings_kxxk_count < 0)
            powerSettings_kxxk_count = 3;
    }
    else if (powerSettings_sxxk_count == 1) //基准电压 加减
    {
        if (keycode == KEYCODE_LEFT)
            vcc_refer += 0.01;
        else if (keycode == KEYCODE_RIGHT)
            vcc_refer -= 0.01;
    }
    else if (powerSettings_sxxk_count == 2) //电源电压校准值 加减
    {
        if (keycode == KEYCODE_LEFT)
            vin_refer += 0.1;
        else if (keycode == KEYCODE_RIGHT)
            vin_refer -= 0.1;
    }
    else if (powerSettings_sxxk_count == 3) //低压报警阈值 加减
    {
        if (keycode == KEYCODE_LEFT)
            vin_low += 0.1;
        else if (keycode == KEYCODE_RIGHT)
            vin_low -= 0.1;
        if (vin_low > 30)
            vin_low = 3.2;
        else if (vin_low < 3.2)
            vin_low = 30;
    }
}

//************************ 烙铁界面 ************************
void laotie_sw_key() //设置界面 按下
{

    switch (t12Settings_kxxk_count)
    {
    case 0: //返回至设置界面
        display_count = SETTINGS;
        break;
    case 1: //冷端补偿 存储
        if (t12Settings_sxxk_count == 0)
            t12Settings_sxxk_count = t12Settings_kxxk_count;
        else if (t12Settings_sxxk_count == t12Settings_kxxk_count)
        {
            EEPROM.put(tc1047_refer_eeprom, tc1047_refer);
            t12Settings_sxxk_count = 0;
        }
        break;
    case 2: //开机加热 存储
        if (t12Settings_sxxk_count == 0)
        {
            t12_heat_on_start = !t12_heat_on_start;
            EEPROM.put(t12_heat_on_start_eeprom, t12_heat_on_start);
        }
        break;
    case 3: //恢复出厂设置 存储
        if (t12Settings_sxxk_count == 0)
            t12Settings_sxxk_count = t12Settings_kxxk_count;
        else if (t12Settings_sxxk_count == t12Settings_kxxk_count)
        {
            if (hfccsz_state == 1)
                showFactoryReset();
            t12Settings_sxxk_count = 0;
        }
        break;
    }
}
void solderMove(int keycode) //设置界面 旋转
{
    if (t12Settings_sxxk_count == 0) //选框移动
    {
        if (keycode == KEYCODE_LEFT)
            t12Settings_kxxk_count++;
        else if (keycode == KEYCODE_RIGHT)
            t12Settings_kxxk_count--;
        if (t12Settings_kxxk_count > 3)
            t12Settings_kxxk_count = 0;
        else if (t12Settings_kxxk_count < 0)
            t12Settings_kxxk_count = 3;
    }
    else if (t12Settings_sxxk_count == 1) //冷端补偿校准值 加减
    {
        if (keycode == KEYCODE_LEFT)
            tc1047_refer += 1;
        else if (keycode == KEYCODE_RIGHT)
            tc1047_refer -= 1;
        if (tc1047_refer > 99)
            tc1047_refer = 0;
        else if (tc1047_refer < -99)
            tc1047_refer = 0;
    }
    else if (t12Settings_sxxk_count == 3) //恢复出厂设置 状态切换
    {
        if (keycode == KEYCODE_LEFT)
            hfccsz_state = !hfccsz_state;
        else if (keycode == KEYCODE_RIGHT)
            hfccsz_state = !hfccsz_state;
    }
}
//************************ 校准界面 ************************
void jiaozhun_sw_key() //设置界面 按下
{

    switch (calibrationSettings_kxxk_count)
    {
    case 0: //返回至设置界面,并恢复正常PID参数和获取设置温度范围
        display_count = SETTINGS;
        calibrationSettings_kxxk_count = 0;
        EEPROM.get(p_eeprom, p); //pid-p
        EEPROM.get(i_eeprom, i); //pid-i
        EEPROM.get(d_eeprom, d); //pid-d

        break;
    case 1: //深圳校准温度1段 存储
        if (calibrationSettings_sxxk_count == 0)
        {
            calibrationSettings_sxxk_count = calibrationSettings_kxxk_count;
            t12_switch = 1; //打开加热
        }
        else if (calibrationSettings_sxxk_count == calibrationSettings_kxxk_count)
        {
            EEPROM.put(sz_temp0_eeprom, sz_temp[0]);
            calibrationSettings_sxxk_count = 0;
            t12_switch = 0; //关闭加热
        }
        break;
    case 2: //深圳校准温度2段 存储
        if (calibrationSettings_sxxk_count == 0)
        {
            calibrationSettings_sxxk_count = calibrationSettings_kxxk_count;
            t12_switch = 1; //打开加热
        }
        else if (calibrationSettings_sxxk_count == calibrationSettings_kxxk_count)
        {
            EEPROM.put(sz_temp1_eeprom, sz_temp[1]);
            calibrationSettings_sxxk_count = 0;
            t12_switch = 0; //关闭加热
        }
        break;
    case 3: //深圳校准温度3段 存储
        if (calibrationSettings_sxxk_count == 0)
        {
            calibrationSettings_sxxk_count = calibrationSettings_kxxk_count;
            t12_switch = 1; //打开加热
        }
        else if (calibrationSettings_sxxk_count == calibrationSettings_kxxk_count)
        {
            EEPROM.put(sz_temp2_eeprom, sz_temp[2]);
            calibrationSettings_sxxk_count = 0;
            t12_switch = 0; //关闭加热
        }
        break;
    case 4: //深圳校准温度4段 存储
        if (calibrationSettings_sxxk_count == 0)
        {
            calibrationSettings_sxxk_count = calibrationSettings_kxxk_count;
            t12_switch = 1; //打开加热
        }
        else if (calibrationSettings_sxxk_count == calibrationSettings_kxxk_count)
        {
            EEPROM.put(sz_temp3_eeprom, sz_temp[3]);
            calibrationSettings_sxxk_count = 0;
            t12_switch = 0; //关闭加热
        }
        break;
    case 5:             //开始曲线拟合系数计算
        curveFitting(); //计算曲线拟合系数
        calibrationSettings_kxxk_count = 6;
        break;
    case 6: //返回至设置界面,并恢复正常PID参数
        display_count = SETTINGS;
        calibrationSettings_kxxk_count = 0;
        EEPROM.get(p_eeprom, p); //pid-p
        EEPROM.get(i_eeprom, i); //pid-i
        EEPROM.get(d_eeprom, d); //pid-d
        break;
    }
}
void calibrationMove(int keycode) //设置界面 旋转
{
    if (calibrationSettings_sxxk_count == 0) //选框移动
    {
        if (keycode == KEYCODE_LEFT)
            calibrationSettings_kxxk_count++;
        else if (keycode == KEYCODE_RIGHT)
            calibrationSettings_kxxk_count--;
        if (calibrationSettings_kxxk_count > 6)
            calibrationSettings_kxxk_count = 0;
        else if (calibrationSettings_kxxk_count < 0)
            calibrationSettings_kxxk_count = 6;
    }
    else if (calibrationSettings_sxxk_count == 1) //选框移动
    {
        if (keycode == KEYCODE_LEFT)
            sz_temp[0] += 1;
        else if (keycode == KEYCODE_RIGHT)
            sz_temp[0] -= 1;
    }
    else if (calibrationSettings_sxxk_count == 2) //选框移动
    {
        if (keycode == KEYCODE_LEFT)
            sz_temp[1] += 1;
        else if (keycode == KEYCODE_RIGHT)
            sz_temp[1] -= 1;
    }
    else if (calibrationSettings_sxxk_count == 3) //选框移动
    {
        if (keycode == KEYCODE_LEFT)
            sz_temp[2] += 1;
        else if (keycode == KEYCODE_RIGHT)
            sz_temp[2] -= 1;
    }
    else if (calibrationSettings_sxxk_count == 4) //选框移动
    {
        if (keycode == KEYCODE_LEFT)
            sz_temp[3] += 1;
        else if (keycode == KEYCODE_RIGHT)
            sz_temp[3] -= 1;
    }
}
