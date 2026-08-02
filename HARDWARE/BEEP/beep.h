#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f1xx_hal.h"

// 初始化蜂鸣器
void Beep_Init(void);

void Beep_On(void);
void Beep_Off(void);


#endif
