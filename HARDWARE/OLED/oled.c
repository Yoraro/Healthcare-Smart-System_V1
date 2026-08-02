#include "oled.h"


// OLED显存
static uint8_t OLED_GRAM[128][8];

// 正确的6x8 ASCII字模
static const unsigned char ASCII_6x8[][6] = {
{0x00,0x00,0x00,0x00,0x00,0x00}, // 空格
{0x00,0x00,0x5F,0x00,0x00,0x00}, // !
{0x00,0x07,0x00,0x07,0x00,0x00}, // "
{0x14,0x7F,0x14,0x7F,0x14,0x00}, // #
{0x24,0x2A,0x7F,0x2A,0x12,0x00}, // $
{0x23,0x13,0x08,0x64,0x62,0x00}, // %
{0x36,0x49,0x55,0x22,0x50,0x00}, // &
{0x00,0x05,0x03,0x00,0x00,0x00}, // '
{0x00,0x1C,0x22,0x41,0x00,0x00}, // (
{0x00,0x41,0x22,0x1C,0x00,0x00}, // )
{0x14,0x08,0x3E,0x08,0x14,0x00}, // *
{0x08,0x08,0x3E,0x08,0x08,0x00}, // +
{0x00,0x50,0x30,0x00,0x00,0x00}, // ,
{0x08,0x08,0x08,0x08,0x08,0x00}, // -
{0x00,0x60,0x60,0x00,0x00,0x00}, // .
{0x20,0x10,0x08,0x04,0x02,0x00}, // /
{0x3E,0x51,0x49,0x45,0x3E,0x00}, // 0
{0x00,0x42,0x7F,0x40,0x00,0x00}, // 1
{0x42,0x61,0x51,0x49,0x46,0x00}, // 2
{0x21,0x41,0x45,0x4B,0x31,0x00}, // 3
{0x18,0x14,0x12,0x7F,0x10,0x00}, // 4
{0x27,0x45,0x45,0x45,0x39,0x00}, // 5
{0x3C,0x4A,0x49,0x49,0x30,0x00}, // 6
{0x01,0x71,0x09,0x05,0x03,0x00}, // 7
{0x36,0x49,0x49,0x49,0x36,0x00}, // 8
{0x06,0x49,0x49,0x29,0x1E,0x00}, // 9
{0x00,0x36,0x36,0x00,0x00,0x00}, // :
{0x00,0x56,0x36,0x00,0x00,0x00}, // ;
{0x08,0x14,0x22,0x41,0x00,0x00}, // <
{0x14,0x14,0x14,0x14,0x14,0x00}, // =
{0x00,0x41,0x22,0x14,0x08,0x00}, // >
{0x02,0x01,0x51,0x09,0x06,0x00}, // ?
{0x32,0x49,0x79,0x41,0x3E,0x00}, // @
{0x7E,0x11,0x11,0x11,0x7E,0x00}, // A
{0x7F,0x49,0x49,0x49,0x36,0x00}, // B
{0x3E,0x41,0x41,0x41,0x22,0x00}, // C
{0x7F,0x41,0x41,0x22,0x1C,0x00}, // D
{0x7F,0x49,0x49,0x49,0x41,0x00}, // E
{0x7F,0x09,0x09,0x09,0x01,0x00}, // F
{0x3E,0x41,0x49,0x49,0x7A,0x00}, // G
{0x7F,0x08,0x08,0x08,0x7F,0x00}, // H
{0x00,0x41,0x7F,0x41,0x00,0x00}, // I
{0x20,0x40,0x41,0x3F,0x01,0x00}, // J
{0x7F,0x08,0x14,0x22,0x41,0x00}, // K
{0x7F,0x40,0x40,0x40,0x40,0x00}, // L
{0x7F,0x02,0x0C,0x02,0x7F,0x00}, // M
{0x7F,0x04,0x08,0x10,0x7F,0x00}, // N
{0x3E,0x41,0x41,0x41,0x3E,0x00}, // O
{0x7F,0x09,0x09,0x09,0x06,0x00}, // P
{0x3E,0x41,0x51,0x21,0x5E,0x00}, // Q
{0x7F,0x09,0x19,0x29,0x46,0x00}, // R
{0x46,0x49,0x49,0x49,0x31,0x00}, // S
{0x01,0x01,0x7F,0x01,0x01,0x00}, // T
{0x3F,0x40,0x40,0x40,0x3F,0x00}, // U
{0x1F,0x20,0x40,0x20,0x1F,0x00}, // V
{0x3F,0x40,0x38,0x40,0x3F,0x00}, // W
{0x63,0x14,0x08,0x14,0x63,0x00}, // X
{0x07,0x08,0x70,0x08,0x07,0x00}, // Y
{0x61,0x51,0x49,0x45,0x43,0x00}, // Z
{0x00,0x7F,0x41,0x41,0x00,0x00}, // [
{0x02,0x04,0x08,0x10,0x20,0x00}, // "\"
{0x00,0x41,0x41,0x7F,0x00,0x00}, // ]
{0x04,0x02,0x01,0x02,0x04,0x00}, // ^
{0x40,0x40,0x40,0x40,0x40,0x00}, // _
{0x00,0x01,0x02,0x04,0x00,0x00}, // `
{0x20,0x54,0x54,0x54,0x78,0x00}, // a
{0x7F,0x48,0x44,0x44,0x38,0x00}, // b
{0x38,0x44,0x44,0x44,0x20,0x00}, // c
{0x38,0x44,0x44,0x48,0x7F,0x00}, // d
{0x38,0x54,0x54,0x54,0x18,0x00}, // e
{0x08,0x7E,0x09,0x01,0x02,0x00}, // f
{0x0C,0x52,0x52,0x52,0x3E,0x00}, // g
{0x7F,0x08,0x04,0x04,0x78,0x00}, // h
{0x00,0x44,0x7D,0x40,0x00,0x00}, // i
{0x20,0x40,0x44,0x3D,0x00,0x00}, // j
{0x7F,0x10,0x28,0x44,0x00,0x00}, // k
{0x00,0x41,0x7F,0x40,0x00,0x00}, // l
{0x7C,0x04,0x18,0x04,0x78,0x00}, // m
{0x7C,0x08,0x04,0x04,0x78,0x00}, // n
{0x38,0x44,0x44,0x44,0x38,0x00}, // o
{0x7C,0x14,0x14,0x14,0x08,0x00}, // p
{0x08,0x14,0x14,0x18,0x7C,0x00}, // q
{0x7C,0x08,0x04,0x04,0x08,0x00}, // r
{0x48,0x54,0x54,0x54,0x20,0x00}, // s
{0x04,0x3F,0x44,0x40,0x20,0x00}, // t
{0x3C,0x40,0x40,0x20,0x7C,0x00}, // u
{0x1C,0x20,0x40,0x20,0x1C,0x00}, // v
{0x3C,0x40,0x30,0x40,0x3C,0x00}, // w
{0x44,0x28,0x10,0x28,0x44,0x00}, // x
{0x0C,0x50,0x50,0x50,0x3C,0x00}, // y
{0x44,0x64,0x54,0x4C,0x44,0x00}, // z
{0x00,0x08,0x36,0x41,0x00,0x00}, // {
{0x00,0x00,0x7F,0x00,0x00,0x00}, // |
{0x00,0x41,0x36,0x08,0x00,0x00}, // }
{0x08,0x04,0x08,0x10,0x08,0x00}, // ~
};

