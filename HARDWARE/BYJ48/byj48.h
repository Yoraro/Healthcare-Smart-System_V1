#ifndef __BYJ48_H
#define __BYJ48_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


// 步进电机引脚定义
#define MOTOR_PORT        GPIOB

#define MOTOR_A_PIN       GPIO_PIN_0
#define MOTOR_B_PIN       GPIO_PIN_1
#define MOTOR_C_PIN       GPIO_PIN_3
#define MOTOR_D_PIN       GPIO_PIN_4

// 引脚控制宏定义（使用HAL库函数）
#define MOTOR_A_HIGH      HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_A_PIN, GPIO_PIN_SET)
#define MOTOR_A_LOW       HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_A_PIN, GPIO_PIN_RESET)

#define MOTOR_B_HIGH      HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_B_PIN, GPIO_PIN_SET)
#define MOTOR_B_LOW       HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_B_PIN, GPIO_PIN_RESET)

#define MOTOR_C_HIGH      HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_C_PIN, GPIO_PIN_SET)
#define MOTOR_C_LOW       HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_C_PIN, GPIO_PIN_RESET)

#define MOTOR_D_HIGH      HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_D_PIN, GPIO_PIN_SET)
#define MOTOR_D_LOW       HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_D_PIN, GPIO_PIN_RESET)

/* 函数声明 */
void MOTOR_Init(void);
void MOTOR_Rhythm_4_1_4(uint8_t step, uint8_t dly);
void MOTOR_Rhythm_4_2_4(uint8_t step, uint8_t dly);
void MOTOR_Rhythm_4_1_8(uint8_t step, uint8_t dly);
void MOTOR_Direction(uint8_t dir, uint8_t num, uint8_t dly);
void MOTOR_Direction_Angle(uint8_t dir, uint8_t num, uint16_t angle, uint8_t dly);
void MOTOR_STOP(void);

#ifdef __cplusplus
}
#endif

#endif /* __BYJ48_H */
