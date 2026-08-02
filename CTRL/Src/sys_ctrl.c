
#include "sys_ctrl.h"
#include "dht11.h"
#include "jw01.h"
#include "dc01.h"
#include "body_hw.h"
#include "fan.h"
#include "beep.h"
#include "led.h"        
#include "byj48.h"      
#include "oled.h"
#include "key.h"
#include "ds1302.h"
#include <string.h>
#include "app_sdcard.h" 

/* 周期任务使用HAL毫秒时基，人体无人计时使用秒。 */
#define SENSOR_UPDATE_INTERVAL_MS  5000U
#define LOG_UPDATE_INTERVAL_MS    10000U
#define ECO_NO_HUMAN_SECONDS       7200U

#define CURTAIN_ANGLE_FULL 180
#define CURTAIN_MOTOR_NUM 0
#define CURTAIN_DELAY 5

typedef enum {
    MOTOR_MODE_8_STEP = 0,
    MOTOR_MODE_4_STEP_SINGLE,
    MOTOR_MODE_4_STEP_DUAL
} MotorMode;

typedef enum {
    MOTOR_DIR_CW = 0,
    MOTOR_DIR_CCW = 1
} MotorDirection;

SensorData_t g_sensor_data = {0};
UIPage_t g_ui_page = UI_TIME_PAGE;
ThresholdEdit_t g_edit_item = EDIT_NONE;
DS1302_Time_t g_current_time;
SysMode_t g_last_mode_before_eco = SYS_MODE_AUTO;

uint8_t oled_mode_selected = 0;   
uint8_t oled_setting_selected = 0;
static uint8_t oled_alarm_selected = 0;

static uint32_t last_update_tick = 0;
static uint8_t  oled_need_refresh = 1;
static uint8_t  last_second = 0xFF;
static uint32_t human_check_timer = 0;
static uint8_t  key_processed = 0;
static uint8_t  oled_brightness = 100;
static uint8_t  auto_to_eco_flag = 0;
static uint8_t  sensor_data_valid = 0;

static void SYS_Update_Sensors(void);
static void SYS_Process_Keys(void);
static void SYS_Update_OLED(void);
static void SYS_Check_Alarms(void);
static void SYS_Manual_Control(void);
static void SYS_Update_RTC_Time(void);
static void SYS_Sync_Device_States(void);

static void SYS_Update_LogFileName(void);
static void SYS_Write_Log(void);
	
static uint32_t last_log_tick = 0;
static char log_filename[32];
uint8_t sd_initialized = 0;

void SYS_CTRL_Init(void)
{
    memset(&g_sensor_data, 0, sizeof(SensorData_t));
    memset(&g_current_time, 0, sizeof(DS1302_Time_t));
    

    g_sensor_data.fan_on = 0;
    g_sensor_data.led_on = 0;

    g_sensor_data.curtain_open = 0;
    g_sensor_data.curtain_current = 0;
    g_sensor_data.curtain_moving = 0;

    g_sensor_data.alarm_active = 0;
		

    SYS_Sync_Device_States();
     
    g_sensor_data.mode_changed = 1;
    last_update_tick = HAL_GetTick();
    human_check_timer = 0;
    key_processed = 0;
    oled_brightness = 100;
	oled_mode_selected = 0;
	

    if(SD_App_Init() == SD_APP_OK)
    {
        sd_initialized = 1;
        SYS_Update_LogFileName();

        char readbuf[2];

        if(SD_App_ReadFile(log_filename, readbuf, sizeof(readbuf)) != SD_APP_OK)
        {
            char header[256];
            snprintf(header, sizeof(header),
                     "Time,temp,humi,co2,pm25,air_score,"
                     "human_detected,activity_count,last_human_tick_ms,"
                     "fan_on,led_on,curtain_open,curtain_current,curtain_moving,"
                     "alarm_active,eco_manual_set,eco_enter_time,mode_changed\r\n");
            SD_App_WriteFile(log_filename, header);
        }
    }
}

