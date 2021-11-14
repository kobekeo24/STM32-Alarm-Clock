/*
 * RTC_interface.c
 *
 *  Created on: Oct 31, 2021
 *      Author: 13372
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "RTC_interface.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/* Includes ------------------------------------------------------------------*/


/* variables -----------------------------------------------------------------*/
static uint8_t last_second, last_minute, last_hour;
static bool b_alarm_triggered;

static uint8_t g_user_buffer_index;

RTC_interface RTC_map;
RTC_mem_union RTC_mem;

char Ask_seconds[50] = "Enter seconds: \n";
char Ask_minutes[50] = "Enter minutes: \n";
char Ask_hours[50]   = "Enter hours: \n";


uint8_t HEX_LUT[] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
};

/* variables -----------------------------------------------------------------*/

/* funtions -----------------------------------------------------------------*/
void RTC_INIT_TIME(void)
{
	RTC_map.seconds_reg = TIME_SECONDS_REG;
	RTC_map.minutes_reg = TIME_MINUTES_REG;
	RTC_map.hours_reg   = TIME_HOURS_REG;
	RTC_map.day_reg     = TIME_DAY_REG;
	RTC_map.date_reg    = TIME_DATE_REG;
	RTC_map.month_reg 	= TIME_MONTH_REG;
	RTC_map.year_reg 	= TIME_YEAR_REG;
	RTC_map.A1s_reg		= ALARM1_SECONDS_REG;
	RTC_map.A1m_reg		= ALARM1_MINUTES_REG;
	RTC_map.A1h_reg		= ALARM1_HOURS_REG;
	RTC_map.A1dd_reg	= ALARM1_DAY_DATE_REG;
	RTC_map.A2m_reg		= ALARM2_MINUTES_REG;
	RTC_map.A2h_reg		= ALARM2_HOURS_REG;
	RTC_map.A2dd_reg	= ALARM2_DAY_DATE_REG;
	RTC_map.control_reg	= CONTROL_REG;
	RTC_map.status_reg	= STATUS_REG;

/*
	for(uint8_t i = 0; i < STATUS_REG + 1; i++)
	{
		RTC_Read(i, (&RTC_mem.byte) + i);
	}
*/
}

void RTC_Write(uint8_t reg, uint8_t data)
{
	  static uint8_t payload[2];
	  payload[0] = reg;
	  payload[1] = data;
	  HAL_I2C_Master_Transmit(&hi2c1,RTC_ADDRESS,payload,2,100);
	  HAL_Delay(30);
}

void RTC_Read(uint8_t* p_reg, uint8_t *p_data)
{
	//Ask slave to read data from register
	HAL_I2C_Master_Transmit(&hi2c1,RTC_ADDRESS+1,p_reg,1,10);
	//Store data
	HAL_I2C_Master_Receive(&hi2c1,RTC_ADDRESS,p_data,1,10);
	HAL_Delay(30);
}

void RTC_Enable_Alarm_IT(uint8_t alarm)
{
	RTC_Read(&RTC_map.control_reg, (&RTC_mem.byte) +  RTC_map.control_reg);
	//Enable Alarm 1 interrupt
	if(alarm == 1)
	{
		RTC_mem.mem.control_A1IE = 0x01;
		RTC_map.b_alarm1_set = true;
	}
	//Enable Alarm 2 interrupt
	else
	{
		RTC_mem.mem.control_A2IE = 0x01;
		RTC_map.b_alarm2_set = true;
	}



	RTC_Write(RTC_map.control_reg, (&RTC_mem.byte)[RTC_map.control_reg]);
}

void RTC_Clear_Alarm_IT(uint8_t alarm)
{
	RTC_Read(&RTC_map.status_reg, (&RTC_mem.byte) + RTC_map.status_reg);
	RTC_Read(&RTC_map.control_reg, (&RTC_mem.byte) +  RTC_map.control_reg);

	//Clear Alarm 1 interrupt
	if(alarm == 1)
	{
		RTC_mem.mem.status_A1F = 0;
		RTC_mem.mem.control_A1IE = 0;
		RTC_map.b_alarm1_triggered = false;

		RTC_map.b_alarm1_set = false;
	}
	//Clear Alarm 2 interrupt
	else
	{
		RTC_mem.mem.status_A2F = 0;
		RTC_mem.mem.control_A2IE = 0;
		RTC_map.b_alarm2_triggered = false;

		RTC_map.b_alarm2_set = false;
	}

	RTC_Write(RTC_map.control_reg, (&RTC_mem.byte)[RTC_map.control_reg]);
	RTC_Write(RTC_map.status_reg, (&RTC_mem.byte)[RTC_map.status_reg]);
	HAL_Delay(30);
}

