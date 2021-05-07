#ifndef _DEFS_H_
#define _DEFS_H_

#include <arduino.h>

#define version "v2.30"

#include <U8g2lib.h> //屏幕驱动库
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include "pins.h"
extern U8G2_SSD1306_128X32_UNIVISION_2_HW_I2C u8g2; //SSD1306  0.96寸OLED
//U8G2_SSD1316_128X32_2_HW_I2C u8g2(U8G2_R0,/* reset=*/ U8X8_PIN_NONE);             //SSD1316  0.87寸OLED
//声明外部变量
extern const uint8_t chinese_t12[1591];

//********** 引脚定义 **********
#define SW PIN2  //右
#define CLK PIN1 //按下
#define DT PIN32 //左
#define RX PIN30 //按下
#define TX PIN31 //左
//除了10号脚不能改，其他都可以改，但模拟引脚只能改模拟引脚（休眠引脚除外）
#define t12_sleep_pin PIN16 //t12休眠   模拟读取引脚
#define t12_temp_pin PIN25  //A2 t12温度   模拟读取引脚
#define tc1047_pin PIN26    //A3 机内温度   模拟读取引脚
#define vin_pin PIN19       //A6 电源电压   模拟读取引脚
#define t12_pwm_pin PIN14   //t12加热    pwm输出引脚，定死不可更改
#define buzzer_pin PIN12    //蜂鸣器     输出引脚
//************ EEPROM地址定义 ************
extern uint8_t auto_get;                              //自动刷写eeprom的状态存储，判断是否首次开机 0-首次 1-非首次
#define auto_get_eeprom 0                             //自动刷写eeprom的状态存储，1位
#define p_eeprom 1                                    //p地址，4位
#define i_eeprom p_eeprom + 4                         //i地址，4位
#define d_eeprom i_eeprom + 4                         //d地址，4位
#define sleep_time_eeprom d_eeprom + 4                //休眠时间，4位
#define sleep_temp_eeprom sleep_time_eeprom + 4       //休眠温度，1位
#define oled_ld_eeprom sleep_temp_eeprom + 1          //OLED亮度，1位
#define vcc_refer_eeprom oled_ld_eeprom + 1           //基准电压，4位
#define vin_refer_eeprom vcc_refer_eeprom + 4         //电源电压校准值，4位
#define t12_heat_on_start_eeprom vin_refer_eeprom + 4 //是否开机就加热？，1位
#define vin_low_eeprom t12_heat_on_start_eeprom + 1   //输入电压低压报警阈值，4位
#define tc1047_refer_eeprom vin_low_eeprom + 4        //冷端补偿校准值，1位
#define oled_fx_eeprom tc1047_refer_eeprom + 1        //OELD方向，1位
#define xzbmq_fx_eeprom oled_fx_eeprom + 1            //编码器旋转方向，1位
#define sz_temp0_eeprom xzbmq_fx_eeprom + 1           //深圳头曲线温度校准1段，4位
#define sz_temp1_eeprom sz_temp0_eeprom + 4           //深圳头曲线温度校准2段，4位
#define sz_temp2_eeprom sz_temp1_eeprom + 4           //深圳头曲线温度校准3段，4位
#define sz_temp3_eeprom sz_temp2_eeprom + 4           //深圳头曲线温度校准4段，4位
#define set_temp_eeprom sz_temp3_eeprom + 4           //t12设置温度，4位
//T12及温度相关
extern boolean t12_switch;        //T12加热开关
extern boolean t12_heat_on_start; //是否开机就加热？ 0-否 1-是

extern float t12_ad;             //T12即时ADC值
extern int16_t t12_temp;         //T12即时温度
extern int16_t t12_temp_display; //T12显示温度

extern boolean t12_error;     //手柄未接提示
extern boolean t12_adc_error; //温度超出测量范围
extern int16_t set_temp_max;  //自动计算设置的温度最大值
extern int16_t set_temp_min;  //自动计算设置的温度最小值

extern int16_t t12_temp_average_cache;    //记录采集温度，缓存用
extern int16_t t12_temp_average;          //8次内采集到的平均温度，显示用
extern uint8_t t12_temp_average_count;    //平均温度采集计数
extern boolean buzzer_temp_average_state; //到达温度提示声

//float tc1047_ad = 0.0;      //tc1047ADC值
extern float tc1047_temp;   //tc1047温度，机内温度
extern int8_t tc1047_refer; //T12环境参考温度校准值

