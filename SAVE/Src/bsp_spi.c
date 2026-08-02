/**
 * @file    bsp_spi.c
 * @brief   SPI2驱动 - 适用于STM32F103
 * @note    PB13(SCK), PB14(MISO), PB15(MOSI)
 */

#include "bsp_spi.h"
#include <stdio.h>

SPI_HandleTypeDef hspi2;

/**
 * @brief  SPI2初始化
 */
void BSP_SPI2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能时钟 */
    __HAL_RCC_SPI2_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    /* PB13(SCK) - 复用推挽输出 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* PB15(MOSI) - 复用推挽输出 */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* PB14(MISO) - 浮空输入 */
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* 先禁用SPI */
    SPI2->CR1 = 0;
    SPI2->CR2 = 0;
    
    /* CR1配置:
     * Bit9 SSM=1: 软件NSS管理
     * Bit8 SSI=1: 内部NSS高电平
     * Bit7 LSBFIRST=0: MSB先发
     * Bit6 SPE=0: 先不使能
     * Bit5:3 BR=111: 256分频
     * Bit2 MSTR=1: 主模式
     * Bit1 CPOL=0: 空闲低电平
     * Bit0 CPHA=0: 第一边沿采样
     */
    SPI2->CR1 = (1 << 9) |   /* SSM */
                (1 << 8) |   /* SSI */
                (7 << 3) |   /* BR=111 (256分频) */
                (1 << 2);    /* MSTR */
    
    /* 使能SPI */
    SPI2->CR1 |= (1 << 6);   /* SPE */
    
    printf("[SPI2] Init: CR1=0x%04X\r\n", (unsigned int)SPI2->CR1);
}

/**
 * @brief  设置SPI速度
 */
void BSP_SPI_SetSpeed(uint32_t prescaler)
{
    uint32_t br;
    
    /* 禁用SPI */
    SPI2->CR1 &= ~(1 << 6);
    
    /* 计算BR位 */
    switch(prescaler) {
        case SPI_BAUDRATEPRESCALER_2:   br = 0; break;
        case SPI_BAUDRATEPRESCALER_4:   br = 1; break;
        case SPI_BAUDRATEPRESCALER_8:   br = 2; break;
        case SPI_BAUDRATEPRESCALER_16:  br = 3; break;
        case SPI_BAUDRATEPRESCALER_32:  br = 4; break;
        case SPI_BAUDRATEPRESCALER_64:  br = 5; break;
        case SPI_BAUDRATEPRESCALER_128: br = 6; break;
        default:                        br = 7; break;
    }
    
    /* 清除BR位并设置新值 */
    SPI2->CR1 = (SPI2->CR1 & ~(7 << 3)) | (br << 3);
    
    /* 使能SPI */
    SPI2->CR1 |= (1 << 6);
}

/**
 * @brief  SPI读写一个字节
 */
uint8_t BSP_SPI_ReadWriteByte(uint8_t TxData)
{
    uint32_t timeout;
    
    /* 等待发送缓冲区空 */
    timeout = 0x1000;
    while (!(SPI2->SR & SPI_SR_TXE)) {
        if (--timeout == 0) return 0xFF;
    }
    
    /* 发送数据 */
    *((__IO uint8_t *)&SPI2->DR) = TxData;
    
    /* 等待接收缓冲区非空 */
    timeout = 0x1000;
    while (!(SPI2->SR & SPI_SR_RXNE)) {
        if (--timeout == 0) return 0xFF;
    }
    
    /* 返回接收的数据 */
    return (uint8_t)SPI2->DR;
}