void SYS_CTRL_Task(void)
{
    uint32_t current_tick = HAL_GetTick();
    

    if (current_tick - last_update_tick >= 1000)
    {
        last_update_tick = current_tick;
        SYS_Update_RTC_Time();

        if (!g_sensor_data.human_detected)
        {
            human_check_timer++;
        }
        else
        {
            human_check_timer = 0;
        }
    }
    

    static uint32_t last_sensor_update = 0;
    if (current_tick - last_sensor_update >= SENSOR_UPDATE_INTERVAL_MS)
    {
        last_sensor_update = current_tick;
        SYS_Update_Sensors();
    }
    

    SYS_Check_Human_Mode();
    

    SYS_Process_Keys();
    

    switch (g_sys_mode)
    {
        case SYS_MODE_MANUAL:
            SYS_Manual_Control();
            break;
            
        case SYS_MODE_AUTO:
            SYS_Auto_Control();
            break;
            
        case SYS_MODE_ECO:
            SYS_Eco_Control();
            break;
            
        default:
            break;
    }
    

    SYS_Check_Alarms();
	

    SYS_Update_OLED();
	
	

    if (sd_initialized && (current_tick - last_log_tick >= LOG_UPDATE_INTERVAL_MS))
    {
        last_log_tick = current_tick;
        SYS_Write_Log();
    }
}

static void SYS_Update_Sensors(void)
{
    uint8_t temp;
    uint8_t humi;

    /* 读取失败时保留上一次有效温湿度，防止默认零值导致误报警。 */

    if (DHT11_Read_Data(&temp, &humi) == 0)
    {
        g_sensor_data.temp = temp;
        g_sensor_data.humi = humi;
        sensor_data_valid = 1;
    }
    

    g_sensor_data.co2 = JW01_Get_CO2();
    

    g_sensor_data.pm25 = DC01_PM25_Value;
    

    uint8_t current_human = BODY_HW_GetData();
    

    if (current_human && !g_sensor_data.human_detected)
    {
        g_sensor_data.activity_count++;
        g_sensor_data.last_human_tick = HAL_GetTick();
    }
    
    g_sensor_data.human_detected = current_human;
    

    g_sensor_data.air_score = Calculate_Air_Quality_Score(
        g_sensor_data.temp,
        g_sensor_data.humi,
        g_sensor_data.co2,
        g_sensor_data.pm25
    );
    

    if (g_ui_page == UI_SENSOR_PAGE) {
        oled_need_refresh = 1;
    }
}

void SYS_Check_Human_Mode(void)
{
    static uint32_t last_check_tick = 0;
    uint32_t current_tick = HAL_GetTick();
    

    if (current_tick - last_check_tick < 1000) return;
    last_check_tick = current_tick;
    

    if (g_sensor_data.eco_manual_set) {
        return;
    }
    

    if (g_sys_mode == SYS_MODE_MANUAL) {
        return;
    }
    

    /* 连续无人两小时自动节能，重新检测到活动后恢复先前模式。 */
    if (!g_sensor_data.human_detected) {
        if (human_check_timer >= ECO_NO_HUMAN_SECONDS && g_sys_mode != SYS_MODE_ECO) {

            g_last_mode_before_eco = g_sys_mode;
            g_sys_mode = SYS_MODE_ECO;
            g_sensor_data.eco_enter_time = human_check_timer;
            auto_to_eco_flag = 1;
            SYS_Eco_Enter();
        }
    } else {

        if (g_sys_mode == SYS_MODE_ECO && auto_to_eco_flag) {
            g_sys_mode = g_last_mode_before_eco;
            auto_to_eco_flag = 0;
            SYS_Eco_Exit();
        }
    }
}

void SYS_Eco_Enter(void)
{

    if (g_sensor_data.fan_on)
    {
        Fan_Off();
        g_sensor_data.fan_on = 0;
    }
    

    if (g_sensor_data.led_on)
    {
        LED_Off();
        g_sensor_data.led_on = 0;
    }
    

    if (g_sensor_data.curtain_open) {
        g_sensor_data.curtain_open = 0;
        
        if (g_sensor_data.curtain_current) {
            MOTOR_Direction_Angle(MOTOR_DIR_CCW, CURTAIN_MOTOR_NUM, CURTAIN_ANGLE_FULL, CURTAIN_DELAY);
            g_sensor_data.curtain_current = 0;
        }
    }
    

    oled_brightness = 30;
    OLED_Set_Brightness_Percent(oled_brightness);
    

    g_sensor_data.mode_changed = 1;
    oled_need_refresh = 1;
}

void SYS_Eco_Exit(void)
{

    oled_brightness = 100;
    OLED_Set_Brightness_Percent(oled_brightness);
    

    SYS_Sync_Device_States();
    

    g_sensor_data.mode_changed = 1;
    oled_need_refresh = 1;
}

void SYS_Eco_Control(void)
{

}

