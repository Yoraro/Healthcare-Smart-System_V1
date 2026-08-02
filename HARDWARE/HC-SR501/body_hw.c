#include "body_hw.h"
void BODY_HW_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 1. 使能 GPIO 时钟 */
    BODY_HW_GPIO_CLK_ENABLE();

    /* 2. 配置引脚为输入，下拉 */
    GPIO_InitStruct.Pin = BODY_HW_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(BODY_HW_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  读取人体红外传感器数字信号
  * @retval 0：无检测到人体，1：检测到人体
  */
uint16_t BODY_HW_GetData(void)
{
    static uint8_t trigger_count = 0;      // 连续触发计数器
    static uint8_t stable_state = 0;       // 当前稳定状态：0=无人，1=有人

    uint8_t raw = HAL_GPIO_ReadPin(BODY_HW_GPIO_PORT, BODY_HW_GPIO_PIN); // 原始信号

    if (raw == 1) {
        // 有信号：计数增加，上限为5
        if (trigger_count < 2) {
            trigger_count++;
        }
        // 如果连续5次都为1，则确认“有人”
        if (trigger_count >= 2) {
            stable_state = 1;
        }
    } else {
        stable_state = 0;
		if (trigger_count < 2) {
            trigger_count++;
        }
        // 如果连续5次都为1，则确认“有人”
        if (trigger_count >= 2) {
            stable_state = 0;
        }
    }

    return stable_state; // 返回滤波后的稳定状态
}
