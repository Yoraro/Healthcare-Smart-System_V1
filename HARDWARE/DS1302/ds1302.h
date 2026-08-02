#ifndef _DS1302_H
#define _DS1302_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

/* ================= GPIO 定义（保持你原来的） ================= */
#define DS1302_GPIO        GPIOA
#define DS1302_SCLK        GPIO_PIN_15
#define DS1302_SDA         GPIO_PIN_12
#define DS1302_RST         GPIO_PIN_11

/* ================= 时间结构体 ================= */
typedef struct
{
    uint8_t second;   // 0–59
    uint8_t minute;   // 0–59
    uint8_t hour;     // 0–23
    uint8_t date;     // 1–31
    uint8_t month;    // 1–12
    uint8_t week;     // 1–7 (Mon–Sun)
    uint8_t year;     // 0–99 -> 2000–2099
} DS1302_Time_t;

/* ================= API ================= */
void DS1302_Init(void);
void DS1302_ReadTime(DS1302_Time_t *time);
void DS1302_WriteTime(const DS1302_Time_t *time);

void DS1302_ClockStart(void);
void DS1302_ClockStop(void);

#endif