static void SYS_Sync_Device_States(void)
{

    if (g_sensor_data.fan_on) {
        Fan_On();
    } else {
        Fan_Off();
    }
    

    if (g_sensor_data.led_on) {
        LED_On();
    } else {
        LED_Off();
    }
    

    if (!g_sensor_data.curtain_moving) {
        if (g_sensor_data.curtain_open && !g_sensor_data.curtain_current) {

            MOTOR_Direction_Angle(MOTOR_DIR_CW, CURTAIN_MOTOR_NUM, CURTAIN_ANGLE_FULL, CURTAIN_DELAY);
            g_sensor_data.curtain_current = 1;
        } 
        else if (!g_sensor_data.curtain_open && g_sensor_data.curtain_current) {

            MOTOR_Direction_Angle(MOTOR_DIR_CCW, CURTAIN_MOTOR_NUM, CURTAIN_ANGLE_FULL, CURTAIN_DELAY);
            g_sensor_data.curtain_current = 0;
        }

    }
}

void SYS_Auto_Control(void)
{

    if (g_sys_mode != SYS_MODE_AUTO) return;

    uint8_t need_fan = 0;
    

    if (g_sensor_data.co2  > g_comfort_th.co2_max)   need_fan = 1;
    if (g_sensor_data.temp > g_comfort_th.temp_max)  need_fan = 1;
    if (g_sensor_data.humi > g_comfort_th.humi_max)  need_fan = 1;

    if (need_fan && !g_sensor_data.fan_on)
    {
        Fan_On();
        g_sensor_data.fan_on = 1;
        oled_need_refresh = 1;
    }
    else if (!need_fan && g_sensor_data.fan_on)
    {
        Fan_Off();
        g_sensor_data.fan_on = 0;
        oled_need_refresh = 1;
    }
    

    if (g_current_time.hour >= 20 || g_current_time.hour < 6)
    {
        if (g_sensor_data.human_detected && !g_sensor_data.led_on) {
            LED_On();
            g_sensor_data.led_on = 1;
            oled_need_refresh = 1;
        } else if (!g_sensor_data.human_detected && g_sensor_data.led_on) {
            LED_Off();
            g_sensor_data.led_on = 0;
            oled_need_refresh = 1;
        }
        

        if (g_sensor_data.curtain_open) {
            g_sensor_data.curtain_open = 0;
            

            if (g_sensor_data.curtain_current) {
                MOTOR_Direction_Angle(MOTOR_DIR_CCW, CURTAIN_MOTOR_NUM, CURTAIN_ANGLE_FULL, CURTAIN_DELAY);
                g_sensor_data.curtain_current = 0;
            }
            oled_need_refresh = 1;
        }
    }
    else
    {

        if (!g_sensor_data.curtain_open) {
            g_sensor_data.curtain_open = 1;
            

            if (!g_sensor_data.curtain_current) {
                MOTOR_Direction_Angle(MOTOR_DIR_CW, CURTAIN_MOTOR_NUM, CURTAIN_ANGLE_FULL, CURTAIN_DELAY);
                g_sensor_data.curtain_current = 1;
            }
            oled_need_refresh = 1;
        }
        

        if (g_sensor_data.led_on) {
            LED_Off();
            g_sensor_data.led_on = 0;
            oled_need_refresh = 1;
        }
    }

}

static void SYS_Manual_Control(void)
{

    

    if (g_sensor_data.fan_on) Fan_On(); else Fan_Off();
    if (g_sensor_data.led_on) LED_On(); else LED_Off();
}

static void SYS_Check_Alarms(void)
{
    uint8_t alarm_triggered = 0;

    /* 未获得有效温度前，仅判断串口传感器，避免上电即误报低温。 */
    

    if (g_sensor_data.co2 > g_alarm_th.co2_max) alarm_triggered = 1;
    if (g_sensor_data.pm25 > g_alarm_th.pm25_max) alarm_triggered = 1;
    if (sensor_data_valid && g_sensor_data.temp > g_alarm_th.temp_max) alarm_triggered = 1;
    if (sensor_data_valid && g_sensor_data.temp < g_alarm_th.temp_min) alarm_triggered = 1;
    

    if (alarm_triggered && !g_sensor_data.alarm_active)
    {
        Beep_On();
        g_sensor_data.alarm_active = 1;
        oled_need_refresh = 1;
    }
    else if (!alarm_triggered && g_sensor_data.alarm_active)
    {
        Beep_Off();
        g_sensor_data.alarm_active = 0;
        oled_need_refresh = 1;
    }
    

    if (g_ui_page == UI_MODE_PAGE && oled_need_refresh) {

        SYS_Update_OLED();
    }
}

