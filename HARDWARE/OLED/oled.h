// oled.h
#ifndef __OLED_H
#define __OLED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "i2c.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"

#include "ds1302.h"
#include "sys_ctrl.h"
#include "sys_init.h"
// OLED定义
#define OLED_I2C_ADDR    0x78  // OLED I2C地址
#define OLED_CMD         0x00  // 写命令
#define OLED_DATA        0x40  // 写数据

// 屏幕参数
#define OLED_WIDTH       128
#define OLED_HEIGHT      64

// 常用符号定义
typedef enum {
    SYM_DEGREE = 0,     // 度符号 °
    SYM_PERCENT,        // 百分比 %
    SYM_CHECK,          // 对勾 ?
    SYM_CROSS,          // 叉号 ×
} Symbol_Type;

// 基本函数声明
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Refresh(void);
void OLED_SetPos(uint8_t x, uint8_t y);
void OLED_ShowChar(uint8_t x, uint8_t y, char chr);
void OLED_ShowString(uint8_t x, uint8_t y, const char *str);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);
void OLED_ShowSignedNum(uint8_t x, uint8_t y, int32_t num, uint8_t len);
void OLED_ShowFloat(uint8_t x, uint8_t y, float num, uint8_t int_len, uint8_t dec_len);
void OLED_ShowSymbol(uint8_t x, uint8_t y, Symbol_Type symbol);

// 绘图函数声明
void OLED_Draw_Pixel(uint8_t x, uint8_t y, uint8_t color);
void OLED_Draw_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void OLED_Draw_Rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void OLED_Draw_FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void OLED_Draw_ProgressBar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t percent);

// 功能函数
uint8_t Calculate_Air_Quality_Score(uint8_t temp, uint8_t humi, uint16_t co2, uint16_t pm25);
void OLED_Set_Brightness(uint8_t brightness);
void OLED_Set_Brightness_Percent(uint8_t percent);

// UI页面函数 - 修复：添加缺失的声明
void OLED_Display_TimePage(DS1302_Time_t *time, SysMode_t mode, uint32_t no_human_time);
void OLED_Display_SensorPage(uint8_t temp, uint8_t humi,uint16_t co2, uint16_t pm25,uint8_t score, SysMode_t mode);
void OLED_Display_ModePage(SysMode_t mode,
                           uint8_t fan_on,
                           uint8_t led_on,
                           uint8_t curtain_open,
                           uint8_t alarm_on,
                           uint8_t selected_line,
                           uint8_t sd_ok);  // 添加SD卡状态参数
// 参数修改页面显示函数
void OLED_Display_SettingPage(ComfortThreshold_t th, uint8_t selected_line, SysMode_t mode);
void OLED_Display_AlarmThresholdPage(AlarmThreshold_t th,
                                     uint8_t selected_line,
                                     SysMode_t mode);
#ifdef __cplusplus
}
#endif

#endif /* __OLED_H */
