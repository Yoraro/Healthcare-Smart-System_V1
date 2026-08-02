#ifndef LOGGER_H
#define LOGGER_H

#include "app_sdcard.h"
#include <stdint.h>
#include <stdio.h>

typedef struct {
    float temperature;
    float humidity;
    uint8_t device_status;  // 各设备状态，可以用位表示
    uint8_t system_mode;
} LogData_t;

/**
 * @brief 初始化日志模块（会初始化SD卡）
 */
void Logger_Init(void);

/**
 * @brief 定时写入日志
 * @param data 采集到的环境和设备数据
 */
void Logger_Write(const LogData_t *data);

/**
 * @brief 获取当天日志文件名
 */
const char* Logger_GetFileName(void);

#endif