static void SYS_Process_Keys(void)
{

    if (key_processed)
    {
        key_processed = 0;
        return;
    }

    if (KEY1_Event == KEY_EVENT_SHORT)
    {

        uint8_t page = (uint8_t)g_ui_page;
        page = (page + 1) % 5;
        g_ui_page = (UIPage_t)page;
        oled_need_refresh = 1;

        KEY1_Event = KEY_EVENT_NONE;
        key_processed = 1;
    }
    else if (KEY1_Event == KEY_EVENT_LONG)
    {

        if (oled_brightness > 10) {
            oled_brightness -= 10;
            OLED_Set_Brightness_Percent(oled_brightness);
        }
        KEY1_Event = KEY_EVENT_NONE;
        key_processed = 1;
    }
	else if (KEY2_Event == KEY_EVENT_SHORT)
	{

		auto_to_eco_flag = 0;

		if (g_sys_mode == SYS_MODE_ECO && g_sensor_data.eco_manual_set) {
			g_sensor_data.eco_manual_set = 0;

		}

		uint8_t mode = (uint8_t)g_sys_mode;
		mode = (mode + 1) % 3;

		if (mode == SYS_MODE_ECO) {

			g_sensor_data.eco_manual_set = 1;
			g_sensor_data.eco_enter_time = human_check_timer;
			SYS_Eco_Enter();
		} 
		else if (g_sys_mode == SYS_MODE_ECO && mode != SYS_MODE_ECO) {

			SYS_Eco_Exit();
			

			if (mode == SYS_MODE_MANUAL) {
				oled_mode_selected = 0;
				SYS_Sync_Device_States();
			}
		}
		else if (mode == SYS_MODE_MANUAL) {

			oled_mode_selected = 0;
			SYS_Sync_Device_States();
		}

		g_sys_mode = (SysMode_t)mode;

		if (g_sys_mode == SYS_MODE_MANUAL) {
			human_check_timer = 0;
		}

		g_sensor_data.mode_changed = 1;
		oled_need_refresh = 1;

		KEY2_Event = KEY_EVENT_NONE;
		key_processed = 1;
	}
    else if (KEY2_Event == KEY_EVENT_LONG)
    {

        if (oled_brightness < 100) {
            oled_brightness += 10;
            OLED_Set_Brightness_Percent(oled_brightness);
        }
        KEY2_Event = KEY_EVENT_NONE;
        key_processed = 1;
    }

    if (g_ui_page == UI_MODE_PAGE)
    {

        if (KEY3_Event == KEY_EVENT_SHORT)
        {
            if (g_sys_mode == SYS_MODE_MANUAL) {
                oled_mode_selected++;
                if (oled_mode_selected > 2) oled_mode_selected = 0;
                oled_need_refresh = 1;
                KEY3_Event = KEY_EVENT_NONE;
                key_processed = 1;
            }
            return;
        }

        if (KEY4_Event == KEY_EVENT_SHORT)
        {

            if (g_sys_mode == SYS_MODE_MANUAL && oled_mode_selected <= 2)
            {

                switch(oled_mode_selected)
                {
                    case 0:
                        if (g_sensor_data.fan_on) {
                            Fan_Off();
                            g_sensor_data.fan_on = 0;
                        } else {
                            Fan_On();
                            g_sensor_data.fan_on = 1;
                        }
                        break;
                        
                    case 1:
                        if (g_sensor_data.led_on) {
                            LED_Off();
                            g_sensor_data.led_on = 0;
                        } else {
                            LED_On();
                            g_sensor_data.led_on = 1;
                        }
                        break;
                        
					case 2:
						if (g_sensor_data.curtain_open) {

							if (g_sensor_data.curtain_current) {
								MOTOR_Direction_Angle(MOTOR_DIR_CCW, CURTAIN_MOTOR_NUM, CURTAIN_ANGLE_FULL, CURTAIN_DELAY);
								g_sensor_data.curtain_current = 0;
							}
							g_sensor_data.curtain_open = 0;
						} else {

							if (!g_sensor_data.curtain_current) {
								MOTOR_Direction_Angle(MOTOR_DIR_CW, CURTAIN_MOTOR_NUM, CURTAIN_ANGLE_FULL, CURTAIN_DELAY);
								g_sensor_data.curtain_current = 1;
							}
							g_sensor_data.curtain_open = 1;
						}
						break;
						
                    default:

                        break;
                }
                oled_need_refresh = 1;
            }
            KEY4_Event = KEY_EVENT_NONE;
            key_processed = 1;
            return;
        }
    }

	if (g_ui_page == UI_SETTING_PAGE)
	{

		if (KEY3_Event == KEY_EVENT_SHORT)
		{
			oled_setting_selected++;
			if (oled_setting_selected > 5) oled_setting_selected = 0;
			oled_need_refresh = 1;
			KEY3_Event = KEY_EVENT_NONE;
			key_processed = 1;
			return;
		}

		if (KEY4_Event == KEY_EVENT_SHORT || KEY4_Event == KEY_EVENT_LONG)
		{
			int8_t step = (KEY4_Event == KEY_EVENT_SHORT) ? 1 : -1;

			switch (oled_setting_selected)
			{
				case 0:
					g_comfort_th.temp_max += step;
					if (g_comfort_th.temp_max > 35) g_comfort_th.temp_max = 35;
					if (g_comfort_th.temp_max < g_comfort_th.temp_min) 
						g_comfort_th.temp_max = g_comfort_th.temp_min;
					break;
				case 1:
					g_comfort_th.temp_min += step;
					if (g_comfort_th.temp_min > g_comfort_th.temp_max) 
						g_comfort_th.temp_min = g_comfort_th.temp_max;
					if (g_comfort_th.temp_min < 5) g_comfort_th.temp_min = 5;
					break;
				case 2:
					g_comfort_th.humi_max += step;
					if (g_comfort_th.humi_max > 100) g_comfort_th.humi_max = 100;
					if (g_comfort_th.humi_max < g_comfort_th.humi_min) 
						g_comfort_th.humi_max = g_comfort_th.humi_min;
					break;
				case 3:
					g_comfort_th.humi_min += step;
					if (g_comfort_th.humi_min > g_comfort_th.humi_max) 
						g_comfort_th.humi_min = g_comfort_th.humi_max;

					break;
				case 4:
					if (step > 0 && g_comfort_th.pm25_max <= 495) g_comfort_th.pm25_max += 5;
					else if (step < 0 && g_comfort_th.pm25_max >= 5) g_comfort_th.pm25_max -= 5;
					break;
				case 5:
					if (step > 0 && g_comfort_th.co2_max <= 4950) g_comfort_th.co2_max += 50;
					else if (step < 0 && g_comfort_th.co2_max >= 50) g_comfort_th.co2_max -= 50;
					break;
			}

			oled_need_refresh = 1;
			KEY4_Event = KEY_EVENT_NONE;
			key_processed = 1;
			return;
		}
	}

	if (g_ui_page == UI_THRESHOLD_PAGE)
	{

		if (KEY3_Event == KEY_EVENT_SHORT)
		{
			oled_alarm_selected++;
			if (oled_alarm_selected > 3) oled_alarm_selected = 0;
			oled_need_refresh = 1;
			KEY3_Event = KEY_EVENT_NONE;
			key_processed = 1;
			return;
		}

		if (KEY4_Event == KEY_EVENT_SHORT || KEY4_Event == KEY_EVENT_LONG)
		{
			int step = (KEY4_Event == KEY_EVENT_SHORT) ? 1 : -1;

			switch ((SettingItem_t)oled_alarm_selected)
			{
				case SET_TEMP_MAX:
					g_alarm_th.temp_max += step;
					if (g_alarm_th.temp_max > 60) g_alarm_th.temp_max = 60;
					if (g_alarm_th.temp_max < g_alarm_th.temp_min)
						g_alarm_th.temp_max = g_alarm_th.temp_min;
					break;

				case SET_TEMP_MIN:
					if (step > 0 && g_alarm_th.temp_min < g_alarm_th.temp_max) {
						g_alarm_th.temp_min++;
					} else if (step < 0 && g_alarm_th.temp_min > 0) {
						g_alarm_th.temp_min--;
					}
					if (g_alarm_th.temp_min > g_alarm_th.temp_max)
						g_alarm_th.temp_min = g_alarm_th.temp_max;
					break;

				case SET_PM25_MAX:
					if (step > 0 && g_alarm_th.pm25_max <= 995) {
						g_alarm_th.pm25_max += 5;
					} else if (step < 0 && g_alarm_th.pm25_max >= 5) {
						g_alarm_th.pm25_max -= 5;
					}
					break;

				case SET_CO2_MAX:
					if (step > 0 && g_alarm_th.co2_max <= 9950) {
						g_alarm_th.co2_max += 50;
					} else if (step < 0 && g_alarm_th.co2_max >= 50) {
						g_alarm_th.co2_max -= 50;
					}
					break;
			}

			oled_need_refresh = 1;
			KEY4_Event = KEY_EVENT_NONE;
			key_processed = 1;
			return;
		}
	}

}

