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

#include "defs.h"
//#include <MsTimer2.h>


#include "InterruptKey.h"
#include "buzzer.h"
#include "sleeptimer.h"
#include "PID.h"
#include "WDT.h"
#include "E2PROM.h"
#include "InterruptKey.h"
#include "Display.h"
#include "analogRead.h"
#include "CurveFitting.h"
#include "E2PROM.h"

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
