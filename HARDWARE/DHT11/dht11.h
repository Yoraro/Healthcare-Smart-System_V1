#ifndef __DHT11_H
#define __DHT11_H

#include "main.h"
#include <stdint.h> // 使用标准整数类型

/*******************************/
// DHT11 引脚定义
#define DHT11_GPIO_PORT  GPIOA
#define DHT11_GPIO_PIN   GPIO_PIN_0

// 方向定义
#define OUT 1
#define IN  0

// DHT11 引脚电平控制
#define DHT11_Low   HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_RESET)
#define DHT11_High  HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_SET)
#define DHT11_Read  HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN)

// 函数声明
uint8_t DHT11_Init(void);                    // 初始化DHT11
uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi);  // 读取温湿度数据
uint8_t DHT11_Read_Byte(void);               // 读取一个字节的数据
uint8_t DHT11_Read_Bit(void);                // 读取一个位的数据
void DHT11_Mode(uint8_t mode);               // 设置DHT11引脚方向
uint8_t DHT11_Check(void);                   // 检测DHT11
void DHT11_Rst(void);                        // 复位DHT11   

#endif
