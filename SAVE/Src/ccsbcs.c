/**
 * @file    ccsbcs.c
 * @brief   FatFs Unicode转换函数 - 简化版(仅支持ASCII)
 */

#include "ff.h"

#if _USE_LFN

/**
 * @brief  OEM代码转Unicode
 */
WCHAR ff_convert(WCHAR chr, UINT dir)
{
    if (chr < 0x80) {
        return chr;  /* ASCII直接返回 */
    }
    return 0;  /* 非ASCII返回0 */
}

/**
 * @brief  Unicode转大写
 */
WCHAR ff_wtoupper(WCHAR chr)
{
    if (chr >= 'a' && chr <= 'z') {
        return chr - 0x20;
    }
    return chr;
}

#endif /* _USE_LFN */
