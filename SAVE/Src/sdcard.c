/**
 * @file    sdcard.c
 * @brief   SD卡驱动 - 适用于STM32F103 (SPI模式)
 * @note    支持SD V1.x, SD V2.0, SDHC, MMC卡
 */

#include "sdcard.h"
/* 全局变量 */
uint8_t SD_TYPE = 0;            /* SD卡类型 */
uint8_t sd_init_step = 0;       /* 初始化步骤(调试用) */
uint8_t sd_debug_r1 = 0xFF;     /* 最后的R1响应 */
uint8_t sd_debug_spi = 0xFF;    /* SPI测试值 */
uint16_t sd_debug_retry = 0;    /* 重试次数 */
uint8_t sd_debug_wait = 0xFF;   /* 等待0xFF时的值 */

/**
 * @brief  SD卡片选控制
 * @param  p: 0=取消选中(高电平), 1=选中(低电平)
 */
static void SD_CS(uint8_t p)
{
    if (p == 0)
        HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, GPIO_PIN_RESET);
}

/**
 * @brief  发送SD卡命令
 * @param  cmd: 命令索引
 * @param  arg: 命令参数
 * @param  crc: CRC校验值
 * @return R1响应值
 */
static int SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t r1;
    uint16_t retry;
    
    /* 取消选中再选中 */
    SD_CS(0);
    HAL_Delay(20);
    SD_CS(1);
    
    /* 等待SD卡准备好 */
    retry = 200;
    do {
        r1 = BSP_SPI_ReadWriteByte(0xFF);
        sd_debug_wait = r1;
        if (--retry == 0) return 0xFE;  /* 超时 */
    } while (r1 != 0xFF);
    
    /* 发送命令 */
    BSP_SPI_ReadWriteByte(cmd | 0x40);
    BSP_SPI_ReadWriteByte(arg >> 24);
    BSP_SPI_ReadWriteByte(arg >> 16);
    BSP_SPI_ReadWriteByte(arg >> 8);
    BSP_SPI_ReadWriteByte(arg);
    BSP_SPI_ReadWriteByte(crc);
    
    /* CMD12需要额外的字节 */
    if (cmd == CMD12) BSP_SPI_ReadWriteByte(0xFF);
    
    /* 等待响应 */
    retry = 200;
    do {
        r1 = BSP_SPI_ReadWriteByte(0xFF);
        if (--retry == 0) return 0xFF;
    } while (r1 & 0x80);
    
    return r1;
}

/**
 * @brief  接收数据块
 * @param  data: 数据缓冲区
 * @param  len: 数据长度
 * @return 0=成功, 1=失败
 */
static uint8_t SD_ReceiveData(uint8_t *data, uint16_t len)
{
    uint8_t r1;
    uint16_t retry = 2000;
    
    SD_CS(1);
    
    /* 等待数据起始令牌0xFE */
    do {
        r1 = BSP_SPI_ReadWriteByte(0xFF);
        if (--retry == 0) return 1;
    } while (r1 != 0xFE);
    
    /* 接收数据 */
    while (len--) {
        *data = BSP_SPI_ReadWriteByte(0xFF);
        data++;
    }
    
    /* 读取CRC(丢弃) */
    BSP_SPI_ReadWriteByte(0xFF);
    BSP_SPI_ReadWriteByte(0xFF);
    
    return 0;
}


/**
 * @brief  发送数据块
 * @param  buf: 数据缓冲区
 * @param  cmd: 数据令牌
 * @return 0=成功, 非0=失败
 */
static uint8_t SD_SendBlock(uint8_t *buf, uint8_t cmd)
{
    uint16_t t;
    uint8_t r1;
    uint16_t retry;
    
    /* 等待SD卡准备好 */
    retry = 500;
    do {
        r1 = BSP_SPI_ReadWriteByte(0xFF);
        if (--retry == 0) return 1;  /* 超时 */
    } while (r1 != 0xFF);
    
    /* 发送数据令牌 */
    BSP_SPI_ReadWriteByte(cmd);
    
    if (cmd != 0xFD) {
        /* 发送512字节数据 */
        for (t = 0; t < 512; t++)
            BSP_SPI_ReadWriteByte(buf[t]);
        
        /* 发送CRC(dummy) */
        BSP_SPI_ReadWriteByte(0xFF);
        BSP_SPI_ReadWriteByte(0xFF);
        
        /* 读取响应 */
        t = BSP_SPI_ReadWriteByte(0xFF);
        if ((t & 0x1F) != 0x05) return 2;  /* 数据未被接受 */
        
        /* 等待SD卡写入完成(BUSY状态) */
        retry = 50000;
        do {
            r1 = BSP_SPI_ReadWriteByte(0xFF);
            if (--retry == 0) return 3;  /* 写入超时 */
        } while (r1 == 0x00);
    }
    
    return 0;
}

