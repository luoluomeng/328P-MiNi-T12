//********** 画设置主界面及其他界面 **********
#define settings_y1 21
int16_t settings_dh_x[6] = {0, 50, 100, 150, 200, 250};
int16_t settings_dh_x_count = 0; //设置界面动画x增量记录

#define settings_dh_bc 5 //变化步长，只能为5的倍数
void settingsHome()
{
    //画十字坐标
    //u8g2.drawHLine(0, 15, 128);
    //u8g2.drawVLine(63, 0, 32);
    //********** 线性滚动动画
    if (settings_kxxk_L == 1) //顺时针滚动
    {
        settings_dh_x_count += settings_dh_bc; //记录变化次数
        for (uint8_t i = 0; i < 6; i++)        //开始变化
        {
            settings_dh_x[i] += settings_dh_bc; //步长5
        }
        for (uint8_t i = 0; i < 6; i++) //数值限制
        {
            if (settings_dh_x[i] > 250)
                settings_dh_x[i] = -45;
        }
        if (settings_dh_x_count >= 50) //变化完毕
        {
            settings_kxxk_L = 0;     //释放状态
            settings_dh_x_count = 0; //变化次数清零
            if (settings_kxxk_count >= 6)
                settings_kxxk_count = 0; //数值限制
        }
    }
    else if (settings_kxxk_R == 1) //逆时针滚动
    {
        settings_dh_x_count -= settings_dh_bc; //记录变化次数
        for (uint8_t i = 0; i < 6; i++)        //开始变化
        {
            settings_dh_x[i] -= settings_dh_bc; //步长5
        }
        for (uint8_t i = 0; i < 6; i++) //数值限制
        {
            if (settings_dh_x[i] < -150)
                settings_dh_x[i] = 145;
        }
        if (settings_dh_x_count <= -50) //变化完毕
        {
            settings_kxxk_R = 0;     //释放状态
            settings_dh_x_count = 0; //变化次数清零
            if (settings_kxxk_count <= -6)
                settings_kxxk_count = 0; //数值限制
        }
    }

    u8g2.setFont(chinese_t12);
    if (settings_dh_x[0] >= -50 && settings_dh_x[0] <= 150)
    {
        u8g2.setCursor(settings_dh_x[0], settings_y1);
        u8g2.print("PID");
    }
    if (settings_dh_x[1] >= -50 && settings_dh_x[1] <= 150)
    {
        u8g2.setCursor(settings_dh_x[1], settings_y1);
        u8g2.print("休眠");
    }
    if (settings_dh_x[2] >= -50 && settings_dh_x[2] <= 150)
    {
        u8g2.setCursor(settings_dh_x[2], settings_y1);
        u8g2.print("屏幕");
    }
    if (settings_dh_x[3] >= -50 && settings_dh_x[3] <= 150)
    {
        u8g2.setCursor(settings_dh_x[3], settings_y1);
        u8g2.print("电源");
    }
    if (settings_dh_x[4] >= -50 && settings_dh_x[4] <= 150)
    {
        u8g2.setCursor(settings_dh_x[4], settings_y1);
        u8g2.print("校准");
    }
    if (settings_dh_x[5] >= -50 && settings_dh_x[5] <= 150)
    {
        u8g2.setCursor(settings_dh_x[5], settings_y1);
        u8g2.print("烙铁");
    }
    u8g2.setDrawColor(2);
    u8g2.drawRBox(43, 6, 40, 20, 1); //空心圆角方形
    u8g2.setDrawColor(1);
}

//********** PID设置界面 **********
#define pidSettings_x1 2
#define pidSettings_x2 14

#define pidSettings_x3 82
#define pidSettings_x4 94

