#include "logger.h"
#include "ds1302.h" 
#include <string.h>
#include <stdio.h>

static char log_filename[32];  // 当前日志文件名
static uint8_t logger_initialized = 0;

/**
 * @brief 生成日志文件名，例如 "2026-01-12.csv"
 */
static void Logger_UpdateFileName(void)
{
    DS1302_TimeTypeDef time;
    DS1302_DateTypeDef date;

    DS1302_GetTime(&time);
    DS1302_GetDate(&date);

    snprintf(log_filename, sizeof(log_filename),
             "0:/%04d-%02d-%02d.csv",
             2000 + date.Year, date.Month, date.Day);
}

const char* Logger_GetFileName(void)
{
    return log_filename;
}

/**
 * @brief 初始化日志模块
 */
void Logger_Init(void)
{
    if(logger_initialized) return;

    if(SD_App_Init() != SD_APP_OK) {
        printf("[Logger] SD Init Failed!\r\n");
        return;
    }

    Logger_UpdateFileName();

    // 如果当天文件不存在，则写入CSV表头
    char buf[128];
    char readbuf[2];
    if(SD_App_ReadFile(log_filename, readbuf, sizeof(readbuf)) != SD_APP_OK) {
        snprintf(buf, sizeof(buf), "Time,Temperature,Humidity,DeviceStatus,SystemMode\r\n");
        SD_App_WriteFile(log_filename, buf);
    }

    logger_initialized = 1;
    printf("[Logger] Initialized, log file: %s\r\n", log_filename);
}

/**
 * @brief 写入一条日志（追加）
 */
void Logger_Write(const LogData_t *data)
{
    if(!logger_initialized) return;

    // 更新当天文件名，如果跨天
    char prev_filename[32];
    strcpy(prev_filename, log_filename);
    Logger_UpdateFileName();

    if(strcmp(prev_filename, log_filename) != 0) {
        // 跨天，写表头
        char buf[128];
        snprintf(buf, sizeof(buf), "Time,Temperature,Humidity,DeviceStatus,SystemMode\r\n");
        SD_App_WriteFile(log_filename, buf);
    }

    char buf[128];
    DS1302_TimeTypeDef time;
    DS1302_GetTime(&time);

    snprintf(buf, sizeof(buf), "%02d:%02d:%02d,%.2f,%.2f,0x%02X,%d\r\n",
             time.Hours, time.Minutes, time.Seconds,
             data->temperature,
             data->humidity,
             data->device_status,
             data->system_mode);

    SD_App_AppendFile(log_filename, buf);
}
