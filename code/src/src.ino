/*项目代码修改自 甘草酸不酸 开源https://github.com/jie326513988/mini-T12
 * 屏幕驱动可改为SSD1316的0.87寸OLED
*/

// 默认支持深圳头，更换其他厂商的头需要自行校准温度曲线
// 若内存不足无法编译，检查是否为UNO的板子
// 并按照下面的链接修改U8G2库
// U8G2优化内存 https://github.com/olikraus/u8g2/wiki/u8g2optimization
// 本项目屏蔽了U8G2_WITH_CLIP_WINDOW_SUPPORT、U8G2_WITH_FONT_ROTATION、U8G2_WITH_INTERSECTION
// V1.10以下的版本不适合V2.2的PCB

/*
  按键功能定义
    长按操作（5下短音最后1下长音）
      主界面：进入设置界面
      其他界面：退出至主界面（校准界面除外）
    双击操作（2下短音）
      主界面：加热或停止状态切换
      其他界面：无
    单击（1下短音）
      主界面：无
      设置界面：进入二级菜单
      二级菜单：切换数值更改的选中状态（空心选框或实心选框），或确认更改数值，无选框状态则退出至一级菜单
  菜单选项
    PID
      P
      I
      D
    休眠
      休眠时间
      休眠温度
    屏幕
      屏幕亮度
      屏幕方向
      编码器方向
    电源
      基准电压
      电源电压
      低压报警
    校准
      调节曲线1段温度
      调节曲线2段温度
      调节曲线2段温度
      调节曲线3段温度
      运行曲线拟合并校准曲线
    烙铁
      冷端补偿
      开机加热
      重置
*/

//********** 版本号 **********
#define version "v2.30"

//#include <MsTimer2.h>
#include <EEPROM.h>    //掉电数值存储库
#include <avr/sleep.h> //休眠库
#include <U8g2lib.h>   //屏幕驱动库
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include <DebounceEvent.h>

U8G2_SSD1306_128X32_UNIVISION_2_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE); //SSD1306  0.96寸OLED
//U8G2_SSD1316_128X32_2_HW_I2C u8g2(U8G2_R0,/* reset=*/ U8X8_PIN_NONE);             //SSD1316  0.87寸OLED
#include "pins.h"
//调用字库
#include "font.c"
//声明外部变量
extern const uint8_t chinese_t12[1591] U8G2_FONT_SECTION("chinese19");

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
uint8_t auto_get = 0;                                 //自动刷写eeprom的状态存储，判断是否首次开机 0-首次 1-非首次
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
boolean t12_switch = 0;        //T12加热开关
boolean t12_heat_on_start = 0; //是否开机就加热？ 0-否 1-是

float t12_ad = 0.0;           //T12即时ADC值
int16_t t12_temp = 0;         //T12即时温度
int16_t t12_temp_display = 0; //T12显示温度

boolean t12_error = 0;     //手柄未接提示
boolean t12_adc_error = 0; //温度超出测量范围
int16_t set_temp_max;      //自动计算设置的温度最大值
int16_t set_temp_min;      //自动计算设置的温度最小值

int16_t t12_temp_average_cache = 0;    //记录采集温度，缓存用
int16_t t12_temp_average = 0;          //8次内采集到的平均温度，显示用
uint8_t t12_temp_average_count = 0;    //平均温度采集计数
boolean buzzer_temp_average_state = 0; //到达温度提示声

//float tc1047_ad = 0.0;      //tc1047ADC值
float tc1047_temp = 0.0;  //tc1047温度，机内温度
int8_t tc1047_refer = 20; //T12环境参考温度校准值

float vcc_refer = 4.93;                 //参考电压，稳压芯片输出的电压是多少就填多少
float vin = 0.0;                        //电源电压
float vin_refer = 0.0;                  //输入电压校准值
float vin_low = 3.3;                    //低压报警阈值
boolean vin_error = 0;                  //低压报警提示
#define vcc_refer_fl vcc_refer / 1023.0 //参考电压每级分量

uint32_t vin_read_time = 0;        //定时读取输入电压时间
uint32_t t12_temp_read_time = 450; //定时T12温度采样时间

boolean buzzer_state = 0;          //蜂鸣器准备状态
uint16_t buzzer_time;              //蜂鸣器响的时间
uint16_t buzzer_count;             //蜂鸣器响的次数
boolean buzzer_settings_state = 0; //进入设置界面的长音状态
boolean buzzer_zjm_state = 0;      //退出至主界面的长音状态

