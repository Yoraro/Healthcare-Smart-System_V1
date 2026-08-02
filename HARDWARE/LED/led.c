#include "led.h"

void LED_Init(void)//电灯
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  /* GPIO端口时钟使能 */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  /* 配置PA8为输出模式（LED引脚）*/
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      // 推挽输出
  GPIO_InitStruct.Pull = GPIO_NOPULL;              // 不上拉/下拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;     // 低速输出
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* 初始化LED为熄灭状态 */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

}

void LED_On(void)
{
    // 输出低电平，风扇开启
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
}

void LED_Off(void)
{
    // 输出高电平，风扇关闭
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
}