static void SYS_Update_RTC_Time(void)
{
    static uint8_t last_second_check = 0xFF;
    DS1302_ReadTime(&g_current_time);
    

    if (g_current_time.second != last_second_check) {
        last_second_check = g_current_time.second;
        if (g_ui_page == UI_TIME_PAGE) {
            oled_need_refresh = 1;
        }
    }
}

static void SYS_Update_OLED(void)
{

    if (!oled_need_refresh && g_current_time.second == last_second)
    {
        return;
    }
    

    if (g_ui_page == UI_TIME_PAGE && g_current_time.second != last_second)
    {
        last_second = g_current_time.second;
        oled_need_refresh = 1;
    }
    
    if (!oled_need_refresh)
    {
        return;
    }
    
    oled_need_refresh = 0;
    

    switch (g_ui_page)
    {
        case UI_TIME_PAGE:
            OLED_Display_TimePage(&g_current_time, g_sys_mode, human_check_timer);
            break;
            
        case UI_SENSOR_PAGE:
            OLED_Display_SensorPage(
                g_sensor_data.temp,
                g_sensor_data.humi,
                g_sensor_data.co2,
                g_sensor_data.pm25,
                g_sensor_data.air_score,
                g_sys_mode
            );
            break;
            
		case UI_MODE_PAGE:
			OLED_Display_ModePage(
				g_sys_mode, 
				g_sensor_data.fan_on,
				g_sensor_data.led_on,
				g_sensor_data.curtain_open,
				g_sensor_data.alarm_active,
				oled_mode_selected,
				sd_initialized
			);
			break;
            
        case UI_SETTING_PAGE:
            OLED_Display_SettingPage(g_comfort_th, oled_setting_selected, g_sys_mode);
            break;
        case UI_THRESHOLD_PAGE:
			OLED_Display_AlarmThresholdPage(g_alarm_th,oled_alarm_selected,g_sys_mode);
			break;

        default:
            break;
    }
}