extern float vcc_refer;                 //参考电压，稳压芯片输出的电压是多少就填多少
extern float vin;                       //电源电压
extern float vin_refer;                 //输入电压校准值
extern float vin_low;                   //低压报警阈值
extern boolean vin_error;               //低压报警提示
#define vcc_refer_fl vcc_refer / 1023.0 //参考电压每级分量

extern uint32_t vin_read_time;      //定时读取输入电压时间
extern uint32_t t12_temp_read_time; //定时T12温度采样时间

extern boolean buzzer_state;          //蜂鸣器准备状态
extern uint16_t buzzer_time;          //蜂鸣器响的时间
extern uint16_t buzzer_count;         //蜂鸣器响的次数
extern boolean buzzer_settings_state; //进入设置界面的长音状态
extern boolean buzzer_zjm_state;      //退出至主界面的长音状态

extern boolean zjm_sleep_ts; //主界面休眠提示  0-无 1-休眠中
extern uint16_t sleep_time;  //进入休眠的时间阈值,单位秒
extern uint8_t sleep_temp;   //休眠时的温度，单位摄氏度
extern boolean sleep_state;  //休眠引脚状态 0低电平 1高电平

extern uint16_t sleep_temp_cache; //休眠前的温度缓存

extern uint8_t oled_ld;      //OLED亮度
extern boolean oled_fx;      //OLED方向 0-不旋转，横向显示 1-旋转180度
extern boolean xzbmq_fx;     //编码器方向 0-顺加逆减 1-顺减逆加
extern boolean hfccsz_state; //恢复出厂设置 0-NO 1-YES

extern boolean xp_state; //息屏状态 0-无 1-息屏
extern int8_t xp_x;      //息屏随机变化位置X
extern int8_t xp_y;      //息屏随机变化位置Y
extern int8_t xp_x_old;  //上一次息屏随机变化位置X
extern int8_t xp_y_old;  //上一次息屏随机变化位置Y

//深圳头曲线拟合的4段ADC
#define sz_adc0 10
#define sz_adc1 350
#define sz_adc2 650
#define sz_adc3 940 //原950
//uint16_t sz_temp[4] = {36, 224, 347, 446};   //深圳头曲线拟合的4段温度   Y 390倍
//uint16_t sz_temp[4] = {43, 259, 382, 490};   //深圳头曲线拟合的4段温度   Y 330倍
extern uint16_t sz_temp[4]; //深圳头曲线拟合的4段温度   Y    1.03版本程序移植
extern float sz_p[4];       //深圳头曲线拟合的4个系数   P
//********** 菜单标志位相关 **********
#define HOME 1                 //主界面
#define SETTINGS 2             //设置界面
#define SETTINGS_PID 3         //PID设置界面
#define SETTINGS_SLEEP 4       //休眠设置界面
#define SETTINGS_OLED 5        //OLED设置界面
#define SETTINGS_POWER 6       //电源设置界面
#define SETTINGS_CALIBRATION 7 //校准设置界面
#define SETTINGS_T12 8         //烙铁头设置界面

extern uint8_t display_count; //默认要显示的界面
extern boolean dec_state;     //小数点输入状态 0关闭 1开启
extern uint8_t qj_ca_count;   //全局长按计数

extern boolean settings_kxxk_L;    //设置界面空心选框 顺时针转
extern boolean settings_kxxk_R;    //设置界面空心选框 逆时针转
extern int8_t settings_kxxk_count; //设置界面空心选框

extern int8_t pidSettings_kxxk_count; //pid界面空心选框
extern int8_t pidSettings_sxxk_count; //pid界面实心选框

extern int8_t sleepSettings_kxxk_count; //pid界面空心选框
extern int8_t sleepSettings_sxxk_count; //pid界面实心选框

extern int8_t oledSettings_kxxk_count; //oled界面空心选框
extern int8_t oledSettings_sxxk_count; //oled界面实心选框

extern int8_t powerSettings_kxxk_count; //电源设置界面空心选框
extern int8_t powerSettings_sxxk_count; //电源设置界面实心选框

extern int8_t settings_pwm_kxxk_count; //PWM界面空心选框
extern int8_t settings_pwm_sxxk_count; //PWM界面实心选框

extern int8_t t12Settings_kxxk_count; //烙铁头设置界面空心选框
extern int8_t t12Settings_sxxk_count; //烙铁头设置界面实心选框