#define pidSettings_y1 14
#define pidSettings_y2 29
void pidSettings()
{
    u8g2.setCursor(pidSettings_x1, pidSettings_y1);
    u8g2.print("P");
    u8g2.setCursor(pidSettings_x2, pidSettings_y1);
    u8g2.print(p, 1);

    u8g2.setCursor(pidSettings_x3, pidSettings_y1);
    u8g2.print("I");
    u8g2.setCursor(pidSettings_x4, pidSettings_y1);
    u8g2.print(i, 1);

    u8g2.setCursor(pidSettings_x1, pidSettings_y2);
    u8g2.print("D");
    u8g2.setCursor(pidSettings_x2, pidSettings_y2);
    u8g2.print(d, 1);

    //画选框
    switch (pidSettings_kxxk_count) //空心选框的位置
    {
    case 1: //p
        if (pidSettings_sxxk_count == pidSettings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(pidSettings_x1 - 2, pidSettings_y1 - 13, 48, 16); //实心方形
            //画水平线，标识进入小数点模式
            if (dec_state == 1)
                u8g2.drawHLine(pidSettings_x2 + 4 + (get_num_digit(p) * 6), pidSettings_y1 + 1, 10);
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(pidSettings_x1 - 2, pidSettings_y1 - 13, 48, 16); //空心方形
        break;
    case 2: //i
        if (pidSettings_sxxk_count == pidSettings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(pidSettings_x3 - 2, pidSettings_y1 - 13, 48, 16); //实心方形
            //画水平线，标识进入小数点模式
            if (dec_state == 1)
                u8g2.drawHLine(pidSettings_x4 + 4 + (get_num_digit(i) * 6), pidSettings_y1 + 1, 10);
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(pidSettings_x3 - 2, pidSettings_y1 - 13, 48, 16); //空心方形
        break;
    case 3: //d
        if (pidSettings_sxxk_count == pidSettings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(pidSettings_x1 - 2, pidSettings_y2 - 13, 48, 16); //实心方形
            //画水平线，标识进入小数点模式
            if (dec_state == 1)
                u8g2.drawHLine(pidSettings_x2 + 4 + (get_num_digit(d) * 6), pidSettings_y2 + 1, 10);
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(pidSettings_x1 - 2, pidSettings_y2 - 13, 48, 16); //空心方形
        break;
    }
}

//********** 休眠设置界面 **********
#define sleepSettings_x1 2
#define sleepSettings_x2 105
#define sleepSettings_y1 14
#define sleepSettings_y2 29
void sleepSettings()
{
    u8g2.setCursor(sleepSettings_x1, sleepSettings_y1);
    u8g2.print("休眠时间 S");
    u8g2.setCursor(sleepSettings_x2, sleepSettings_y1);
    if (sleep_time != 999)
        u8g2.print(sleep_time);
    else
        u8g2.print("关");

    u8g2.setCursor(sleepSettings_x1, sleepSettings_y2);
    u8g2.print("休眠温度 ℃");
    u8g2.setCursor(sleepSettings_x2, sleepSettings_y2);
    u8g2.print(sleep_temp);

    //画选框
    switch (sleepSettings_kxxk_count) //空心选框的位置
    {
    case 1: //设置温度
        if (sleepSettings_sxxk_count == sleepSettings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(sleepSettings_x1 - 2, sleepSettings_y1 - 13, 128, 16); //实心方形
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(sleepSettings_x1 - 2, sleepSettings_y1 - 13, 128, 16); //空心方形
        break;
    case 2: //设置温度
        if (sleepSettings_sxxk_count == sleepSettings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(sleepSettings_x1 - 2, sleepSettings_y2 - 13, 128, 16); //实心方形
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(sleepSettings_x1 - 2, sleepSettings_y2 - 13, 128, 16); //空心方形
        break;
    }
}

//********** OLED亮度设置界面 **********
#define oledSettings_x1 2
#define oledSettings_x2 106
#define oledSettings_x3 86
#define oledSettings_y1 14
#define oledSettings_y2 29
void oledSettings()
{
    if (oledSettings_kxxk_count == 3) //第2页
    {
        u8g2.setCursor(oledSettings_x1, oledSettings_y1);
        u8g2.print("旋转方向");
        u8g2.setCursor(oledSettings_x3, oledSettings_y1);
        if (xzbmq_fx == 0)
            u8g2.print("顺+逆-");
        else
            u8g2.print("顺-逆+");
    }
    else //第1页
    {
        u8g2.setCursor(oledSettings_x1, oledSettings_y1);
        u8g2.print("屏幕亮度");
        u8g2.setCursor(oledSettings_x2, oledSettings_y1);
        u8g2.print(oled_ld);

        u8g2.setCursor(oledSettings_x1, oledSettings_y2);
        u8g2.print("屏幕方向");
        u8g2.setCursor(oledSettings_x2, oledSettings_y2);
        if (oled_fx == 0)
            u8g2.print("0");
        else
            u8g2.print("180");
    }
    //画选框
    switch (oledSettings_kxxk_count) //空心选框的位置
    {
    case 1: //OLED亮度
        if (oledSettings_sxxk_count == oledSettings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(oledSettings_x1 - 2, oledSettings_y1 - 13, 128, 16); //实心方形
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(oledSettings_x1 - 2, oledSettings_y1 - 13, 128, 16); //空心方形
        break;
    case 2: //OLED方向
        if (oledSettings_sxxk_count == oledSettings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(oledSettings_x1 - 2, oledSettings_y2 - 13, 128, 16); //实心方形
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(oledSettings_x1 - 2, oledSettings_y2 - 13, 128, 16); //空心方形
        break;
    case 3:                                                                 //旋转方向
        u8g2.drawFrame(oledSettings_x1 - 2, oledSettings_y1 - 13, 128, 16); //空心方形
        break;
    }
}

//********** 电源设置界面 **********
#define powerSettings_x1 2
#define powerSettings_x2 100
#define powerSettings_y1 14
#define powerSettings_y2 29
void powerSettings()
{
    if (powerSettings_kxxk_count == 3) //第2页
    {
        u8g2.setCursor(powerSettings_x1, powerSettings_y1);
        u8g2.print("低压报警 V");
        u8g2.setCursor(powerSettings_x2, powerSettings_y1);
        if (vin_low >= 3.3)
            u8g2.print(vin_low, 1);
        else if (vin_low < 3.3)
            u8g2.print("关");
    }
    else //第1页
    {
        u8g2.setCursor(powerSettings_x1, powerSettings_y1);
        u8g2.print("基准电压 V");
        u8g2.setCursor(powerSettings_x2, powerSettings_y1);
        u8g2.print(vcc_refer);
        u8g2.setCursor(powerSettings_x1, powerSettings_y2);
        u8g2.print("电源电压 V");
        if (vin_refer >= 0)
        {
            u8g2.setCursor(powerSettings_x2 - 8, powerSettings_y2);
            u8g2.print("+");
            u8g2.setCursor(powerSettings_x2, powerSettings_y2);
            u8g2.print(vin_refer);
        }
        else
        {
            u8g2.setCursor(powerSettings_x2 - 6, powerSettings_y2);
            u8g2.print(vin_refer);
        }
    }
    //画选框
    switch (powerSettings_kxxk_count) //空心选框的位置
    {
    case 1: //基准电压(V)
        if (powerSettings_sxxk_count == powerSettings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(powerSettings_x1 - 2, powerSettings_y1 - 13, 128, 16); //实心方形
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(powerSettings_x1 - 2, powerSettings_y1 - 13, 128, 16); //空心方形
        break;
    case 2: //基准电压(V)
        if (powerSettings_sxxk_count == powerSettings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(powerSettings_x1 - 2, powerSettings_y2 - 13, 128, 16); //实心方形
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(powerSettings_x1 - 2, powerSettings_y2 - 13, 128, 16); //空心方形
        break;
    case 3: //电源电压(V)
        if (powerSettings_sxxk_count == powerSettings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(powerSettings_x1 - 2, powerSettings_y1 - 13, 128, 16); //实心方形
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(powerSettings_x1 - 2, powerSettings_y1 - 13, 128, 16); //空心方形
        break;
    }
}
//********** 烙铁设置界面 **********
#define t12Settings_x1 2
#define t12Settings_x4 105

#define t12Settings_y1 14
#define t12Settings_y2 29

void t12Settings()
{

    if (t12Settings_kxxk_count == 3) //第2页
    {
        u8g2.setCursor(t12Settings_x1, t12Settings_y1);
        u8g2.print("重置");
        u8g2.setCursor(t12Settings_x4, t12Settings_y1);
        if (hfccsz_state)
            u8g2.print("是");
        else
            u8g2.print("否");
    }
    else //第1页
    {
        u8g2.setCursor(t12Settings_x1, t12Settings_y1);
        u8g2.print("冷端补偿 ℃");
        if (tc1047_refer <= -10)
            u8g2.setCursor(t12Settings_x4, t12Settings_y1);
        else
            u8g2.setCursor(t12Settings_x4, t12Settings_y1);
        u8g2.print(tc1047_refer);

        u8g2.setCursor(t12Settings_x1, t12Settings_y2);
        u8g2.print("开机加热");
        u8g2.setCursor(t12Settings_x4, t12Settings_y2);
        if (t12_heat_on_start)
            u8g2.print("是");
        else
            u8g2.print("否");
    }

    //画选框
    switch (t12Settings_kxxk_count) //空心选框的位置
    {
    case 1: //冷端补偿
        if (t12Settings_sxxk_count == t12Settings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(t12Settings_x1 - 2, t12Settings_y1 - 13, 128, 16); //实心方形
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(t12Settings_x1 - 2, t12Settings_y1 - 13, 128, 16); //空心方形
        break;
    case 2:                                                               //开机加热
        u8g2.drawFrame(t12Settings_x1 - 2, t12Settings_y2 - 13, 128, 16); //空心方形
        break;
    case 3: //恢复出厂设置
        if (t12Settings_sxxk_count == t12Settings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(t12Settings_x1 - 2, t12Settings_y1 - 13, 128, 16); //实心方形
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(t12Settings_x1 - 2, t12Settings_y1 - 13, 128, 16); //空心方形
        break;
    }
}
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
void calibrationSettings()
{
    u8g2.setFont(u8g2_font_lucasfont_alternate_tr);
    if (calibrationSettings_kxxk_count == 6)
    {
        u8g2.setCursor(0, 7);
        u8g2.print("P0:");
        u8g2.setCursor(25, 7);
        u8g2.print(sz_p[0], 10);

        u8g2.setCursor(0, 15);
        u8g2.print("P1:");
        u8g2.setCursor(25, 15);
        u8g2.print(sz_p[1], 10);

        u8g2.setCursor(0, 24);
        u8g2.print("P2:");
        u8g2.setCursor(25, 24);
        u8g2.print(sz_p[2], 10);

        u8g2.setCursor(0, 32);
        u8g2.print("P3:");
        u8g2.setCursor(25, 32);
        u8g2.print(sz_p[3], 10);
    }
    else
    {
        u8g2.setCursor(calibrationSettings_x1, calibrationSettings_y1);
        u8g2.print("ADC   10   350  650  950");
        u8g2.setCursor(calibrationSettings_x1, calibrationSettings_y2);
        u8g2.print("Temp");
        u8g2.setCursor(calibrationSettings_x2, calibrationSettings_y2);
        u8g2.print(sz_temp[0]);
        u8g2.setCursor(calibrationSettings_x3, calibrationSettings_y2);
        u8g2.print(sz_temp[1]);
        u8g2.setCursor(calibrationSettings_x4, calibrationSettings_y2);
        u8g2.print(sz_temp[2]);
        u8g2.setCursor(calibrationSettings_x5, calibrationSettings_y2);
        u8g2.print(sz_temp[3]);

        u8g2.drawHLine(0, 22, 128);

        u8g2.setCursor(calibrationSettings_x1, calibrationSettings_y3);
        u8g2.print("Now ADC:");
        u8g2.setCursor(48, calibrationSettings_y3);
        u8g2.print(t12_ad, 0);

        u8g2.setCursor(calibrationSettings_x6, calibrationSettings_y3);
        u8g2.print("Save");
    }
    //画选框
    switch (calibrationSettings_kxxk_count) //空心选框的位置
    {
    case 1: //深圳头1段温度
        if (calibrationSettings_sxxk_count == calibrationSettings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(calibrationSettings_x2 - 2, calibrationSettings_y2 - 9, 21, 11); //实心方形
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(calibrationSettings_x2 - 2, calibrationSettings_y2 - 9, 21, 11); //空心方形
        break;
    case 2: //深圳头2段温度
        if (calibrationSettings_sxxk_count == calibrationSettings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(calibrationSettings_x3 - 2, calibrationSettings_y2 - 9, 21, 11); //实心方形
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(calibrationSettings_x3 - 2, calibrationSettings_y2 - 9, 21, 11); //空心方形
        break;
    case 3: //深圳头3段温度
        if (calibrationSettings_sxxk_count == calibrationSettings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(calibrationSettings_x4 - 2, calibrationSettings_y2 - 9, 21, 11); //实心方形
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(calibrationSettings_x4 - 2, calibrationSettings_y2 - 9, 21, 11); //空心方形
        break;
    case 4: //深圳头4段温度
        if (calibrationSettings_sxxk_count == calibrationSettings_kxxk_count)
        {
            u8g2.setDrawColor(2);
            u8g2.drawBox(calibrationSettings_x5 - 2, calibrationSettings_y2 - 9, 21, 11); //实心方形
            u8g2.setDrawColor(1);
        }
        else
            u8g2.drawFrame(calibrationSettings_x5 - 2, calibrationSettings_y2 - 9, 21, 11); //空心方形
        break;
    case 5: //保存
        u8g2.setDrawColor(2);
        u8g2.drawBox(calibrationSettings_x6 - 2, calibrationSettings_y3 - 8, 28, 9); //实心方形
        u8g2.setDrawColor(1);
        break;
    }
}

