
#include "sys_init.h"
#include "led.h"
#include "beep.h"
#include "fan.h"
#include "byj48.h"
#include "oled.h"
#include "dht11.h"
#include "jw01.h"
#include "dc01.h"
#include "body_hw.h"
#include "ds1302.h"
#include "key.h"
#include "sdcard.h"

SysMode_t g_sys_mode = SYS_MODE_AUTO;
SysStateEnum_t g_sys_state = SYS_STATE_NORMAL;
ComfortThreshold_t g_comfort_th;
AlarmThreshold_t g_alarm_th;

static void SYS_Hardware_Init(void)
{
    LED_Init();
    Beep_Init();
    Fan_Init();
    MOTOR_Init();

    OLED_Init();
    DHT11_Init();
    JW01_USART3_Init();
    DC01_USART2_Init();
    BODY_HW_Init();
    DS1302_Init();
    KEY_Init();
	

}

static void SYS_Param_Init(void)
{
    /* 舒适阈值用于自动联动，报警阈值用于蜂鸣器告警。 */

    g_comfort_th.temp_min = 15;
    g_comfort_th.temp_max = 28;
    g_comfort_th.humi_min = 40;
    g_comfort_th.humi_max = 60;
    g_comfort_th.co2_max  = 1000;
    g_comfort_th.pm25_max = 75;
	

    g_alarm_th.temp_min = 0;
    g_alarm_th.temp_max = 35;
    g_alarm_th.pm25_max = 150;
    g_alarm_th.co2_max  = 1200;
}

static void SYS_Display_Init(void)
{

    OLED_Clear();
    OLED_ShowString(30, 2, "Smart Cabin");
    OLED_ShowString(50, 4, "Starting...");
    OLED_Refresh();
    HAL_Delay(1000);
    OLED_Clear();
    

    OLED_Set_Brightness(255);
}

void SYS_Init_All(void)
{
    SYS_Hardware_Init();
    SYS_Param_Init();
    SYS_Display_Init();
}
