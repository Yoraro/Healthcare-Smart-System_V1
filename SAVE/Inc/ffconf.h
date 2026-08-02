/**
 * FatFs配置文件 - 适用于STM32F103 + SD卡(SPI模式)
 * 精简配置，仅保留必要功能
 */

#ifndef _FFCONF
#define _FFCONF 32020   /* 版本号，必须与ff.h匹配 */

/*---------------------------------------------------------------------------/
/ 功能配置
/---------------------------------------------------------------------------*/

#define _FS_TINY        0   /* 0:正常模式 1:精简模式 */
#define _FS_READONLY    0   /* 0:读写 1:只读 */
#define _FS_MINIMIZE    0   /* 0:全功能 1-3:精简 */
#define _USE_STRFUNC    1   /* 0:禁用 1-2:启用字符串函数 */
#define _USE_FIND       0   /* 0:禁用查找功能 */
#define _USE_MKFS       1   /* 0:禁用 1:启用f_mkfs */
#define _USE_FASTSEEK   0   /* 0:禁用快速定位 */
#define _USE_LABEL      0   /* 0:禁用卷标 */
#define _USE_FORWARD    0   /* 0:禁用f_forward */

/*---------------------------------------------------------------------------/
/ 区域设置
/---------------------------------------------------------------------------*/

#define _CODE_PAGE      1252    /* 1252=Latin1(西欧) */
#define _USE_LFN        1       /* 1:启用长文件名(支持大小写) */
#define _MAX_LFN        64      /* 最大文件名长度 */
#define _LFN_UNICODE    0       /* 0:ANSI/OEM */
#define _STRF_ENCODE    0
#define _FS_RPATH       0       /* 0:禁用相对路径 */

/*---------------------------------------------------------------------------/
/ 驱动/卷配置
/---------------------------------------------------------------------------*/

#define _VOLUMES        1       /* 逻辑驱动器数量 */
#define _STR_VOLUME_ID  0       /* 0:数字卷ID */
#define _MULTI_PARTITION 0      /* 0:单分区 */
#define _MIN_SS         512     /* 最小扇区大小 */
#define _MAX_SS         512     /* 最大扇区大小 */
#define _USE_TRIM       0       /* 0:禁用TRIM */
#define _FS_NOFSINFO    0

/*---------------------------------------------------------------------------/
/ 系统配置
/---------------------------------------------------------------------------*/

#define _FS_NORTC       1       /* 1:禁用RTC(使用固定时间戳) */
#define _NORTC_MON      1
#define _NORTC_MDAY     1
#define _NORTC_YEAR     2025

#define _FS_LOCK        0       /* 0:禁用文件锁 */
#define _FS_REENTRANT   0       /* 0:禁用重入 */
#define _FS_TIMEOUT     1000
#define _SYNC_t         void*

#define _WORD_ACCESS    0       /* 0:字节访问(兼容性好) */

#endif /* _FFCONF */