boolean zjm_sleep_ts = 0;  //主界面休眠提示  0-无 1-休眠中
uint16_t sleep_time = 150; //进入休眠的时间阈值,单位秒
uint8_t sleep_temp = 200;  //休眠时的温度，单位摄氏度
boolean sleep_state = 0;   //休眠引脚状态 0低电平 1高电平

uint16_t sleep_temp_cache = 0; //休眠前的温度缓存

uint8_t oled_ld = 50;     //OLED亮度
boolean oled_fx = 0;      //OLED方向 0-不旋转，横向显示 1-旋转180度
boolean xzbmq_fx = 0;     //编码器方向 0-顺加逆减 1-顺减逆加
boolean hfccsz_state = 0; //恢复出厂设置 0-NO 1-YES

boolean xp_state = 0; //息屏状态 0-无 1-息屏
int8_t xp_x = 0;      //息屏随机变化位置X
int8_t xp_y = 0;      //息屏随机变化位置Y
int8_t xp_x_old = 50; //上一次息屏随机变化位置X
int8_t xp_y_old = 21; //上一次息屏随机变化位置Y

//深圳头曲线拟合的4段ADC
#define sz_adc0 10
#define sz_adc1 350
#define sz_adc2 650
#define sz_adc3 940 //原950
//uint16_t sz_temp[4] = {36, 224, 347, 446};   //深圳头曲线拟合的4段温度   Y 390倍
//uint16_t sz_temp[4] = {43, 259, 382, 490};   //深圳头曲线拟合的4段温度   Y 330倍
uint16_t sz_temp[4] = {49, 262, 391, 486}; //深圳头曲线拟合的4段温度   Y    1.03版本程序移植
float sz_p[4];                             //深圳头曲线拟合的4个系数   P
//********** 菜单标志位相关 **********
#define HOME 1                 //主界面
#define SETTINGS 2             //设置界面
#define SETTINGS_PID 3         //PID设置界面
#define SETTINGS_SLEEP 4       //休眠设置界面
#define SETTINGS_OLED 5        //OLED设置界面
#define SETTINGS_POWER 6       //电源设置界面
#define SETTINGS_CALIBRATION 7 //校准设置界面
#define SETTINGS_T12 8         //烙铁头设置界面

uint8_t display_count = 1; //默认要显示的界面
boolean dec_state = 0;     //小数点输入状态 0关闭 1开启
uint8_t qj_ca_count = 0;   //全局长按计数

boolean settings_kxxk_L = 0;    //设置界面空心选框 顺时针转
boolean settings_kxxk_R = 0;    //设置界面空心选框 逆时针转
int8_t settings_kxxk_count = 0; //设置界面空心选框

int8_t pidSettings_kxxk_count = 0; //pid界面空心选框
int8_t pidSettings_sxxk_count = 0; //pid界面实心选框

int8_t sleepSettings_kxxk_count = 0; //pid界面空心选框
int8_t sleepSettings_sxxk_count = 0; //pid界面实心选框

int8_t oledSettings_kxxk_count = 0; //oled界面空心选框
int8_t oledSettings_sxxk_count = 0; //oled界面实心选框

int8_t powerSettings_kxxk_count = 0; //电源设置界面空心选框
int8_t powerSettings_sxxk_count = 0; //电源设置界面实心选框

int8_t settings_pwm_kxxk_count = 0; //PWM界面空心选框
int8_t settings_pwm_sxxk_count = 0; //PWM界面实心选框

int8_t t12Settings_kxxk_count = 0; //烙铁头设置界面空心选框
int8_t t12Settings_sxxk_count = 0; //烙铁头设置界面实心选框

int8_t calibrationSettings_kxxk_count = 0; //烙铁头设置界面空心选框
int8_t calibrationSettings_sxxk_count = 0; //烙铁头设置界面实心选框
//********** 动态调参PID **********
float p = 25.0; //比例系数
float i = 8.7;  //积分系数
float d = 2.6;  //微分系数

#define p_jz 12.0 //校准界面用的 比例系数
#define i_jz 2.2  //校准界面用的 积分系数
#define d_jz 1.2  //校准界面用的 微分系数

