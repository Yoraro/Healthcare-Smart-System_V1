#ifndef __JW01_H
#define __JW01_H

#include "main.h"
#include "stm32f1xx_hal.h"
#include <stdbool.h>

#define JW01_USART3_RXBUFF_SIZE   6

extern UART_HandleTypeDef huart3_jw01;
extern uint8_t Jw01_RecBuf[JW01_USART3_RXBUFF_SIZE];
extern volatile bool jw01_rev_start;
extern volatile bool jw01_rev_stop;
extern volatile uint8_t jw01_RxCounter;
extern uint16_t JW01_CO2_Value;
extern volatile uint8_t jw01_rx_data;
extern volatile uint32_t jw01_receive_count;  // 添加这行

void JW01_UART_RxCpltCallback(void);
void JW01_USART3_Init(void);
uint16_t JW01_Get_CO2(void);

#endif
