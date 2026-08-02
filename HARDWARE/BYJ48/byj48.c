#include "byj48.h"

/* 初始化步进电机引脚 */
void MOTOR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
                                                                  
    /* 使能GPIOB时钟 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    /* 配置步进电机控制引脚为输出模式 */
    GPIO_InitStruct.Pin = MOTOR_A_PIN | MOTOR_B_PIN | MOTOR_C_PIN | MOTOR_D_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;        // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;                // 不上拉不下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;       // 低速输出
    HAL_GPIO_Init(MOTOR_PORT, &GPIO_InitStruct);
    
    /* 初始化所有引脚为低电平 */
    HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_A_PIN | MOTOR_B_PIN | MOTOR_C_PIN | MOTOR_D_PIN, GPIO_PIN_RESET);
}

/* 4相单4拍模式 */
void MOTOR_Rhythm_4_1_4(uint8_t step, uint8_t dly)
{
    switch(step)
    {
        case 0 :
            break;
            
        case 1 :
            MOTOR_A_LOW; MOTOR_B_HIGH; MOTOR_C_HIGH; MOTOR_D_HIGH;    // A相导通
            break;
            
        case 2 :
            MOTOR_A_HIGH; MOTOR_B_LOW; MOTOR_C_HIGH; MOTOR_D_HIGH;    // B相导通
            break;
            
        case 3 :
            MOTOR_A_HIGH; MOTOR_B_HIGH; MOTOR_C_LOW; MOTOR_D_HIGH;    // C相导通
            break;
            
        case 4 :
            MOTOR_A_HIGH; MOTOR_B_HIGH; MOTOR_C_HIGH; MOTOR_D_LOW;    // D相导通
            break;        
    }
    HAL_Delay(dly);  // 使用HAL库延时
}

/* 4相双4拍模式 */
void MOTOR_Rhythm_4_2_4(uint8_t step, uint8_t dly)
{
    switch(step)
    {
        case 0 :
            break;
            
        case 1 :
            MOTOR_A_LOW; MOTOR_B_HIGH; MOTOR_C_HIGH; MOTOR_D_LOW;    // A、D相导通
            break;
            
        case 2 :
            MOTOR_A_LOW; MOTOR_B_LOW; MOTOR_C_HIGH; MOTOR_D_HIGH;    // B、C相导通
            break;
            
        case 3 :
            MOTOR_A_HIGH; MOTOR_B_LOW; MOTOR_C_LOW; MOTOR_D_HIGH;    // C、D相导通
            break;
            
        case 4 :
            MOTOR_A_HIGH; MOTOR_B_HIGH; MOTOR_C_LOW; MOTOR_D_LOW;    // D、A相导通
            break;        
    }
    HAL_Delay(dly);  // 使用HAL库延时
}

/* 4相单双8拍模式 */
void MOTOR_Rhythm_4_1_8(uint8_t step, uint8_t dly)
{
    switch(step)
    {
        case 0 :
            break;
            
        case 1 :
            MOTOR_A_LOW; MOTOR_B_HIGH; MOTOR_C_HIGH; MOTOR_D_HIGH;    // A相导通
            break;
            
        case 2 :
            MOTOR_A_LOW; MOTOR_B_LOW; MOTOR_C_HIGH; MOTOR_D_HIGH;    // A、B相导通
            break;
            
        case 3 :
            MOTOR_A_HIGH; MOTOR_B_LOW; MOTOR_C_HIGH; MOTOR_D_HIGH;    // B相导通
            break;
            
        case 4 :
            MOTOR_A_HIGH; MOTOR_B_LOW; MOTOR_C_LOW; MOTOR_D_HIGH;    // B、C相导通
            break;    
            
        case 5 :
            MOTOR_A_HIGH; MOTOR_B_HIGH; MOTOR_C_LOW; MOTOR_D_HIGH;    // C相导通
            break;
            
        case 6 :
            MOTOR_A_HIGH; MOTOR_B_HIGH; MOTOR_C_LOW; MOTOR_D_LOW;    // C、D相导通
            break;
            
        case 7 :
            MOTOR_A_HIGH; MOTOR_B_HIGH; MOTOR_C_HIGH; MOTOR_D_LOW;    // D相导通
            break;
            
        case 8 :
            MOTOR_A_LOW; MOTOR_B_HIGH; MOTOR_C_HIGH; MOTOR_D_LOW;    // D、A相导通
            break;            
    }
    HAL_Delay(dly);  // 使用HAL库延时
}

/* 控制电机方向 */
void MOTOR_Direction(uint8_t dir, uint8_t num, uint8_t dly)
{
    if(dir)  // 正转
    {
        switch(num)
        {
            case 0:
                for(uint8_t i = 1; i < 9; i++) {
                    MOTOR_Rhythm_4_1_8(i, dly);
                }
                break;
            case 1:
                for(uint8_t i = 1; i < 5; i++) {
                    MOTOR_Rhythm_4_1_4(i, dly);
                }
                break;
            case 2:
                for(uint8_t i = 1; i < 5; i++) {
                    MOTOR_Rhythm_4_2_4(i, dly);
                }
                break;
            default:
                break;
        }
    }
    else  // 反转
    {
        switch(num)
        {
            case 0:
                for(uint8_t i = 8; i > 0; i--) {
                    MOTOR_Rhythm_4_1_8(i, dly);
                }
                break;
            case 1:
                for(uint8_t i = 4; i > 0; i--) {
                    MOTOR_Rhythm_4_1_4(i, dly);
                }
                break;
            case 2:
                for(uint8_t i = 4; i > 0; i--) {
                    MOTOR_Rhythm_4_2_4(i, dly);
                }
                break;
            default:
                break;
        }    
    }
}

/* 控制电机旋转指定角度 */
void MOTOR_Direction_Angle(uint8_t dir, uint8_t num, uint16_t angle, uint8_t dly)
{
    // 28BYJ-48减速比为64:1，所以64步为360度，每步5.625度
    // 计算需要步数：angle / 5.625 = angle * 64 / 360 = angle * 16 / 90
    uint16_t steps = (uint16_t)((uint32_t)angle * 16 / 90);
    
    for(uint16_t i = 0; i < steps; i++)
    {
        MOTOR_Direction(dir, num, dly);    
    }
}

/* 停止电机 */
void MOTOR_STOP(void)
{
    HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_A_PIN | MOTOR_B_PIN | MOTOR_C_PIN | MOTOR_D_PIN, GPIO_PIN_RESET);
}

