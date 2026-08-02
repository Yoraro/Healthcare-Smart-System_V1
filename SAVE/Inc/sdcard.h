/**
 * @file    sdcard.h
 * @brief   SD卡驱动头文件 - 适用于STM32F103 (SPI模式)
 */

#ifndef __SDCARD_H
#define __SDCARD_H

#include "stm32f1xx_hal.h"
#include "bsp_spi.h"

/* SD卡类型定义 */
#define SD_TYPE_ERR     0x00    /* 错误/未知类型 */
#define SD_TYPE_MMC     0x01    /* MMC卡 */
#define SD_TYPE_V1      0x02    /* SD卡 V1.x */
#define SD_TYPE_V2      0x04    /* SD卡 V2.0 (标准容量) */
#define SD_TYPE_V2HC    0x06    /* SD卡 V2.0 (高容量SDHC) */

/* SD卡命令定义 */
#define CMD0    0       /* GO_IDLE_STATE - 复位SD卡 */
#define CMD1    1       /* SEND_OP_COND - 发送操作条件(MMC) */
#define CMD8    8       /* SEND_IF_COND - 发送接口条件 */
#define CMD9    9       /* SEND_CSD - 读取CSD寄存器 */
#define CMD10   10      /* SEND_CID - 读取CID寄存器 */
#define CMD12   12      /* STOP_TRANSMISSION - 停止传输 */
#define CMD16   16      /* SET_BLOCKLEN - 设置块长度 */
#define CMD17   17      /* READ_SINGLE_BLOCK - 读单块 */
#define CMD18   18      /* READ_MULTIPLE_BLOCK - 读多块 */
#define CMD23   23      /* SET_BLOCK_COUNT - 设置块数量 */
#define CMD24   24      /* WRITE_BLOCK - 写单块 */
#define CMD25   25      /* WRITE_MULTIPLE_BLOCK - 写多块 */
#define CMD41   41      /* SD_SEND_OP_COND - 发送操作条件(SD) */
#define CMD55   55      /* APP_CMD - 应用命令前缀 */
#define CMD58   58      /* READ_OCR - 读取OCR寄存器 */
#define CMD59   59      /* CRC_ON_OFF - CRC开关 */

/* CS引脚定义 - 使用PB5 (避免与DHT11的PB12冲突) */
#define SD_CS_PORT      GPIOB
#define SD_CS_PIN       GPIO_PIN_5

/* 调试变量 */
extern uint8_t SD_TYPE;         /* SD卡类型 */
extern uint8_t sd_init_step;    /* 初始化步骤 */
extern uint8_t sd_debug_r1;     /* 最后的R1响应 */
extern uint8_t sd_debug_spi;    /* SPI测试值 */
extern uint16_t sd_debug_retry; /* 重试次数 */
extern uint8_t sd_debug_wait;   /* 等待0xFF时的值 */

/* 函数声明 */
uint8_t SD_Init(void);
uint8_t SD_ReadDisk(uint8_t *buf, uint32_t sector, uint8_t cnt);
uint8_t SD_WriteDisk(uint8_t *buf, uint32_t sector, uint8_t cnt);
uint32_t SD_GetSectorCount(void);

#endif /* __SDCARD_H */
