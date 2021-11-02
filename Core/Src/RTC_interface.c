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
/* Includes ------------------------------------------------------------------*/


/* variables -----------------------------------------------------------------*/
static uint8_t last_second, last_minute, last_hour;

RTC_struct RTC_Time, RTC_Alarm1, RTC_Alarm2;

static uint8_t control_data, status_data;
static uint8_t control_reg = CONTROL_REG;
static uint8_t status_reg = STATUS_REG;

char Ask_seconds[50] = "Enter seconds: \n";
char Ask_minutes[50] = "Enter minutes: \n";
char Ask_hours[50]   = "Enter hours: \n";

/* variables -----------------------------------------------------------------*/

/* funtions -----------------------------------------------------------------*/
void RTC_INIT_TIME(void)
{
	RTC_Time.seconds_reg = TIME_SECONDS_REG;
	RTC_Time.minutes_reg = TIME_MINUTES_REG;
	RTC_Time.hours_reg 	 = TIME_HOURS_REG;
	RTC_Time.day_reg 	 = TIME_DAY_REG;
	RTC_Time.date_reg 	 = TIME_DATE_REG;
	RTC_Time.month_reg 	 = TIME_MONTH_REG;
	RTC_Time.year_reg 	 = TIME_YEAR_REG;
}

void RTC_INIT_ALARMS(void)
{
	RTC_Alarm1.seconds_reg 	 = ALARM1_SECONDS_REG;
	RTC_Alarm1.minutes_reg 	 = ALARM1_MINUTES_REG;
	RTC_Alarm1.hours_reg 	 = ALARM1_HOURS_REG;
	RTC_Alarm1.day_date_reg  = ALARM1_DAY_DATE_REG;

	RTC_Alarm2.minutes_reg 	 = ALARM2_MINUTES_REG;
	RTC_Alarm2.hours_reg 	 = ALARM2_HOURS_REG;
	RTC_Alarm2.day_date_reg  = ALARM2_DAY_DATE_REG;
}

void RTC_Write(uint8_t reg, uint8_t data)
{
	  static uint8_t payload[2];
	  payload[0] = reg;
	  payload[1] = data;
	  HAL_I2C_Master_Transmit(&hi2c1,RTC_ADDRESS,payload,2,10);
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
	RTC_Read(&control_reg, &control_data);
	//Enable Alarm 1 interrupt
	if(alarm == 1)
	{
		control_data |=	 ENABLE_CONTROL_ALARM1;
		RTC_Alarm1.b_alarm_set = true;
	}
	//Enable Alarm 2 interrupt
	else
	{
		control_data |=	 ENABLE_CONTROL_ALARM2;
		RTC_Alarm2.b_alarm_set = true;
	}

	RTC_Write(control_reg, control_data);
}

void RTC_Clear_Alarm_IT(uint8_t alarm)
{
	RTC_Read(&status_reg, &status_data);
	RTC_Read(&control_reg, &control_data);

	//Clear Alarm 1 interrupt
	if(alarm == 1)
	{
		status_data &=	 CLEAR_STATUS_ALARM1;
		control_data &=	 CLEAR_CONTROL_ALARM1;
		RTC_Alarm1.b_alarmed_triggered = false;

		RTC_Alarm1.b_alarm_set = false;
	}
	//Clear Alarm 2 interrupt
	else
	{
		status_data &=	 CLEAR_STATUS_ALARM2;
		control_data &=	 CLEAR_CONTROL_ALARM2;
		RTC_Alarm2.b_alarmed_triggered = false;

		RTC_Alarm2.b_alarm_set = false;
	}

	RTC_Write(control_reg, control_data);
	RTC_Write(status_reg, status_data);
	HAL_Delay(500);
}

void RTC_Display_Time(void)
{
	RTC_Read(&RTC_Time.seconds_reg, &RTC_Time.seconds);
	RTC_Read(&RTC_Time.minutes_reg, &RTC_Time.minutes);
	RTC_Read(&RTC_Time.hours_reg, &RTC_Time.hours);
	RTC_Read(&RTC_Time.day_reg, &RTC_Time.day);

	if(last_second != RTC_Time.seconds || last_minute != RTC_Time.minutes || last_hour != RTC_Time.hours)
	{
		sprintf(txData, "%x:%x:%x\n",RTC_Time.hours, RTC_Time.minutes, RTC_Time.seconds);
		HAL_UART_Transmit(&huart2,(uint8_t*) txData,strlen(txData), 10);
	}
	last_second = RTC_Time.seconds;
	last_minute = RTC_Time.minutes;
	last_hour	= RTC_Time.hours;
}