// 常用符号字模
static const unsigned char SYMBOLS[][6] = {
{0x0E,0x11,0x11,0x0E,0x00,0x00}, // 度符号 °
{0x46,0x26,0x10,0x08,0x64,0x62}, // 百分比 %
{0x08,0x14,0x22,0x41,0x22,0x14}, // 对勾 ?
{0x41,0x22,0x14,0x08,0x14,0x22}, // 叉号 ×
};

// 写命令（保持不变）
void OLED_Write_Cmd(uint8_t cmd)
{
    uint8_t data[2] = {OLED_CMD, cmd};
    HAL_I2C_Master_Transmit(&hi2c1, OLED_I2C_ADDR, data, 2, 100);
}

// 写数据（保持不变）
void OLED_Write_Data(uint8_t data)
{
    uint8_t buf[2] = {OLED_DATA, data};
    HAL_I2C_Master_Transmit(&hi2c1, OLED_I2C_ADDR, buf, 2, 100);
}

// OLED初始化
void OLED_Init(void)
{
    HAL_Delay(100);
    
    OLED_Write_Cmd(0xAE); // 关闭显示
	OLED_Write_Cmd(0x21); // 设置列地址命令
    OLED_Write_Cmd(0x00); // 起始列地址 = 0
    OLED_Write_Cmd(0x7F); // 结束列地址 = 127 (0x7F)
    OLED_Write_Cmd(0x22); // 设置页地址命令  
    OLED_Write_Cmd(0x00); // 起始页地址 = 0
    OLED_Write_Cmd(0x07); // 结束页地址 = 7
    OLED_Write_Cmd(0x00); // 低列地址
    OLED_Write_Cmd(0x10); // 高列地址
    OLED_Write_Cmd(0x40); // 起始行
    OLED_Write_Cmd(0xB0); // 页地址
    OLED_Write_Cmd(0x81); // 对比度
    OLED_Write_Cmd(0xFF); 
    OLED_Write_Cmd(0xA1); // 段重映射
    OLED_Write_Cmd(0xC8); // 扫描方向
    OLED_Write_Cmd(0xA6); // 正常显示
    OLED_Write_Cmd(0xA8); // 多路复用
    OLED_Write_Cmd(0x3F); // 1/64
    OLED_Write_Cmd(0xA4); // 输出跟随RAM
    OLED_Write_Cmd(0xD3); // 显示偏移
    OLED_Write_Cmd(0x00); 
    OLED_Write_Cmd(0xD5); // 振荡分频
    OLED_Write_Cmd(0x80); 
    OLED_Write_Cmd(0xD9); // 预充电
    OLED_Write_Cmd(0xF1); 
    OLED_Write_Cmd(0xDA); // COM引脚
    OLED_Write_Cmd(0x12); 
    OLED_Write_Cmd(0xDB); // VCOMH
    OLED_Write_Cmd(0x40); 
    OLED_Write_Cmd(0x8D); // 电荷泵
    OLED_Write_Cmd(0x14); 
    OLED_Write_Cmd(0xAF); // 开启显示
    
    OLED_Clear();
    OLED_Refresh();
}

