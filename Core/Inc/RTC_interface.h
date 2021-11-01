/*
 * RTC_interface.h
 *
 *  Created on: Oct 31, 2021
 *      Author: 13372
 */

#ifndef INC_RTC_INTERFACE_H_
#define INC_RTC_INTERFACE_H_

#include <stdbool.h>

/* Definitions ---------------------------------------------------------------*/
#define RTC_ADDRESS				(0xd0)

#define TIME_SECONDS_REG		(0x00)

#define TIME_MINUTES_REG		(0x01)

#define TIME_HOURS_REG			(0x02)

#define TIME_DAY_REG			(0x03)

#define TIME_DATE_REG			(0x04)

#define TIME_MONTH_REG			(0x05)

#define TIME_YEAR_REG			(0x06)

#define ALARM1_SECONDS_REG		(0x07)

#define ALARM1_MINUTES_REG		(0x08)

#define ALARM1_HOURS_REG		(0x09)

#define ALARM1_DAY_DATE_REG		(0x0a)

#define ALARM2_MINUTES_REG		(0x0b)

#define ALARM2_HOURS_REG		(0x0c)

#define ALARM2_DAY_DATE_REG		(0x0d)

#define CONTROL_REG				(0x0e)

#define STATUS_REG				(0x0f)

#define CLEAR_CONTROL_ALARM1	(0xFE)

#define CLEAR_STATUS_ALARM1		(0xFE)

#define CLEAR_CONTROL_ALARM2	(0xFD)

#define CLEAR_STATUS_ALARM2		(0xFD)

#define ENABLE_CONTROL_ALARM1	(0x05)

#define ENABLE_CONTROL_ALARM2	(0x06)
/* Definitions ---------------------------------------------------------------*/

/* Time Structure -------------------------------------------------------------*/
typedef struct RTC_interface
{
	uint8_t seconds_reg;
	uint8_t minutes_reg;
	uint8_t hours_reg;
	uint8_t day_reg;
	uint8_t date_reg;
	uint8_t month_reg;
	uint8_t year_reg;
	uint8_t day_date_reg;

	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t day_date;

	bool b_alarmed_triggered;
	bool b_alarm_set;
} RTC_struct;
/* Time Structure -------------------------------------------------------------*/

char Ask_seconds[50];
char txData[50];
char rxData[50];

bool b_button_pressed;

bool b_seconds_entered, b_minutes_entered, b_hours_entered;

UART_HandleTypeDef huart2;

I2C_HandleTypeDef hi2c1;

void RTC_INIT_TIME(void);

void RTC_INIT_ALARMS(void);

void RTC_Write(uint8_t reg, uint8_t data);

void RTC_Read(uint8_t* p_reg, uint8_t *p_data);

void RTC_Enable_Alarm_IT(uint8_t alarm);

void RTC_Clear_Alarm_IT(uint8_t alarm);

void RTC_Display_Time(void);

void RTC_Set_Seconds(uint8_t seconds);

void RTC_Set_Minutes(uint8_t minutes);

void RTC_Set_Hours(uint8_t hours);

void RTC_Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds);

void RTC_Day_Date(uint8_t day, uint8_t date);

void RTC_Set_Alarm(uint8_t alarm, uint8_t hours, uint8_t minutes, uint8_t seconds);

bool RTC_Alarm_triggered(uint8_t alarm);
#endif /* INC_RTC_INTERFACE_H_ */