static void SYS_Update_LogFileName(void)
{
    DS1302_ReadTime(&g_current_time);

    snprintf(log_filename, sizeof(log_filename),
             "0:/%04d-%02d-%02d.csv",
             2000 + g_current_time.year,
             g_current_time.month,
             g_current_time.date);
}

static void SYS_Write_Log(void)
{
    if(!sd_initialized) return;

    char prev_filename[32];
    strcpy(prev_filename, log_filename);
    SYS_Update_LogFileName();
    if(strcmp(prev_filename, log_filename) != 0)
    {

        char header[256];
        snprintf(header, sizeof(header),
                 "Time,temp,humi,co2,pm25,air_score,"
                 "human_detected,activity_count,last_human_tick_ms,"
                 "fan_on,led_on,curtain_open,curtain_current,curtain_moving,"
                 "alarm_active,eco_manual_set,eco_enter_time,mode_changed\r\n");
        SD_App_WriteFile(log_filename, header);
    }

    char buf[256];

    /* 格式串与CSV表头严格保持18列一致。 */
	snprintf(buf, sizeof(buf),
         "%02d:%02d:%02d,%u,%u,%u,%u,%u,%u,%lu,"
         "%lu,%u,%u,%u,%u,%u,%u,%u,%lu,%u\r\n",
         g_current_time.hour, g_current_time.minute, g_current_time.second,
         g_sensor_data.temp, g_sensor_data.humi, g_sensor_data.co2,
         g_sensor_data.pm25, g_sensor_data.air_score,
         g_sensor_data.human_detected, (unsigned long)g_sensor_data.activity_count,
         (unsigned long)g_sensor_data.last_human_tick, g_sensor_data.fan_on,
         g_sensor_data.led_on, g_sensor_data.curtain_open,
         g_sensor_data.curtain_current, g_sensor_data.curtain_moving,
         g_sensor_data.alarm_active, g_sensor_data.eco_manual_set,
         (unsigned long)g_sensor_data.eco_enter_time,
         g_sensor_data.mode_changed);

    SD_App_AppendFile(log_filename, buf);
}

