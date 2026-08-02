/**
 * @file    bsp_spi.h
 * @brief   SPI2驱动头文件 - 适用于STM32F103
 * @note    使用SPI2避免与按键PA7冲突
 *          PB13(SCK), PB14(MISO), PB15(MOSI), PB12(CS)
 */

#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "stm32f1xx_hal.h"

/* SPI句柄 */
extern SPI_HandleTypeDef hspi2;

/* 函数声明 */
void BSP_SPI2_Init(void);
void BSP_SPI_SetSpeed(uint32_t prescaler);
uint8_t BSP_SPI_ReadWriteByte(uint8_t TxData);

#endif /* __BSP_SPI_H */