void RTC_Display_Time(void)
{
	static uint8_t seconds, minutes, hours, day;
	RTC_Read(&RTC_map.seconds_reg, (&RTC_mem.byte) + RTC_map.seconds_reg);
	RTC_Read(&RTC_map.minutes_reg, (&RTC_mem.byte) + RTC_map.minutes_reg);
	RTC_Read(&RTC_map.hours_reg, (&RTC_mem.byte) + RTC_map.hours_reg);
	RTC_Read(&RTC_map.day_reg, (&RTC_mem.byte) + RTC_map.day_reg);

	seconds = (&RTC_mem.byte)[RTC_map.seconds_reg];
	minutes = (&RTC_mem.byte)[RTC_map.minutes_reg];
	//hours   = RTC_mem.mem.hours_10s << 4 + RTC_mem.mem.hours_1s;
	day     = (&RTC_mem.byte)[RTC_map.day_reg];

	if(last_second != seconds || last_minute != minutes || last_hour != hours)
	{
		sprintf(txData, "%x%x:%x:%x\n",RTC_mem.mem.hours_10s,RTC_mem.mem.hours_1s, minutes, seconds);
		HAL_UART_Transmit(&huart2,(uint8_t*) txData,strlen(txData), 10);
	}
	last_second = seconds;
	last_minute = minutes;
	last_hour	= hours;
}

void RTC_Set_Seconds(uint8_t seconds)
{
	(&RTC_mem.byte)[RTC_map.seconds_reg] = seconds;
	RTC_Write(RTC_map.seconds_reg, seconds);
}

void RTC_Set_Minutes(uint8_t minutes)
{
	(&RTC_mem.byte)[RTC_map.minutes_reg] = minutes;
	RTC_Write(RTC_map.minutes_reg, minutes);
}

void RTC_Set_Hours(uint8_t hours)
{
	(&RTC_mem.byte)[RTC_map.hours_reg] = hours;
	RTC_Write(RTC_map.hours_reg, hours);
}

void RTC_Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	(&RTC_mem.byte)[RTC_map.seconds_reg] = seconds;
	(&RTC_mem.byte)[RTC_map.minutes_reg] = minutes;
	(&RTC_mem.byte)[RTC_map.hours_reg]   = hours;

	RTC_Write(RTC_map.seconds_reg, seconds);
	RTC_Write(RTC_map.minutes_reg, minutes);
	RTC_Write(RTC_map.hours_reg, hours);
}

void RTC_Day_Date(uint8_t day, uint8_t date)
{
	(&RTC_mem.byte)[RTC_map.day_reg] = day;
	(&RTC_mem.byte)[RTC_map.date_reg] = date;

	RTC_Write(RTC_map.day_reg, day);
	RTC_Write(RTC_map.date_reg, date);
}

void RTC_Set_Alarm(uint8_t alarm, uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	if(alarm == 0x01)
	{
		(&RTC_mem.byte)[RTC_map.A1s_reg] = seconds;
		(&RTC_mem.byte)[RTC_map.A1m_reg] = minutes;
		(&RTC_mem.byte)[RTC_map.A1h_reg]   = hours;
		RTC_mem.mem.A1_day_date_sel = 0x01;
		RTC_mem.mem.A1_day_date =  RTC_mem.mem.day;


		RTC_Write(RTC_map.A1s_reg, seconds);
		RTC_Write(RTC_map.A1m_reg, minutes);
		RTC_Write(RTC_map.A1h_reg, hours);
		RTC_Read(&RTC_map.A1dd_reg, (&RTC_mem.byte)[RTC_map.A1dd_reg]);
		RTC_Write(RTC_map.A1dd_reg, (&RTC_mem.byte)[RTC_map.A1dd_reg]);
	}

	else if(alarm == 0x02)
	{
		(&RTC_mem.byte)[RTC_map.A2m_reg] = minutes;
		(&RTC_mem.byte)[RTC_map.A2h_reg]   = hours;
		RTC_mem.mem.A2_day_date_sel = 0x01;

		RTC_Write(RTC_map.A2m_reg, minutes);
		RTC_Write(RTC_map.A2h_reg, hours);
		RTC_Read(&RTC_map.A2dd_reg, (&RTC_mem.byte)+ RTC_map.A2dd_reg);
		RTC_Write(RTC_map.A2dd_reg, (&RTC_mem.byte) + RTC_map.A2dd_reg);
	}
	RTC_Clear_Alarm_IT(alarm);
	RTC_Enable_Alarm_IT(alarm);
}