extern int8_t calibrationSettings_kxxk_count; //烙铁头设置界面空心选框
extern int8_t calibrationSettings_sxxk_count; //烙铁头设置界面实心选框
//********** 动态调参PID **********
extern float p; //比例系数
extern float i; //积分系数
extern float d; //微分系数

#define p_jz 12.0 //校准界面用的 比例系数
#define i_jz 2.2  //校准界面用的 积分系数
#define d_jz 1.2  //校准界面用的 微分系数

extern uint16_t set_temp;         //设置温度
extern int16_t thisError;         //本次误差
extern int16_t thisError_average; //累积误差的平均值
extern int16_t lasterror;         //前一拍偏差
extern int16_t preerror;          //前两拍偏差
extern const int8_t deadband;     //死区
extern int16_t pid_out;           //PID控制器计算结果
extern uint8_t pid_out_bfb;       //输出值0-100%
extern const float pwm_max;       //输出值上限
extern const float pwm_min;       //输出值下限
extern const float errorabsmax;   //偏差绝对值最大值
extern const float errorabsmin;   //偏差绝对值最小值
extern const float alpha;         //不完全微分系数
extern float deltadiff;           //微分增量

#define ADCMax 1000

#define CUSTOM_DEBOUNCE_DELAY 10
#define CUSTOM_REPEAT_DELAY 100

//********** 画主界面 **********
extern uint8_t zjm_x1;
extern uint8_t zjm_x2;
#define zjm_x3 75
#define zjm_x4 zjm_x3 + 4

#define zjm_y1 7
#define zjm_y2 19
#define zjm_y3 32
#define zjm_y4 14
extern boolean zjm_dh_run;    //主界面动画状态 0-不允许 1-允许
extern uint32_t zjm_dh_time1; //动画1运转对比时间
extern uint32_t zjm_dh_time2; //动画2运转对比时间
//热、停切换动画变量
extern uint8_t zjm_dh_x1;
extern uint8_t zjm_dh_y1;
extern uint8_t zjm_dh_r1;
//pwm动效动画
extern uint8_t zjm_dh_l[14];
//开机过度动画y增量
extern int8_t zjm_dh_y;

extern uint16_t last_move_time;
extern uint16_t screen_saver_past;
extern uint16_t time_past;
#define HEAT_STOP_TIME 600    //seconds
#define SCTRRN_SAVER_TIME 180 //seconds
extern uint32_t error_buzzer_db_time;

extern float vin_cache;
extern uint8_t vin_cache_count;

extern float tc1047_cache;
extern uint8_t tc1047_cache_count;

#define cycs 10                       //时间平均滤波的采样次数 采样周期 = 采样次数*定时执行任务的时间
extern uint8_t t12_adc_error_count;   //t12ADC超出检测范围错误计数
extern uint32_t system_sleep_db_time; //系统进入睡眠对比时间

#define settings_y1 21
extern int16_t settings_dh_x[6];
extern int16_t settings_dh_x_count; //设置界面动画x增量记录

#define settings_dh_bc 5 //变化步长，只能为5的倍数

//********** PID设置界面 **********
#define pidSettings_x1 2
#define pidSettings_x2 14

#define pidSettings_x3 82
#define pidSettings_x4 94

#define pidSettings_y1 14
#define pidSettings_y2 29

//********** 休眠设置界面 **********
#define sleepSettings_x1 2
#define sleepSettings_x2 105
#define sleepSettings_y1 14
#define sleepSettings_y2 29

//********** OLED亮度设置界面 **********
#define oledSettings_x1 2
#define oledSettings_x2 106
#define oledSettings_x3 86
#define oledSettings_y1 14
#define oledSettings_y2 29

//********** 电源设置界面 **********
#define powerSettings_x1 2
#define powerSettings_x2 100
#define powerSettings_y1 14
#define powerSettings_y2 29

//********** 烙铁设置界面 **********
#define t12Settings_x1 2
#define t12Settings_x4 105

#define t12Settings_y1 14
#define t12Settings_y2 29

//********** 校准设置界面 **********
#define calibrationSettings_x1 0

#define calibrationSettings_x2 30
#define calibrationSettings_x3 54
#define calibrationSettings_x4 80
#define calibrationSettings_x5 106

#define calibrationSettings_x6 102

#define calibrationSettings_y1 7
#define calibrationSettings_y2 18
#define calibrationSettings_y3 31

#endif