/**
 * @brief  SD卡初始化
 * @return 0=成功, 非0=失败
 */
uint8_t SD_Init(void)
{
    uint8_t r1;
    uint8_t buff[6] = {0};
    uint16_t retry;
    uint8_t i;
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    sd_init_step = 1;
    
    /* 初始化CS引脚 - PB5 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, GPIO_PIN_SET);
    GPIO_InitStruct.Pin = SD_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SD_CS_PORT, &GPIO_InitStruct);
    
    sd_init_step = 2;
    
    /* 初始化SPI2 - 低速模式 */
    BSP_SPI2_Init();
    BSP_SPI_SetSpeed(SPI_BAUDRATEPRESCALER_256);
    
    /* 等待SD卡上电稳定 */
    HAL_Delay(100);
    
    sd_init_step = 3;
    
    /* CS拉高，发送至少74个时钟脉冲让SD卡进入SPI模式 */
    SD_CS(0);  /* CS高 */
    for (retry = 0; retry < 20; retry++) {
        BSP_SPI_ReadWriteByte(0xFF);
    }
    
    sd_init_step = 4;
    
    /* 发送CMD0进入IDLE状态 */
    retry = 20;
    do {
        r1 = SD_SendCmd(CMD0, 0, 0x95);
        sd_debug_r1 = r1;
        sd_debug_retry = retry;
        HAL_Delay(10);
    } while (r1 != 0x01 && --retry);
    
    if (retry == 0) {
        sd_init_step = 40;  /* CMD0超时 */
        SD_CS(0);
        return 1;
    }
    
    sd_init_step = 5;
    
    /* 检查SD卡版本 */
    SD_TYPE = 0;
    r1 = SD_SendCmd(CMD8, 0x1AA, 0x87);
    sd_debug_r1 = r1;
    
    if (r1 == 0x01) {
        /* SD V2.0卡 */
        sd_init_step = 6;
        
        /* 读取R7响应 */
        for (i = 0; i < 4; i++)
            buff[i] = BSP_SPI_ReadWriteByte(0xFF);
        
        if (buff[2] == 0x01 && buff[3] == 0xAA) {
            sd_init_step = 7;
            
            /* 发送ACMD41初始化 */
            retry = 500;
            do {
                SD_SendCmd(CMD55, 0, 0x01);
                r1 = SD_SendCmd(CMD41, 0x40000000, 0x01);
                sd_debug_r1 = r1;
                sd_debug_retry = retry;
                if (--retry == 0) {
                    sd_init_step = 70;  /* ACMD41超时 */
                    SD_CS(0);
                    return 2;
                }
            } while (r1);
            
            sd_init_step = 8;
            
            /* 读取OCR判断是否为SDHC */
            if (SD_SendCmd(CMD58, 0, 0x01) == 0) {
                for (i = 0; i < 4; i++)
                    buff[i] = BSP_SPI_ReadWriteByte(0xFF);
                if (buff[0] & 0x40)
                    SD_TYPE = SD_TYPE_V2HC;
                else
                    SD_TYPE = SD_TYPE_V2;
            }
        } else {
            /* CMD8响应错误，尝试V1卡 */
            sd_init_step = 60;
            
            SD_SendCmd(CMD55, 0, 0x01);
            r1 = SD_SendCmd(CMD41, 0, 0x01);
            
            if (r1 <= 1) {
                SD_TYPE = SD_TYPE_V1;
                retry = 500;
                do {
                    SD_SendCmd(CMD55, 0, 0x01);
                    r1 = SD_SendCmd(CMD41, 0, 0x01);
                } while (r1 && retry--);
            } else {
                /* MMC卡 */
                SD_TYPE = SD_TYPE_MMC;
                retry = 500;
                do {
                    r1 = SD_SendCmd(CMD1, 0, 0x01);
                } while (r1 && retry--);
            }
            
            if (retry == 0 || SD_SendCmd(CMD16, 512, 0x01) != 0)
                SD_TYPE = SD_TYPE_ERR;
        }
    } else {
        /* CMD8失败，可能是V1卡或MMC */
        sd_init_step = 50;
        
        SD_SendCmd(CMD55, 0, 0x01);
        r1 = SD_SendCmd(CMD41, 0, 0x01);
        
        if (r1 <= 1) {
            SD_TYPE = SD_TYPE_V1;
            retry = 500;
            do {
                SD_SendCmd(CMD55, 0, 0x01);
                r1 = SD_SendCmd(CMD41, 0, 0x01);
            } while (r1 && retry--);
        } else {
            SD_TYPE = SD_TYPE_MMC;
            retry = 500;
            do {
                r1 = SD_SendCmd(CMD1, 0, 0x01);
            } while (r1 && retry--);
        }
        
        if (retry == 0 || SD_SendCmd(CMD16, 512, 0x01) != 0)
            SD_TYPE = SD_TYPE_ERR;
    }
    
    /* 取消片选，切换到高速模式(8分频更稳定) */
    SD_CS(0);
    BSP_SPI_SetSpeed(SPI_BAUDRATEPRESCALER_8);
    
    if (SD_TYPE) {
        sd_init_step = 100;  /* 成功 */
        return 0;
    }
    return 1;
}