bool RTC_Alarm_triggered(uint8_t alarm)
{
	if(RTC_is_Alarm_triggered())
	{
		if(alarm == 0x01)
		{
			return RTC_map.b_alarm1_triggered;
		}
		else if(alarm == 0x02)
		{
			return RTC_map.b_alarm2_triggered;
		}
	}
	else
	{
		return false;
	}
}

bool RTC_is_Alarm_triggered(void)
{
	RTC_Read(&RTC_map.status_reg, (&RTC_mem.byte) + RTC_map.status_reg);
	if(b_alarm_triggered)
	{
		if(RTC_mem.mem.status_A1F == 1)
		{
			RTC_map.b_alarm1_triggered = true;
		}
		else if(RTC_mem.mem.status_A2F == 1)
		{
			RTC_map.b_alarm2_triggered = true;
		}
	}
	return b_alarm_triggered;
}

void RTC_User_Set_Time(bool b_set_alarm)
{
	static uint8_t user_seconds;
	static uint8_t user_minutes;
	static uint8_t user_hours;

	g_user_buffer_index = 0;
	memset(g_user_buffer,0,sizeof(g_user_buffer));
	HAL_UART_Transmit(&huart2,(uint8_t*) Ask_seconds,strlen(Ask_seconds), 50);
	HAL_Delay(200);
	while(!b_message_received);
	HAL_Delay(50);
	user_seconds = HEX_LUT[ atoi(g_user_buffer) ];

	g_user_buffer_index = 0;
	memset(g_user_buffer,0,sizeof(g_user_buffer));
	HAL_UART_Transmit(&huart2,(uint8_t*) Ask_minutes,strlen(Ask_minutes), 50);
	b_message_received = false;
	HAL_Delay(200);
	while(!b_message_received);
	HAL_Delay(50);
	user_minutes = HEX_LUT[ atoi(g_user_buffer) ];

	g_user_buffer_index = 0;
	memset(g_user_buffer,0,sizeof(g_user_buffer));
	HAL_UART_Transmit(&huart2,(uint8_t*) Ask_hours,strlen(Ask_hours), 50);
	b_message_received = false;
	HAL_Delay(200);
	while(!b_message_received);
	HAL_Delay(100);
	user_hours = HEX_LUT[ atoi(g_user_buffer) ];


	b_message_received = false;
	if(b_set_alarm)
	{
		RTC_mem.mem.A1_hours_1s = user_hours;
		RTC_mem.mem.A1_hours_10s = user_hours >> 4;

		RTC_Set_Alarm(0x01, (&RTC_mem.byte)[RTC_map.A1h_reg], user_minutes, user_seconds);

		sprintf(txData,"Alarm Set: %d:%d:d",user_hours,user_minutes,user_seconds);
		HAL_UART_Transmit(&huart2,(uint8_t*) txData,strlen(txData), 50);
	}

	else
	{
		RTC_mem.mem.hours_1s = user_hours;
		RTC_mem.mem.hours_10s = user_hours >> 4;
		RTC_Set_Time( (&RTC_mem.byte)[RTC_map.hours_reg], user_minutes, user_seconds);
	}
	g_user_buffer_index = 0;
	memset(g_user_buffer,0,sizeof(g_user_buffer));
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
  if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1) == GPIO_PIN_RESET)
  {
	  b_alarm_triggered = true;
  }
  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
  {
	  b_button_pressed = true;
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */

	  g_user_buffer[g_user_buffer_index] = rxData[0];

	  g_user_buffer_index++;

	  b_message_received = true;

}