// 设置坐标
void OLED_SetPos(uint8_t x, uint8_t y)
{
    OLED_Write_Cmd(0xB0 + y);
    OLED_Write_Cmd(0x00 + (x & 0x0F));
    OLED_Write_Cmd(0x10 + ((x & 0xF0) >> 4));
}

// 清屏
void OLED_Clear(void)
{
    uint8_t i, j;
    for(j = 0; j < 8; j++)
    {
        for(i = 0; i < 128; i++)
        {
            OLED_GRAM[i][j] = 0x00;
        }
    }
}

// 刷新显示
void OLED_Refresh(void)
{
    uint8_t i, j;
    for(j = 0; j < 8; j++)
    {
        OLED_SetPos(0, j);
        for(i = 0; i < 128; i++)
        {
            OLED_Write_Data(OLED_GRAM[i][j]);
        }
    }
}

// 显示一个字符
void OLED_ShowChar(uint8_t x, uint8_t y, char chr)
{
    uint8_t temp, t, pos;
    uint8_t char_index;
    
    // 正常字符索引
    char_index = chr - 32;
    
    // 防止数组越界
    if(char_index >= 95) char_index = 0;
    
    for(pos = 0; pos < 6; pos++)
    {
        temp = ASCII_6x8[char_index][pos];
        for(t = 0; t < 8; t++)
        {
            if(temp & 0x01)
                OLED_GRAM[x][y] |= (1 << t);
            else
                OLED_GRAM[x][y] &= ~(1 << t);
            temp >>= 1;
        }
        x++;
    }
}

