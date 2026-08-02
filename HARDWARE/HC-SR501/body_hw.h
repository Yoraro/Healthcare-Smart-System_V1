#ifndef __BODY_HW_H
#define __BODY_HW_H

#include "main.h"

/*****************辰哥单片机设计******************
 * 文件: HC-SR501人体红外传感器 HAL 驱动头文件
 * 版本: V2.0 (HAL版)
 * 日期: 2025.10.16
 * MCU : STM32F103C8T6
 *************************************************/

/***************根据自己需求更改****************/

#define BODY_HW_GPIO_PORT      GPIOB
#define BODY_HW_GPIO_PIN       GPIO_PIN_8
#define BODY_HW_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

/*********************END**********************/

void BODY_HW_Init(void);
uint16_t BODY_HW_GetData(void);

#endif /* __BODY_HW_H */
