#ifndef __DC01_H
#define __DC01_H

#include "main.h"
#include "stm32f1xx_hal.h"
#include <stdbool.h>

#define DC01_USART2_RXBUFF_SIZE   54 

// 声明全局变量
extern UART_HandleTypeDef huart2_dc01;
extern uint8_t Dc01_RecBuf[DC01_USART2_RXBUFF_SIZE];
extern volatile bool dc01_rev_start;
extern volatile bool dc01_rev_stop;
extern volatile uint8_t dc01_RxCounter;
extern int DC01_PM25_Value;

// 新添加的变量声明
extern volatile uint8_t dc01_rx_data;
extern volatile uint8_t dc01_frame_counter;
extern volatile uint8_t dc01_DATAH;
extern volatile uint8_t dc01_DATAL;
extern volatile uint8_t dc01_CHECKSUM;

// 函数声明
void DC01_UART_RxCpltCallback(void);
void DC01_USART2_Init(void);

#endif