uint16_t set_temp = 320;          //设置温度
int16_t thisError = 0;            //本次误差
int16_t thisError_average = 0;    //累积误差的平均值
int16_t lasterror = 0;            //前一拍偏差
int16_t preerror = 0;             //前两拍偏差
const int8_t deadband = 0;        //死区
int16_t pid_out = 0.0;            //PID控制器计算结果
uint8_t pid_out_bfb = 0.0;        //输出值0-100%
const float pwm_max = 2047.0;     //输出值上限
const float pwm_min = 0.0;        //输出值下限
const float errorabsmax = 2000.0; //偏差绝对值最大值
const float errorabsmin = 50.0;   //偏差绝对值最小值
const float alpha = 0.2;          //不完全微分系数
float deltadiff = 0.0;            //微分增量

#define ADCMax 1000

void initPins()
{
    pinMode(SW, INPUT_PULLUP);            //右键
    pinMode(RX, INPUT_PULLUP);            //按下
    pinMode(TX, INPUT_PULLUP);            //左键
    pinMode(t12_temp_pin, INPUT);         //t12温度
    pinMode(t12_sleep_pin, INPUT_PULLUP); //t12休眠
    pinMode(vin_pin, INPUT);              //电源电压
    pinMode(tc1047_pin, INPUT);           //机内温度
    pinMode(buzzer_pin, OUTPUT);          //蜂鸣器
    pinMode(t12_pwm_pin, OUTPUT);         //t12加热，不可更改

    digitalWrite(t12_pwm_pin, 0); //默认关闭
    digitalWrite(buzzer_pin, 0);  //默认关闭
}

void initDisplay()
{
    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.setContrast(oled_ld); //设置亮度1-255
    //u8g2.setBusClock(400000); //设置I2C通讯速度默认MAX400000
    //u8g2.setPowerSave(0);     //0开启屏幕 1关闭屏幕
    oled_fx ? u8g2.setDisplayRotation(U8G2_R2) : u8g2.setDisplayRotation(U8G2_R0);
    showLogo();
}

void initADC()
{
    //ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);  // 设置ADC分频 128倍
    ADCSRA |= bit(ADPS1) | bit(ADPS2); //设置ADC分频 64倍
    ADCSRA |= bit(ADIE);               // 开启ADC中断
}

#define CUSTOM_DEBOUNCE_DELAY 10
#define CUSTOM_REPEAT_DELAY 100

DebounceEvent rightBtn = DebounceEvent(SW, onKeyPress, BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP, CUSTOM_DEBOUNCE_DELAY, CUSTOM_REPEAT_DELAY);
DebounceEvent enterBtn = DebounceEvent(RX, onKeyPress, BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP, CUSTOM_DEBOUNCE_DELAY, CUSTOM_REPEAT_DELAY);
DebounceEvent leftBtn = DebounceEvent(TX, onKeyPress, BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP, CUSTOM_DEBOUNCE_DELAY, CUSTOM_REPEAT_DELAY);
DebounceEvent enterBtn2 = DebounceEvent(CLK, onKeyPress, BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP, CUSTOM_DEBOUNCE_DELAY, CUSTOM_REPEAT_DELAY);
DebounceEvent leftBtn2 = DebounceEvent(DT, onKeyPress, BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP, CUSTOM_DEBOUNCE_DELAY, CUSTOM_REPEAT_DELAY);

void initT12()
{

    for (uint8_t i = 0; i < 10; i++) //读取初始值
    {
        processT12();
    }
}

void setup()
{
    //***** 首次开机自动刷写eeprom
    initEEPROM();
    //***** 编码器配置
    initPins();
    //***** OLED初始化
    initDisplay();
    //***** ADC初始化
    initADC();
    //***** 中断初始化
    //initInterrupt();
    //***** 播放开机画面和开机声
    startTune();
    //读取电压/温度
    initT12();
    //自动获取设置温度的范围
    calcTempRange();
    //***** 开启看门狗
    //set_wdt_mod(1, 7);
    //***** 是否开机加热
    t12_heat_on_start == 1 ? t12_switch = 1 : t12_switch = 0;

    //***** 开机计算曲线拟合系数
    curveFitting();
}

void loop()
{
    __asm__ __volatile__("wdr"); //看门狗复位

    leftBtn.loop();
    rightBtn.loop();
    enterBtn.loop();
    leftBtn2.loop();
    enterBtn2.loop();

    showPage();   //刷新屏幕
    buzzer_run(); //蜂鸣器运行决策
    onPIDTimer(); //读取温度电压执行PID
    checkSleep(); //睡眠检测
}
