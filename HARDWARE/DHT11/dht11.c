#include "dht11.h"
#include "delay.h"

/* 向DHT11发送起始信号。 */
void DHT11_Rst(void)	   
{                 
	DHT11_Mode(OUT);
	DHT11_Low;
	HAL_Delay(20);
	DHT11_High;
	delay_us(13);
}

uint8_t DHT11_Check(void) 	   
{   
	uint8_t retry = 0;
	DHT11_Mode(IN);
	

    while (DHT11_Read && retry < 100)
	{
		retry++;
		delay_us(1);
	}
	 
	if(retry >= 100) return 1;
	else retry = 0;
	

    while (!DHT11_Read && retry < 100)
	{
		retry++;
		delay_us(1);
	}
	if(retry >= 100) return 1;	    
	return 0;
}

uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry = 0;

	while(DHT11_Read && retry < 100)
	{
		retry++;
		delay_us(1);
	}
	retry = 0;

	while(!DHT11_Read && retry < 100)
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);
	
	if(DHT11_Read) return 1;
	else return 0;		   
}

uint8_t DHT11_Read_Byte(void)    
{        
	uint8_t i, dat;
	dat = 0;
	for (i = 0; i < 8; i++) 
	{
		dat <<= 1; 
		dat |= DHT11_Read_Bit();
	}						    
	return dat;
}

uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi)    
{        
	uint8_t buf[5];
	uint8_t i;

	if (temp == NULL || humi == NULL)
	{
		return 1;
	}

	DHT11_Rst();
	if(DHT11_Check() == 0)
	{
		for(i = 0; i < 5; i++)
		{
			buf[i] = DHT11_Read_Byte();
		}
		if((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
		{
			*humi = buf[0];
			*temp = buf[2];
			return 0;
		}
	}

	/* 无响应和校验失败都作为读取失败返回。 */
	return 1;
}

uint8_t DHT11_Init(void)
{	 

    
	DHT11_Rst();
	return DHT11_Check();
}

void DHT11_Mode(uint8_t mode)
{
    static GPIO_InitTypeDef GPIO_InitStruct;
    
    if(mode == OUT)
    {
        GPIO_InitStruct.Pin = DHT11_GPIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStruct);
    }
    else
    {
        GPIO_InitStruct.Pin = DHT11_GPIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStruct);
    }
}
