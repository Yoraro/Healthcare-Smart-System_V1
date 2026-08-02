// sys_init.h
#ifndef __SYS_INIT_H
#define __SYS_INIT_H

#include "main.h"
#include <stdint.h>

/* ================= 系统模式定义（三模式并列） ================= */
typedef enum
{
    SYS_MODE_MANUAL = 0,    // 手动模式
    SYS_MODE_AUTO,          // 自动模式
    SYS_MODE_ECO            // 节能模式
} SysMode_t;

/* ================= 系统状态枚举 ================= */
typedef enum
{
    SYS_STATE_NORMAL = 0,   // 正常状态
    SYS_STATE_NO_HUMAN,     // 无人状态
    SYS_STATE_ALARM         // 报警状态
} SysStateEnum_t;

/* ================= 舒适阈值结构体 ================= */
typedef struct
{
    // 正常模式阈值（仅用于自动模式）
    uint8_t  temp_min;
    uint8_t  temp_max;
    uint8_t  humi_min;
    uint8_t  humi_max;
    uint16_t co2_max;
    uint16_t pm25_max;
} ComfortThreshold_t;

/* ================= 报警阈值结构体 ================= */
typedef struct
{
    uint8_t  temp_min;
    uint8_t  temp_max;
    uint16_t pm25_max;
    uint16_t co2_max;
} AlarmThreshold_t;

/* ================= 系统全局参数 ================= */
extern SysMode_t g_sys_mode;           // 当前系统模式
extern SysStateEnum_t g_sys_state;     // 当前系统状态
extern ComfortThreshold_t g_comfort_th; // 阈值参数（仅用于自动模式）
extern AlarmThreshold_t g_alarm_th;     //报警阈值

/* ================= 接口函数 ================= */
void SYS_Init_All(void);               // 系统初始化

#endif