// 显示字符串
void OLED_ShowString(uint8_t x, uint8_t y, const char *str)
{
    while(*str != '\0')
    {
        OLED_ShowChar(x, y, *str);
        x += 6;
        str++;
        
        if(x > 122)
        {
            x = 0;
            y++;
            if(y >= 8) y = 7;
        }
    }
}

// 显示数字
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{
    char str[20];
    sprintf(str, "%lu", num);
    
    uint8_t str_len = strlen(str);
    if(len > str_len)
    {
        char temp[20];
        memset(temp, ' ', len - str_len);
        temp[len - str_len] = '\0';
        strcat(temp, str);
        OLED_ShowString(x, y, temp);
    }
    else
    {
        OLED_ShowString(x, y, str);
    }
}

// 显示有符号数字
void OLED_ShowSignedNum(uint8_t x, uint8_t y, int32_t num, uint8_t len)
{
    char str[20];
    sprintf(str, "%ld", num);
    OLED_ShowString(x, y, str);
}

// 显示浮点数
void OLED_ShowFloat(uint8_t x, uint8_t y, float num, uint8_t int_len, uint8_t dec_len)
{
    char format[20];
    char str[20];
    
    sprintf(format, "%%%d.%df", int_len + dec_len + 1, dec_len);
    sprintf(str, format, num);
    OLED_ShowString(x, y, str);
}

// 显示符号
void OLED_ShowSymbol(uint8_t x, uint8_t y, Symbol_Type symbol)
{
    if(symbol > SYM_CROSS) return;
    
    uint8_t temp, t, pos;
    for(pos = 0; pos < 6; pos++)
    {
        temp = SYMBOLS[symbol][pos];
        for(t = 0; t < 8; t++)
        {
            if(temp & 0x01)
                OLED_GRAM[x][y] |= (1 << t);
            else
                OLED_GRAM[x][y] &= ~(1 << t);
            temp >>= 1;
        }
        x++;
    }
}

// 绘制像素点
void OLED_Draw_Pixel(uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= 128 || y >= 64) return;
    
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    
    if (color) {
        OLED_GRAM[x][page] |= (1 << bit);
    } else {
        OLED_GRAM[x][page] &= ~(1 << bit);
    }
}

// 绘制直线
void OLED_Draw_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        OLED_Draw_Pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// 绘制矩形框
void OLED_Draw_Rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
{
    // 上下横线
    for (uint8_t i = 0; i < width; i++) {
        OLED_Draw_Pixel(x + i, y, color);
        OLED_Draw_Pixel(x + i, y + height - 1, color);
    }
    // 左右竖线
    for (uint8_t i = 0; i < height; i++) {
        OLED_Draw_Pixel(x, y + i, color);
        OLED_Draw_Pixel(x + width - 1, y + i, color);
    }
}

// 绘制实心矩形
void OLED_Draw_FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
{
    for (uint8_t i = 0; i < height; i++) {
        for (uint8_t j = 0; j < width; j++) {
            OLED_Draw_Pixel(x + j, y + i, color);
        }
    }
}

// 绘制进度条
void OLED_Draw_ProgressBar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t percent)
{
    if (percent > 100) percent = 100;
    
    // 绘制外框
    OLED_Draw_Rect(x, y, width, height, 1);
    
    // 计算填充宽度
    uint8_t fill_width = (width - 2) * percent / 100;
    
    // 绘制填充
    if (fill_width > 0) {
        OLED_Draw_FillRect(x + 1, y + 1, fill_width, height - 2, 1);
    }
}

/**
 * @brief 计算室内空气质量综合评分
 * @param temp 温度值(0~50℃)
 * @param humi 湿度值(20~90%)
 * @param co2 CO2浓度(ppm)
 * @param pm25 PM2.5浓度(ug/m3)
 * @return 综合质量评分(0~100)
 */
