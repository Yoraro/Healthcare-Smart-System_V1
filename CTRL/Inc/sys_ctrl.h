// sys_ctrl.h
#ifndef __SYS_CTRL_H
#define __SYS_CTRL_H

#include "main.h"
#include <stdint.h>
#include "sys_init.h"

/* ================= 传感器数据结构体 ================= */
typedef struct
{
    /* 传感器数据 */
    uint8_t  temp;
    uint8_t  humi;
    uint16_t co2;
    uint16_t pm25;
    uint8_t  air_score;

    /* 人体状态 */
    uint8_t  human_detected;
    uint32_t activity_count;     // PIR由无信号变为有信号的累计次数
    uint32_t last_human_tick;

    /* 设备状态 */
    uint8_t fan_on;
    uint8_t led_on;          // LED状态
    uint8_t curtain_open;    // 窗帘状态
    uint8_t curtain_current;  // 当前位置（0=关闭，1=打开）
    uint8_t curtain_moving;   // 是否正在运动（0=静止，1=运动中）	
		
    uint8_t alarm_active;    
	
    // 节能模式相关
    uint8_t eco_manual_set;    // 手动设置的节能模式
    uint32_t eco_enter_time;     // 进入节能模式前的连续无人秒数
    
    // 刷新标志
    uint8_t mode_changed;
	
} SensorData_t;

/* ================= UI 页面枚举 ================= */
typedef enum
{
    UI_TIME_PAGE = 0,      // 时间首页
    UI_SENSOR_PAGE,        // 传感器页面
    UI_MODE_PAGE,          // 模式显示页面
    UI_SETTING_PAGE,       // 设置页面
	UI_THRESHOLD_PAGE      // 阈值修改页面
} UIPage_t;

/* ================= 舒适度阈值编辑枚举 ================= */
typedef enum
{
    EDIT_NONE = 0,
    EDIT_TEMP_MIN,
    EDIT_TEMP_MAX,
    EDIT_HUMI_MIN,
    EDIT_HUMI_MAX,
    EDIT_CO2_MAX,
    EDIT_PM25_MAX
} ThresholdEdit_t;

/* ================= 报警阈值编辑枚举 ================= */
typedef enum {
    SET_TEMP_MAX = 0,
    SET_TEMP_MIN,
    SET_PM25_MAX,
    SET_CO2_MAX,
} SettingItem_t;

/* ================= 全局变量声明 ================= */
// 这些变量在sys_ctrl.c中定义
extern SensorData_t g_sensor_data;     // 传感器数据
extern UIPage_t g_ui_page;             // 当前UI页面
extern ThresholdEdit_t g_edit_item;    // 当前编辑项
extern SysMode_t g_last_mode_before_eco; // 进入节能模式前的模式

// 这些变量在sys_init.c中定义
extern SysMode_t g_sys_mode;
extern SysStateEnum_t g_sys_state;
extern ComfortThreshold_t g_comfort_th;
extern AlarmThreshold_t g_alarm_th;

/* ================= 接口函数 ================= */
void SYS_CTRL_Init(void);              // 控制初始化
void SYS_CTRL_Task(void);              // 主控制任务
void SYS_Check_Human_Mode(void);       // 检查人体模式切换
void SYS_Auto_Control(void);           // 自动控制函数
void SYS_Eco_Control(void);            // 节能控制函数
void SYS_Eco_Enter(void);              // 进入节能模式
void SYS_Eco_Exit(void);               // 退出节能模式

#endif
