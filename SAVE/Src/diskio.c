/**
 * @file    diskio.c
 * @brief   FatFs底层磁盘接口 - 适配SD卡(SPI模式)
 */

#include "diskio.h"
#include "sdcard.h"

/* 磁盘状态 */
static volatile DSTATUS Stat = STA_NOINIT;

/**
 * @brief  获取磁盘状态
 */
DSTATUS disk_status(BYTE pdrv)
{
    if (pdrv != 0) return STA_NOINIT;
    return Stat;
}

/**
 * @brief  初始化磁盘
 */
DSTATUS disk_initialize(BYTE pdrv)
{
    if (pdrv != 0) return STA_NOINIT;
    
    if (SD_Init() == 0) {
        Stat = 0;  /* 初始化成功 */
    } else {
        Stat = STA_NOINIT;
    }
    return Stat;
}

/**
 * @brief  读扇区
 */
DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    if (pdrv != 0 || count == 0) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    
    if (SD_ReadDisk(buff, sector, count) == 0) {
        return RES_OK;
    }
    return RES_ERROR;
}

/**
 * @brief  写扇区
 */
DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    if (pdrv != 0 || count == 0) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    
    if (SD_WriteDisk((BYTE*)buff, sector, count) == 0) {
        return RES_OK;
    }
    return RES_ERROR;
}

/**
 * @brief  磁盘控制
 */
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    DRESULT res = RES_ERROR;
    
    if (pdrv != 0) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    
    switch (cmd) {
        case CTRL_SYNC:         /* 同步 */
            res = RES_OK;
            break;
            
        case GET_SECTOR_COUNT:  /* 获取扇区数 */
            *(DWORD*)buff = SD_GetSectorCount();
            res = RES_OK;
            break;
            
        case GET_SECTOR_SIZE:   /* 获取扇区大小 */
            *(WORD*)buff = 512;
            res = RES_OK;
            break;
            
        case GET_BLOCK_SIZE:    /* 获取擦除块大小 */
            *(DWORD*)buff = 1;  /* 1个扇区 */
            res = RES_OK;
            break;
            
        default:
            res = RES_PARERR;
            break;
    }
    return res;
}

/**
 * @brief  获取时间戳(禁用RTC时不需要)
 */
DWORD get_fattime(void)
{
    /* 返回固定时间: 2025-01-01 00:00:00 */
    return ((DWORD)(2025 - 1980) << 25)
         | ((DWORD)1 << 21)
         | ((DWORD)1 << 16)
         | ((DWORD)0 << 11)
         | ((DWORD)0 << 5)
         | ((DWORD)0 >> 1);
}