uint8_t Calculate_Air_Quality_Score(uint8_t temp, uint8_t humi, uint16_t co2, uint16_t pm25)
{
    float temp_score = 0, humi_score = 0, co2_score = 0, pm25_score = 0;
    float total_score = 0;
    
    // 1. 温度评分 (0~50℃, 理想范围20~25℃)
    if(temp >= 20 && temp <= 25) {
        temp_score = 100;  // 最佳范围
    } else if(temp < 10 || temp > 40) {
        temp_score = 10;   // 极端温度，低分
    } else if(temp < 20) {
        temp_score = 20 + (temp - 10) * 8;  // 10-20℃线性评分
    } else {
        temp_score = 100 - (temp - 25) * 6; // 25-40℃线性评分
    }
    
    // 2. 湿度评分 (20~90%, 理想范围40~60%)
    if(humi >= 40 && humi <= 60) {
        humi_score = 100;  // 最佳范围
    } else if(humi < 20 || humi > 80) {
        humi_score = 10;   // 极端湿度，低分
    } else if(humi < 40) {
        humi_score = 20 + (humi - 20) * 4;  // 20-40%线性评分
    } else {
        humi_score = 100 - (humi - 60) * 4.5; // 60-80%线性评分
    }
    
    // 3. CO2浓度评分 (理想值<1000ppm)
    if(co2 < 600) {
        co2_score = 100;  // 优秀
    } else if(co2 < 1000) {
        co2_score = 80;   // 良好
    } else if(co2 < 1500) {
        co2_score = 60;   // 一般
    } else if(co2 < 2000) {
        co2_score = 40;   // 较差
    } else {
        co2_score = 20;   // 很差
    }
    
    // 4. PM2.5浓度评分 (理想值<35ug/m3)
    if(pm25 < 35) {
        pm25_score = 100;  // 优秀
    } else if(pm25 < 75) {
        pm25_score = 75;   // 良好
    } else if(pm25 < 115) {
        pm25_score = 50;   // 一般
    } else if(pm25 < 150) {
        pm25_score = 25;   // 较差
    } else {
        pm25_score = 10;   // 很差
    }
    
    // 四个部分权重相等，各占25%
    total_score = temp_score * 0.25 + humi_score * 0.25 + 
                  co2_score * 0.25 + pm25_score * 0.25;
    
    // 限制在0-100范围内
    if(total_score > 100) total_score = 100;
    if(total_score < 0) total_score = 0;
    
    return (uint8_t)total_score;
}


//———————————控制模块控制显示—————————————//
// OLED亮度控制函数
void OLED_Set_Brightness(uint8_t brightness)
{
    // SM0802003V01使用SSD1306驱动芯片
    // 通过设置预充电周期和VCOMH来调整亮度，而不仅仅是对比度
    
    // 设置对比度（主要亮度控制）
    OLED_Write_Cmd(0x81); // 设置对比度命令
    OLED_Write_Cmd(brightness); // 对比度值，0-255
    
    // 调整预充电周期以获得更好的亮度控制
    OLED_Write_Cmd(0xD9); // 设置预充电周期
    if (brightness < 64) {
        OLED_Write_Cmd(0xF1); // 低亮度时的预充电
    } else if (brightness < 128) {
        OLED_Write_Cmd(0x82); // 中等亮度
    } else {
        OLED_Write_Cmd(0xF1); // 高亮度
    }
    
    // 调整VCOMH电压
    OLED_Write_Cmd(0xDB); // 设置VCOMH
    if (brightness < 85) {
        OLED_Write_Cmd(0x20); // 0.77*VCC（低亮度）
    } else if (brightness < 170) {
        OLED_Write_Cmd(0x30); // 0.83*VCC（中亮度）
    } else {
        OLED_Write_Cmd(0x40); // 0.92*VCC（高亮度）
    }
}