void RTC_Set_Seconds(uint8_t seconds)
{
	RTC_Time.seconds = seconds;
	RTC_Write(RTC_Time.seconds_reg, RTC_Time.seconds);
}

void RTC_Set_Minutes(uint8_t minutes)
{
	RTC_Time.minutes = minutes;
	RTC_Write(RTC_Time.minutes_reg, RTC_Time.minutes);
}

void RTC_Set_Hours(uint8_t hours)
{
	RTC_Time.hours   = hours;
	RTC_Write(RTC_Time.hours_reg, RTC_Time.hours);
}

void RTC_Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	RTC_Time.seconds = seconds;
	RTC_Time.minutes = minutes;
	RTC_Time.hours   = hours;

	RTC_Write(RTC_Time.seconds_reg, RTC_Time.seconds);
	RTC_Write(RTC_Time.minutes_reg, RTC_Time.minutes);
	RTC_Write(RTC_Time.hours_reg, RTC_Time.hours);
}

void RTC_Day_Date(uint8_t day, uint8_t date)
{
	RTC_Time.day = day;
	RTC_Time.date = date;

	RTC_Write(RTC_Time.day_reg, RTC_Time.day);
	RTC_Write(RTC_Time.date_reg, RTC_Time.date);
}

void RTC_Set_Alarm(uint8_t alarm, uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	if(alarm == 0x01)
	{
		RTC_Alarm1.seconds = seconds;
		RTC_Alarm1.minutes = minutes;
		RTC_Alarm1.hours   = hours;
		RTC_Alarm1.day_date = 0x40 + RTC_Time.day;

		RTC_Write(RTC_Alarm1.seconds_reg, RTC_Alarm1.seconds);
		RTC_Write(RTC_Alarm1.minutes_reg, RTC_Alarm1.minutes);
		RTC_Write(RTC_Alarm1.hours_reg, RTC_Alarm1.hours);
		RTC_Write(RTC_Alarm1.day_date_reg, RTC_Alarm1.day_date);
	}

	else if(alarm == 0x02)
	{
		RTC_Alarm2.minutes = minutes;
		RTC_Alarm2.hours   = hours;
		RTC_Alarm2.day_date = 0x40 + RTC_Time.day;

		RTC_Write(RTC_Alarm2.minutes_reg, RTC_Alarm2.minutes);
		RTC_Write(RTC_Alarm2.hours_reg, RTC_Alarm2.hours);
		RTC_Write(RTC_Alarm2.day_date_reg, RTC_Alarm2.day_date);
	}
	RTC_Clear_Alarm_IT(alarm);
	RTC_Enable_Alarm_IT(alarm);
}

bool RTC_Alarm_triggered(uint8_t alarm)
{
	if(alarm == 0x01)
	{
		return RTC_Alarm1.b_alarmed_triggered;
	}
	else if(alarm == 0x02)
	{
		return RTC_Alarm2.b_alarmed_triggered;
	}
}

void RTC_User_Set_Time(bool b_set_alarm)
{
	static uint8_t user_seconds;
	static uint8_t user_minutes;
	static uint8_t user_hours;

	HAL_UART_Transmit(&huart2,(uint8_t*) Ask_seconds,strlen(Ask_seconds), 50);
	HAL_Delay(200);
	while(!b_message_received);
	user_seconds = rxData;

	HAL_UART_Transmit(&huart2,(uint8_t*) Ask_minutes,strlen(Ask_minutes), 50);
	b_message_received = false;
	HAL_Delay(200);
	while(!b_message_received);
	user_minutes = rxData;

	HAL_UART_Transmit(&huart2,(uint8_t*) Ask_minutes,strlen(Ask_minutes), 50);
	b_message_received = false;
	HAL_Delay(200);
	while(!b_message_received);
	user_hours = rxData;

	b_message_received = false;
	if(b_set_alarm)
	{
		RTC_Set_Alarm(0x01, user_hours, user_minutes, user_seconds);

		sprintf(txData,"Alarm Set: %d:%d:d",user_hours,user_minutes,user_seconds);
		HAL_UART_Transmit(&huart2,(uint8_t*) txData,strlen(txData), 50);
	}

	else
	{
		RTC_Set_Time(user_hours, user_minutes, user_seconds);
	}
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
	  if(RTC_Alarm1.b_alarm_set)
	  {
		  RTC_Alarm1.b_alarmed_triggered = true;
	  }
	  else if(RTC_Alarm2.b_alarm_set)
	  {
		  RTC_Alarm2.b_alarmed_triggered = true;
	  }
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

  HAL_UART_Transmit(&huart2,(uint8_t*) rxData,strlen(rxData), 50);

  b_message_received = true;
}
