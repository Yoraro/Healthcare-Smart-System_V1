#include "ds1302.h"
#include "delay.h"

/* ================= 寄存器定义 ================= */
#define DS1302_SEC     0x80
#define DS1302_MIN     0x82
#define DS1302_HOUR    0x84
#define DS1302_DATE    0x86
#define DS1302_MONTH   0x88
#define DS1302_DAY     0x8A
#define DS1302_YEAR    0x8C
#define DS1302_CTRL    0x8E
#define DS1302_BURST   0xBE

/* ================= BCD 转换 ================= */
#define HEX2BCD(x)   (uint8_t)(((x) / 10 << 4) | ((x) % 10))
#define BCD2HEX(x)   (uint8_t)(((x) >> 4) * 10 + ((x) & 0x0F))

/* ================= SDA 方向控制 ================= */
static void SDA_OUT(void)
{
    GPIO_InitTypeDef g = {0};
    g.Pin = DS1302_SDA;
    g.Mode = GPIO_MODE_OUTPUT_PP;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS1302_GPIO, &g);
}

static void SDA_IN(void)
{
    GPIO_InitTypeDef g = {0};
    g.Pin = DS1302_SDA;
    g.Mode = GPIO_MODE_INPUT;
    g.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DS1302_GPIO, &g);
}

/* ================= 低层 IO ================= */
static void DS1302_WriteByteRaw(uint8_t val)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(DS1302_GPIO, DS1302_SDA,
            (val & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);

        HAL_GPIO_WritePin(DS1302_GPIO, DS1302_SCLK, GPIO_PIN_SET);
        delay_us(1);
        HAL_GPIO_WritePin(DS1302_GPIO, DS1302_SCLK, GPIO_PIN_RESET);
        delay_us(1);

        val >>= 1;
    }
}

static uint8_t DS1302_ReadByteRaw(void)
{
    uint8_t val = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        val >>= 1;
        if (HAL_GPIO_ReadPin(DS1302_GPIO, DS1302_SDA))
            val |= 0x80;

        HAL_GPIO_WritePin(DS1302_GPIO, DS1302_SCLK, GPIO_PIN_SET);
        delay_us(1);
        HAL_GPIO_WritePin(DS1302_GPIO, DS1302_SCLK, GPIO_PIN_RESET);
        delay_us(1);
    }
    return val;
}

static void DS1302_WriteReg(uint8_t reg, uint8_t val)
{
    HAL_GPIO_WritePin(DS1302_GPIO, DS1302_RST, GPIO_PIN_SET);
    DS1302_WriteByteRaw(reg);
    DS1302_WriteByteRaw(val);
    HAL_GPIO_WritePin(DS1302_GPIO, DS1302_RST, GPIO_PIN_RESET);
}

static uint8_t DS1302_ReadReg(uint8_t reg)
{
    uint8_t val;

    HAL_GPIO_WritePin(DS1302_GPIO, DS1302_RST, GPIO_PIN_SET);
    DS1302_WriteByteRaw(reg | 0x01);
    SDA_IN();
    val = DS1302_ReadByteRaw();
    SDA_OUT();
    HAL_GPIO_WritePin(DS1302_GPIO, DS1302_RST, GPIO_PIN_RESET);

    return val;
}

/* ================= 初始化 ================= */
void DS1302_Init(void)
{
    GPIO_InitTypeDef g = {0};
    g.Pin = DS1302_SCLK | DS1302_SDA | DS1302_RST;
    g.Mode = GPIO_MODE_OUTPUT_PP;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS1302_GPIO, &g);

    HAL_GPIO_WritePin(DS1302_GPIO, DS1302_RST, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DS1302_GPIO, DS1302_SCLK, GPIO_PIN_RESET);

    DS1302_ClockStart();
}

/* ================= 写时间（低频使用） ================= */
void DS1302_WriteTime(const DS1302_Time_t *t)
{
    __disable_irq();

    DS1302_WriteReg(DS1302_CTRL, 0x00);

    DS1302_WriteReg(DS1302_SEC,   HEX2BCD(t->second) & 0x7F);
    DS1302_WriteReg(DS1302_MIN,   HEX2BCD(t->minute));
    DS1302_WriteReg(DS1302_HOUR,  HEX2BCD(t->hour));
    DS1302_WriteReg(DS1302_DATE,  HEX2BCD(t->date));
    DS1302_WriteReg(DS1302_MONTH, HEX2BCD(t->month));
    DS1302_WriteReg(DS1302_DAY,   HEX2BCD(t->week));
    DS1302_WriteReg(DS1302_YEAR,  HEX2BCD(t->year));

    DS1302_WriteReg(DS1302_CTRL, 0x80);

    __enable_irq();
}

/* ================= 读时间（稳定核心） ================= */
void DS1302_ReadTime(DS1302_Time_t *t)
{
    uint8_t buf[8];

    __disable_irq();   // ★ 核心：隔离所有中断

    HAL_GPIO_WritePin(DS1302_GPIO, DS1302_RST, GPIO_PIN_SET);
    DS1302_WriteByteRaw(DS1302_BURST | 0x01);
    SDA_IN();

    for (uint8_t i = 0; i < 8; i++)
        buf[i] = DS1302_ReadByteRaw();

    SDA_OUT();
    HAL_GPIO_WritePin(DS1302_GPIO, DS1302_RST, GPIO_PIN_RESET);

    __enable_irq();

    /* -------- 合法性校验，防止垃圾数据 -------- */
    uint8_t sec   = BCD2HEX(buf[0] & 0x7F);
    uint8_t min   = BCD2HEX(buf[1]);
    uint8_t hour  = BCD2HEX(buf[2]);
    uint8_t date  = BCD2HEX(buf[3]);
    uint8_t month = BCD2HEX(buf[4]);
    uint8_t week  = BCD2HEX(buf[5]);
    uint8_t year  = BCD2HEX(buf[6]);

    if (sec > 59 || min > 59 || hour > 23 ||
        date == 0 || date > 31 ||
        month == 0 || month > 12 ||
        week < 1 || week > 7)
    {
        return;   // 丢弃本次读数
    }

    t->second = sec;
    t->minute = min;
    t->hour   = hour;
    t->date   = date;
    t->month  = month;
    t->week   = week;
    t->year   = year;
}

/* ================= 时钟控制 ================= */
void DS1302_ClockStart(void)
{
    uint8_t sec = DS1302_ReadReg(DS1302_SEC);
    sec &= ~0x80;
    DS1302_WriteReg(DS1302_SEC, sec);
}

void DS1302_ClockStop(void)
{
    uint8_t sec = DS1302_ReadReg(DS1302_SEC);
    sec |= 0x80;
    DS1302_WriteReg(DS1302_SEC, sec);
}
