// jw01.c - 修正后的完整代码
#include "jw01.h"
#include <stdbool.h>

UART_HandleTypeDef huart3_jw01;
uint8_t Jw01_RecBuf[JW01_USART3_RXBUFF_SIZE];
volatile uint8_t jw01_RxCounter = 0;
uint16_t JW01_CO2_Value = 0;
volatile uint8_t jw01_rx_data = 0;
volatile uint32_t jw01_rx_total = 0;

void JW01_USART3_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能时钟
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // USART3 GPIO配置
    GPIO_InitStruct.Pin = GPIO_PIN_10;  // TX
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_11;  // RX
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // USART3初始化
    huart3_jw01.Instance = USART3;
    huart3_jw01.Init.BaudRate = 9600;
    huart3_jw01.Init.WordLength = UART_WORDLENGTH_8B;
    huart3_jw01.Init.StopBits = UART_STOPBITS_1;
    huart3_jw01.Init.Parity = UART_PARITY_NONE;
    huart3_jw01.Init.Mode = UART_MODE_TX_RX;
    huart3_jw01.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3_jw01.Init.OverSampling = UART_OVERSAMPLING_16;
    
    HAL_UART_Init(&huart3_jw01);
    
    // 设置中断优先级（比USART2低）
    HAL_NVIC_SetPriority(USART3_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    
    // 使能接收中断（添加类型转换）
    HAL_UART_Receive_IT(&huart3_jw01, (uint8_t*)&jw01_rx_data, 1);
}

// JW01串口接收中断处理函数
void JW01_UART_RxCpltCallback(void){
    uint8_t rx_byte = jw01_rx_data;  // 复制到非volatile变量
    static uint8_t frame_pos = 0;
    
    // 记录接收次数
    jw01_rx_total++;
    
    // 简单帧解析
    if(rx_byte == 0x2C && frame_pos == 0) {
        // 收到起始字节
        Jw01_RecBuf[0] = rx_byte;
        frame_pos = 1;
    } 
    else if(frame_pos > 0 && frame_pos < 6) {
        // 存储数据字节
        Jw01_RecBuf[frame_pos] = rx_byte;
        frame_pos++;
        
        // 检查是否收到完整帧
        if(frame_pos >= 6) {
            // 简单计算校验和
            uint8_t calc_checksum = Jw01_RecBuf[0] + Jw01_RecBuf[1] + 
                                    Jw01_RecBuf[2] + Jw01_RecBuf[3] + 
                                    Jw01_RecBuf[4];
            // 验证校验和
            if(calc_checksum == Jw01_RecBuf[5]) {
                // 计算CO2值：B2 * 256 + B3
                JW01_CO2_Value = (Jw01_RecBuf[1] << 8) | Jw01_RecBuf[2];
            }
            else {
                // 校验失败
                JW01_CO2_Value = 0;
            }
            
            // 重置帧位置
            frame_pos = 0;
        }
    }
    else {
        // 非预期数据，重置
        frame_pos = 0;
    }
    
    // 重新使能接收（添加类型转换）
    HAL_UART_Receive_IT(&huart3_jw01, (uint8_t*)&jw01_rx_data, 1);
}

uint16_t JW01_Get_CO2(void) {
    return JW01_CO2_Value;
}
