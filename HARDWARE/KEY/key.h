#ifndef __KEY_H
#define __KEY_H

#include "stm32f1xx_hal.h"
#include "tim.h"
/* ================== 按键事件类型 ================== */
typedef enum
{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SHORT,
    KEY_EVENT_LONG
} KeyEvent_t;

/* ================== 对外事件变量 ================== */
extern volatile KeyEvent_t KEY1_Event;
extern volatile KeyEvent_t KEY2_Event;
extern volatile KeyEvent_t KEY3_Event;
extern volatile KeyEvent_t KEY4_Event;

/* ================== 接口函数 ================== */
void KEY_Init(void);
void KEY_Scan_10ms(void);

#endif

