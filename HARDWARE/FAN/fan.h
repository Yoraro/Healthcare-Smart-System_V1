// fan.h
#ifndef __FAN_H
#define __FAN_H

#include "stm32f1xx_hal.h"

// 风扇初始化
void Fan_Init(void);

// 风扇开启（低电平驱动）
void Fan_On(void);

// 风扇关闭（高电平停止）
void Fan_Off(void);

#endif
