#include "dc01.h"
#include <stdbool.h>

UART_HandleTypeDef huart2_dc01;
volatile uint8_t dc01_rx_data = 0;           // 用于存储接收到的字节
volatile uint8_t dc01_frame_counter = 0;     // 用于记录接收到的字节数量
volatile uint8_t dc01_DATAH = 0;             // 存储数据字节1
volatile uint8_t dc01_DATAL = 0;             // 存储数据字节2
volatile uint8_t dc01_CHECKSUM = 0;          // 存储校验字节
int DC01_PM25_Value = 0;

// 原有的其他变量
uint8_t Dc01_RecBuf[DC01_USART2_RXBUFF_SIZE];
volatile bool dc01_rev_start = 0;
volatile bool dc01_rev_stop = 0;
volatile uint8_t dc01_RxCounter = 0;

void DC01_USART2_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能USART2和GPIOA时钟
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // USART2 GPIO配置
    // TX: PA2, RX: PA3
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // USART2初始化
    huart2_dc01.Instance = USART2;
    huart2_dc01.Init.BaudRate = 9600;
    huart2_dc01.Init.WordLength = UART_WORDLENGTH_8B;
    huart2_dc01.Init.StopBits = UART_STOPBITS_1;
    huart2_dc01.Init.Parity = UART_PARITY_NONE;
    huart2_dc01.Init.Mode = UART_MODE_TX_RX;
    huart2_dc01.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2_dc01.Init.OverSampling = UART_OVERSAMPLING_16;
    
    HAL_UART_Init(&huart2_dc01);
    
    // 设置中断优先级（USART2优先级设为1，比USART3低）
    HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    
    // 使能USART2接收中断
    HAL_UART_Receive_IT(&huart2_dc01, (uint8_t*)&dc01_rx_data, 1);
}

// DC01串口接收中断处理函数（HAL库回调形式）
void DC01_UART_RxCpltCallback(void)
{
    uint8_t ucTemp = dc01_rx_data; // 读取接收到的数据
    
    
    if(dc01_frame_counter == 0 && ucTemp == 0xA5) // 如果是第一个字节且为特征字节
    {
        dc01_frame_counter++;
    }
    else if(dc01_frame_counter > 0 && dc01_frame_counter < 4) // 如果已经接收到特征字节，继续接收数据
    {
        if(dc01_frame_counter == 1)
        {
            dc01_DATAH = ucTemp;     // 存储数据字节1
            dc01_frame_counter++;
        }
        else if(dc01_frame_counter == 2)
        {
            dc01_DATAL = ucTemp;     // 存储数据字节2
            dc01_frame_counter++;
        }
        else if(dc01_frame_counter == 3)
        {
            dc01_CHECKSUM = ucTemp;  // 存储校验字节

            // 计算校验和：0xA5 + DATAH + DATAL，然后取低7位（与0x7F相与）
            uint8_t sum = (0xA5 + dc01_DATAH + dc01_DATAL) & 0x7F;

            if(sum != dc01_CHECKSUM) // 如果校验失败
            {
                // 可以在这里添加错误处理代码
                dc01_frame_counter = 0; // 重新计数
                // printf("DC01 Checksum Error!\n");
            }
            else
            {
                // 校验成功，计算浓度值
                // 注意：原代码是 (DATAH << 7) | (DATAL & 0x7F)
                // 这意味着DATAH是高7位，DATAL的低7位是低7位
                uint16_t concentration = (dc01_DATAH << 7) | (dc01_DATAL & 0x7F);
                DC01_PM25_Value = concentration;
                //printf("PM25:%d",DC01_PM25_Value);
                // 这里可以添加代码将浓度值发送给其他模块或显示
            }
            dc01_frame_counter = 0; // 重置计数器，准备接收下一帧数据
        }
    }
    else
    {
        dc01_frame_counter = 0;  // 如果接收到的数据不是特征字节，重置计数器
    }

	// 需要在函数最后添加
	HAL_UART_Receive_IT(&huart2_dc01, (uint8_t*)&dc01_rx_data, 1);
}