/**
 * @brief  读取SD卡扇区
 * @param  buf: 数据缓冲区
 * @param  sector: 扇区地址
 * @param  cnt: 扇区数量
 * @return 0=成功, 非0=失败
 */
uint8_t SD_ReadDisk(uint8_t *buf, uint32_t sector, uint8_t cnt)
{
    uint8_t r1;
    
    /* 非SDHC卡需要将扇区地址转换为字节地址 */
    if (SD_TYPE != SD_TYPE_V2HC) sector <<= 9;
    
    if (cnt == 1) {
        /* 单块读取 */
        r1 = SD_SendCmd(CMD17, sector, 0x01);
        if (r1 == 0) r1 = SD_ReceiveData(buf, 512);
    } else {
        /* 多块读取 */
        r1 = SD_SendCmd(CMD18, sector, 0x01);
        do {
            r1 = SD_ReceiveData(buf, 512);
            buf += 512;
        } while (--cnt && r1 == 0);
        SD_SendCmd(CMD12, 0, 0x01);  /* 停止传输 */
    }
    
    SD_CS(0);
    return r1;
}

/**
 * @brief  写入SD卡扇区
 * @param  buf: 数据缓冲区
 * @param  sector: 扇区地址
 * @param  cnt: 扇区数量
 * @return 0=成功, 非0=失败
 */
uint8_t SD_WriteDisk(uint8_t *buf, uint32_t sector, uint8_t cnt)
{
    uint8_t r1;
    uint16_t retry;
    
    /* 非SDHC卡需要将扇区地址转换为字节地址 */
    if (SD_TYPE != SD_TYPE_V2HC) sector *= 512;
    
    if (cnt == 1) {
        /* 单块写入 */
        r1 = SD_SendCmd(CMD24, sector, 0x01);
        if (r1 == 0) r1 = SD_SendBlock(buf, 0xFE);
    } else {
        /* 多块写入 */
        if (SD_TYPE != SD_TYPE_MMC) {
            SD_SendCmd(CMD55, 0, 0x01);
            SD_SendCmd(CMD23, cnt, 0x01);  /* 预设块数量 */
        }
        r1 = SD_SendCmd(CMD25, sector, 0x01);
        if (r1 == 0) {
            do {
                r1 = SD_SendBlock(buf, 0xFC);
                buf += 512;
            } while (--cnt && r1 == 0);
            r1 = SD_SendBlock(0, 0xFD);  /* 停止令牌 */
        }
    }
    
    /* 等待SD卡完全空闲 */
    retry = 500;
    do {
        r1 = BSP_SPI_ReadWriteByte(0xFF);
    } while (r1 != 0xFF && --retry);
    
    SD_CS(0);
    return (retry == 0) ? 4 : 0;
}

/**
 * @brief  获取CSD寄存器
 * @param  csd_data: CSD数据缓冲区(16字节)
 * @return 0=成功, 非0=失败
 */
static uint8_t SD_GetCSD(uint8_t *csd_data)
{
    uint8_t r1;
    r1 = SD_SendCmd(CMD9, 0, 0x01);
    if (r1 == 0) r1 = SD_ReceiveData(csd_data, 16);
    SD_CS(0);
    return r1;
}

/**
 * @brief  获取SD卡扇区数量
 * @return 扇区数量(每扇区512字节)
 */
uint32_t SD_GetSectorCount(void)
{
    uint8_t csd[16];
    uint32_t Capacity;
    uint8_t n;
    uint16_t csize;
    
    if (SD_GetCSD(csd) != 0) return 0;
    
    /* 根据CSD版本计算容量 */
    if ((csd[0] & 0xC0) == 0x40) {
        /* CSD V2.0 (SDHC) */
        csize = csd[9] + ((uint16_t)csd[8] << 8) + 1;
        Capacity = (uint32_t)csize << 10;  /* 扇区数 = (C_SIZE+1) * 1024 */
    } else {
        /* CSD V1.0 */
        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
        csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
        Capacity = (uint32_t)csize << (n - 9);
    }
    
    return Capacity;
}
