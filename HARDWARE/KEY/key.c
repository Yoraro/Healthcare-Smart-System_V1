#include "key.h"

/* ================== GPIO 定义 ================== */
#define KEY_PORT    GPIOA
#define KEY1_PIN    GPIO_PIN_4
#define KEY2_PIN    GPIO_PIN_5
#define KEY3_PIN    GPIO_PIN_6
#define KEY4_PIN    GPIO_PIN_7

/* ================== 参数（基于 10ms） ================== */
#define KEY_DEBOUNCE_TICKS   2       // 2 × 10ms = 20ms
#define KEY_LONG_TICKS      100     // 100 × 10ms = 1s

/* ================== 事件变量 ================== */
volatile KeyEvent_t KEY1_Event = KEY_EVENT_NONE;
volatile KeyEvent_t KEY2_Event = KEY_EVENT_NONE;
volatile KeyEvent_t KEY3_Event = KEY_EVENT_NONE;
volatile KeyEvent_t KEY4_Event = KEY_EVENT_NONE;

/* ================== 内部状态 ================== */
static uint8_t  key_state[4]       = {0};   // 0: 松开 1: 按下
static uint8_t  key_debounce[4]    = {0};
static uint16_t key_press_ticks[4] = {0};

/* ================== 初始化 ================== */
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin  = KEY1_PIN | KEY2_PIN | KEY3_PIN | KEY4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;

    HAL_GPIO_Init(KEY_PORT, &GPIO_InitStruct);
}

/* ================== 10ms 扫描函数（TIM4 中断调用） ================== */
void KEY_Scan_10ms(void)
{
    uint8_t read[4];

    read[0] = (HAL_GPIO_ReadPin(KEY_PORT, KEY1_PIN) == GPIO_PIN_RESET);
    read[1] = (HAL_GPIO_ReadPin(KEY_PORT, KEY2_PIN) == GPIO_PIN_RESET);
    read[2] = (HAL_GPIO_ReadPin(KEY_PORT, KEY3_PIN) == GPIO_PIN_RESET);
    read[3] = (HAL_GPIO_ReadPin(KEY_PORT, KEY4_PIN) == GPIO_PIN_RESET);

    for(uint8_t i = 0; i < 4; i++)
    {
        /* ---------- 消抖 ---------- */
        if(read[i] != key_state[i])
        {
            if(++key_debounce[i] >= KEY_DEBOUNCE_TICKS)
            {
                key_state[i] = read[i];
                key_debounce[i] = 0;

                /* 刚按下 */
                if(key_state[i])
                {
                    key_press_ticks[i] = 0;
                }
                /* 刚松开 */
                else
                {
                    if(key_press_ticks[i] > 0 &&
                       key_press_ticks[i] < KEY_LONG_TICKS)
                    {
                        switch(i)
                        {
                            case 0: KEY1_Event = KEY_EVENT_SHORT; break;
                            case 1: KEY2_Event = KEY_EVENT_SHORT; break;
                            case 2: KEY3_Event = KEY_EVENT_SHORT; break;
                            case 3: KEY4_Event = KEY_EVENT_SHORT; break;
                        }
                    }
                    key_press_ticks[i] = 0;
                }
            }
        }
        else
        {
            key_debounce[i] = 0;
        }

        /* ---------- 长按计时 ---------- */
        if(key_state[i])
        {
            if(key_press_ticks[i] < KEY_LONG_TICKS)
            {
                key_press_ticks[i]++;
                if(key_press_ticks[i] == KEY_LONG_TICKS)
                {
                    switch(i)
                    {
                        case 0: KEY1_Event = KEY_EVENT_LONG; break;
                        case 1: KEY2_Event = KEY_EVENT_LONG; break;
                        case 2: KEY3_Event = KEY_EVENT_LONG; break;
                        case 3: KEY4_Event = KEY_EVENT_LONG; break;
                    }
                }
            }
        }
    }
}

