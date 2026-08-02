/**
 * @file    app_sdcard.c
 * @brief   SD卡应用层实现
 */

#include "app_sdcard.h"

/* FatFs对象 */
static FATFS fs;
static FIL file;
static uint8_t sd_mounted = 0;

/**
 * @brief  SD卡应用初始化(挂载文件系统)
 */
SD_App_Status_t SD_App_Init(void)
{
    FRESULT res;
    
    if(sd_mounted) return SD_APP_OK;
    
    res = f_mount(&fs, "0:", 1);
    if(res != FR_OK) {
        printf("[SD] Mount Failed! res=%d\r\n", res);
        printf("[SD] Type=%d, Step=%d, R1=0x%02X\r\n", SD_TYPE, sd_init_step, sd_debug_r1);
        return SD_APP_MOUNT_ERR;
    }
    
    sd_mounted = 1;
    printf("[SD] Mount OK!\r\n");
    return SD_APP_OK;
}

/**
 * @brief  SD卡应用反初始化(卸载文件系统)
 */
void SD_App_DeInit(void)
{
    if(sd_mounted) {
        f_mount(NULL, "0:", 0);
        sd_mounted = 0;
        printf("[SD] Unmounted\r\n");
    }
}

/**
 * @brief  写入文件(覆盖)
 */
SD_App_Status_t SD_App_WriteFile(const char *filename, const char *data)
{
    FRESULT res;
    UINT bw;
    uint32_t len = strlen(data);
    
    if(!sd_mounted) {
        if(SD_App_Init() != SD_APP_OK) return SD_APP_MOUNT_ERR;
    }
    
    res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if(res != FR_OK) {
        printf("[SD] Open Failed! res=%d\r\n", res);
        return SD_APP_OPEN_ERR;
    }
    
    res = f_write(&file, data, len, &bw);
    if(res != FR_OK || bw != len) {
        printf("[SD] Write Failed! res=%d, bw=%d\r\n", res, bw);
        f_close(&file);
        return SD_APP_WRITE_ERR;
    }
    
    /* 同步数据到SD卡 */
    res = f_sync(&file);
    if(res != FR_OK) {
        printf("[SD] Sync Failed! res=%d\r\n", res);
    }
    
    f_close(&file);
    
    printf("[SD] Write OK: %s (%d bytes)\r\n", filename, bw);
    return SD_APP_OK;
}

/**
 * @brief  读取文件
 */
SD_App_Status_t SD_App_ReadFile(const char *filename, char *buf, uint32_t bufsize)
{
    FRESULT res;
    UINT br;
    
    if(!sd_mounted) {
        if(SD_App_Init() != SD_APP_OK) return SD_APP_MOUNT_ERR;
    }
    
    res = f_open(&file, filename, FA_READ);
    if(res != FR_OK) {
        printf("[SD] Open Failed! res=%d\r\n", res);
        return SD_APP_OPEN_ERR;
    }
    
    memset(buf, 0, bufsize);
    res = f_read(&file, buf, bufsize - 1, &br);
    f_close(&file);
    
    if(res != FR_OK) {
        printf("[SD] Read Failed! res=%d\r\n", res);
        return SD_APP_READ_ERR;
    }
    
    printf("[SD] Read OK: %s (%d bytes)\r\n", filename, br);
    return SD_APP_OK;
}

/**
 * @brief  追加写入文件
 */
SD_App_Status_t SD_App_AppendFile(const char *filename, const char *data)
{
    FRESULT res;
    UINT bw;
    
    if(!sd_mounted) {
        if(SD_App_Init() != SD_APP_OK) return SD_APP_MOUNT_ERR;
    }
    
    res = f_open(&file, filename, FA_OPEN_ALWAYS | FA_WRITE);
    if(res != FR_OK) {
        printf("[SD] Open Failed! res=%d\r\n", res);
        return SD_APP_OPEN_ERR;
    }
    
    /* 移动到文件末尾 */
    f_lseek(&file, f_size(&file));
    
    res = f_write(&file, data, strlen(data), &bw);
    f_close(&file);
    
    if(res != FR_OK) {
        printf("[SD] Append Failed! res=%d\r\n", res);
        return SD_APP_WRITE_ERR;
    }
    
    printf("[SD] Append OK: %s (%d bytes)\r\n", filename, bw);
    return SD_APP_OK;
}

/**
 * @brief  SD卡测试
 */
void SD_App_Test(void)
{
    char readbuf[64];
    
    printf("\r\n--- SD Card Test ---\r\n");
    
    /* 写入测试 */
    if(SD_App_WriteFile("0:/data.txt", "20000") != SD_APP_OK) {
        printf("[SD] Test Failed!\r\n");
        return;
    }
    
    /* 读回验证 */
    if(SD_App_ReadFile("0:/data.txt", readbuf, sizeof(readbuf)) == SD_APP_OK) {
        printf("[SD] Content: %s\r\n", readbuf);
    }
    
    /* 卸载 */
    SD_App_DeInit();
    
    printf("--- SD Test Complete ---\r\n\r\n");
}