// 添加新的亮度调节函数，支持0-100%范围
void OLED_Set_Brightness_Percent(uint8_t percent)
{
    if (percent > 100) percent = 100;
    
    // 将百分比转换为0-255范围，使用指数曲线更符合人眼感知
    uint8_t brightness;
    if (percent == 0) {
        brightness = 0;
    } else if (percent <= 20) {
        brightness = percent * 2;  // 0-20%: 线性增加
    } else if (percent <= 50) {
        brightness = 40 + (percent - 20) * 3;  // 20-50%: 中等斜率
    } else {
        brightness = 130 + (percent - 50) * 2.5;  // 50-100%: 较平缓
    }
    
    OLED_Set_Brightness(brightness);
}

//界面显示：
void OLED_Display_TimePage(DS1302_Time_t *time, SysMode_t mode, uint32_t no_human_time)
{
    OLED_Clear();

    /* ===== Page 0：TIME | MODE ===== */
    OLED_ShowString(0, 0, "TIME");
    const char *mode_str[] = {"MANUAL", "AUTO", "ECO"};
    uint8_t mode_x = 128 - strlen(mode_str[mode]) * 6;
    OLED_ShowString(mode_x, 0, mode_str[mode]);

    /* ===== Page 1：分隔线 ===== */
    OLED_ShowString(0, 1, "---------------------");

    /* ===== Page 2：日期（居中） ===== */
    char date_str[16];
    sprintf(date_str, "20%02d-%02d-%02d",
            time->year, time->month, time->date);
    OLED_ShowString((128 - strlen(date_str) * 6) / 2, 2, date_str);

    /* ===== Page 4：时间（重点，留出上下间距） ===== */
    char time_str[16];
    sprintf(time_str, "%02d:%02d:%02d",
            time->hour, time->minute, time->second);
    OLED_ShowString((128 - strlen(time_str) * 6) / 2, 4, time_str);

    /* ===== Page 6：星期 ===== */
    const char *week_str[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    char week_buf[16];
    sprintf(week_buf, "Week: %s", week_str[time->week % 7]);
    OLED_ShowString((128 - strlen(week_buf) * 6) / 2, 6, week_buf);

    /* ===== Page 7：人员状态 ===== */
    char human_buf[24];
    if (no_human_time > 0)
    {
        uint16_t h = no_human_time / 3600;
        uint16_t m = (no_human_time % 3600) / 60;
        sprintf(human_buf, "NO HUMAN %02d:%02d", h, m);
    }
    else
    {
        strcpy(human_buf, "HUMAN DETECTED");
    }
    OLED_ShowString((128 - strlen(human_buf) * 6) / 2, 7, human_buf);

    OLED_Refresh();
}


// 环境状态页面显示函数（无边框版本）
void OLED_Display_SensorPage(uint8_t temp, uint8_t humi,
                             uint16_t co2, uint16_t pm25,
                             uint8_t score, SysMode_t mode)
{
    OLED_Clear();

    /* ===== Page 0：ENVIRONMENT | MODE ===== */
    OLED_ShowString(0, 0, "ENVIRONMENT");

    const char *mode_str[] = {"MANUAL", "AUTO", "ECO"};
    uint8_t mode_x = 128 - strlen(mode_str[mode]) * 6;
    OLED_ShowString(mode_x, 0, mode_str[mode]);

    /* ===== Page 1：分隔线 ===== */
    OLED_ShowString(0, 1, "---------------------");

    char buf[32];

    /* ===== Page 2：温度 ===== */
    sprintf(buf, "Temp: %2d ", temp);
	// 计算居中起始 X
	uint8_t start_x = (128 - (strlen(buf) + 2) * 6) / 2;

	// 显示 "Temp: xx"
	OLED_ShowString(start_x, 2, buf);

	// 显示 °
	OLED_ShowSymbol(start_x + strlen(buf) * 6, 2, SYM_DEGREE);

	// 显示 C
	OLED_ShowString(start_x + strlen(buf) * 6 + 6, 2, "C");

    /* ===== Page 3：湿度 ===== */
    sprintf(buf, "Humi: %2d %%", humi);
    OLED_ShowString((128 - strlen(buf) * 6) / 2, 3, buf);

    /* ===== Page 4：CO2 ===== */
    sprintf(buf, "CO2: %4d ppm", co2);
    OLED_ShowString((128 - strlen(buf) * 6) / 2, 4, buf);

    /* ===== Page 5：PM2.5 ===== */
    sprintf(buf, "PM2.5: %3d ug/m3", pm25);
    OLED_ShowString((128 - strlen(buf) * 6) / 2, 5, buf);

    /* ===== Page 6：质量评分 ===== */
    sprintf(buf, "Quality: %3d / 100", score);
    OLED_ShowString((128 - strlen(buf) * 6) / 2, 6, buf);

    /* ===== Page 7：进度条 ===== */
    OLED_Draw_Rect(0, 56, 128, 8, 1);     // 外框
    uint8_t fill_width = (126 * score) / 100;
    if (fill_width > 0)
    {
        OLED_Draw_FillRect(1, 57, fill_width, 6, 1);
    }

    OLED_Refresh();
}

// 设备状态页面（MODE ）
void OLED_Display_ModePage(SysMode_t mode,
                           uint8_t fan_on,
                           uint8_t led_on,
                           uint8_t curtain_open,
                           uint8_t alarm_on,
                           uint8_t selected_line,
                           uint8_t sd_ok)  // 添加SD卡状态参数
{
    OLED_Clear();

    // Page 0：标题 + 当前模式
    OLED_ShowString(0, 0, "MODE");
    const char *mode_str[] = {"MANUAL", "AUTO", "ECO"};
    uint8_t mode_x = 128 - strlen(mode_str[mode]) * 6;
    OLED_ShowString(mode_x, 0, mode_str[mode]);

    // Page 1：分隔线
    OLED_ShowString(0, 1, "---------------------");

    // Page 2：风扇状态
    OLED_ShowString(0, 2, "Fan     ");
    if (fan_on) {
        OLED_ShowString(48, 2, "[ON ]");
    } else {
        OLED_ShowString(48, 2, "[OFF]");
    }

    // Page 3：LED状态
    OLED_ShowString(0, 3, "LED     ");
    if (led_on) {
        OLED_ShowString(48, 3, "[ON ]");
    } else {
        OLED_ShowString(48, 3, "[OFF]");
    }

    // Page 4：窗帘状态
    OLED_ShowString(0, 4, "Curtain ");
    if (curtain_open) {
        OLED_ShowString(48, 4, "[OPEN]");
    } else {
        OLED_ShowString(48, 4, "[CLOSE]");
    }

    // Page 5：分隔线
    OLED_ShowString(0, 5, "---------------------");

    // Page 6：报警状态（不可修改，只显示）
    OLED_ShowString(0, 6, "Alarm   ");
    if (alarm_on) {
        OLED_ShowString(48, 6, "[ON ]");
    } else {
        OLED_ShowString(48, 6, "[OFF]");
    }

    // Page 7：SD卡状态
    OLED_ShowString(0, 7, "SD Card ");
    if (sd_ok) {
        OLED_ShowString(48, 7, "[OK ]");
    } else {
        OLED_ShowString(48, 7, "[FAIL]");
    }

    // 选中行提示 << （仅手动模式有效，且只对 Fan/LED/Curtain，报警不可选）
    if (mode == SYS_MODE_MANUAL && selected_line <= 2) {
        OLED_ShowString(110, 2 + selected_line, "<<");  // 加 2 对齐 OLED 显示行
    }

    OLED_Refresh();
}

// 设置页面（SETTING 页面）
void OLED_Display_SettingPage(ComfortThreshold_t th, uint8_t selected_line, SysMode_t mode)
{
    OLED_Clear();

    OLED_ShowString(0, 0, "SETTING");

    const char *mode_str[] = {"MANUAL", "AUTO", "ECO"};
    uint8_t mode_x = 128 - strlen(mode_str[mode]) * 6;
    OLED_ShowString(mode_x, 0, mode_str[mode]);

    OLED_ShowString(0, 1, "--------------------");

    char buf[32];
    uint8_t x_unit;

    // Temp Max
    sprintf(buf, "Temp Max : %3d", th.temp_max);
    OLED_ShowString(0, 2, buf);
    x_unit = strlen(buf) * 6;
    OLED_ShowSymbol(x_unit, 2, SYM_DEGREE);
    OLED_ShowString(x_unit + 6, 2, "C");

    // Temp Min
    sprintf(buf, "Temp Min : %3d", th.temp_min);
    OLED_ShowString(0, 3, buf);
    x_unit = strlen(buf) * 6;
    OLED_ShowSymbol(x_unit, 3, SYM_DEGREE);
    OLED_ShowString(x_unit + 6, 3, "C");

    // Humi Max
    sprintf(buf, "Humi Max : %3d", th.humi_max);
    OLED_ShowString(0, 4, buf);
    OLED_ShowString(strlen(buf) * 6, 4, "%");

    // Humi Min
    sprintf(buf, "Humi Min : %3d", th.humi_min);
    OLED_ShowString(0, 5, buf);
    OLED_ShowString(strlen(buf) * 6, 5, "%");

    // PM2.5 Max
    sprintf(buf, "PM2.5 Max: %3d", th.pm25_max);
    OLED_ShowString(0, 6, buf);
    OLED_ShowString(strlen(buf) * 6 + 6, 6, "ug/m3");

    // CO2 Max
    sprintf(buf, "CO2 Max  : %4d", th.co2_max);
    OLED_ShowString(0, 7, buf);
    OLED_ShowString(strlen(buf) * 6 + 6, 7, "ppm");

    if (selected_line <= 5)
    {
        OLED_ShowString(110, 2 + selected_line, "<<");
    }

    OLED_Refresh();
}

// 报警阈值设置页面（ALARM SET 页面）
void OLED_Display_AlarmThresholdPage(AlarmThreshold_t th,
                                     uint8_t selected_line,
                                     SysMode_t mode)
{
    OLED_Clear();

    // Page 0：标题
    OLED_ShowString(0, 0, "ALARM SET");
               
    // 右上角：当前模式（与 SETTING 页面完全一致）
    const char *mode_str[] = {"MANUAL", "AUTO", "ECO"};
    uint8_t mode_x = 128 - strlen(mode_str[mode]) * 6;
    OLED_ShowString(mode_x, 0, mode_str[mode]);

    // Page 1：分隔线
    OLED_ShowString(0, 1, "--------------------");

    char buf[32];
    uint8_t x_unit;

    // Temp Max
    sprintf(buf, "Temp Max : %3d", th.temp_max);
    OLED_ShowString(0, 2, buf);
    x_unit = strlen(buf) * 6;
    OLED_ShowSymbol(x_unit, 2, SYM_DEGREE);
    OLED_ShowString(x_unit + 6, 2, "C");

    // Temp Min
    sprintf(buf, "Temp Min : %3d", th.temp_min);
    OLED_ShowString(0, 3, buf);
    x_unit = strlen(buf) * 6;
    OLED_ShowSymbol(x_unit, 3, SYM_DEGREE);
    OLED_ShowString(x_unit + 6, 3, "C");

    // PM2.5 Max
    sprintf(buf, "PM2.5 Max: %3d", th.pm25_max);
    OLED_ShowString(0, 4, buf);
    OLED_ShowString(strlen(buf) * 6 + 6, 4, "ug/m3");

    // CO2 Max
    sprintf(buf, "CO2 Max  : %4d", th.co2_max);
    OLED_ShowString(0, 5, buf);
    OLED_ShowString(strlen(buf) * 6 + 6, 5, "ppm");

    // 选中行指示
    if (selected_line <= 3)
    {
        OLED_ShowString(110, 2 + selected_line, "<<");
    }

    OLED_Refresh();
}

