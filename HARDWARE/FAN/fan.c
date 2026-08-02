// fan.c
#include "fan.h"

// 风扇控制引脚定义
#define FAN_GPIO_PORT    GPIOB
#define FAN_GPIO_PIN     GPIO_PIN_9

void Fan_Init(void)
{
    // 使能GPIOA时钟（如果未使能）
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // GPIO初始化结构体
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 配置PA1为推挽输出
    GPIO_InitStruct.Pin = FAN_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;              // 不上拉不下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;     // 低速
    
    HAL_GPIO_Init(FAN_GPIO_PORT, &GPIO_InitStruct);
    
    // 初始化状态：风扇关闭（高电平）
    Fan_Off();
}

void Fan_On(void)
{
    // 输出低电平，风扇开启
    HAL_GPIO_WritePin(FAN_GPIO_PORT, FAN_GPIO_PIN, GPIO_PIN_RESET);
}

void Fan_Off(void)
{
    // 输出高电平，风扇关闭
    HAL_GPIO_WritePin(FAN_GPIO_PORT, FAN_GPIO_PIN, GPIO_PIN_SET);
}
