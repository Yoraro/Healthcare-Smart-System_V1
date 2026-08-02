/**
 * @file    app_sdcard.h
 * @brief   SD卡应用层接口
 */

#ifndef __APP_SDCARD_H
#define __APP_SDCARD_H

#include "stm32f1xx_hal.h"
#include "ff.h"
#include "sdcard.h"
#include <string.h>
#include <stdio.h>

/* SD卡状态 */
typedef enum {
    SD_APP_OK = 0,
    SD_APP_MOUNT_ERR,
    SD_APP_OPEN_ERR,
    SD_APP_WRITE_ERR,
    SD_APP_READ_ERR,
    SD_APP_CLOSE_ERR
} SD_App_Status_t;

/* 函数声明 */
SD_App_Status_t SD_App_Init(void);
void SD_App_DeInit(void);
SD_App_Status_t SD_App_WriteFile(const char *filename, const char *data);
SD_App_Status_t SD_App_ReadFile(const char *filename, char *buf, uint32_t bufsize);
SD_App_Status_t SD_App_AppendFile(const char *filename, const char *data);
void SD_App_Test(void);

#endif /* __APP_SDCARD_H */
