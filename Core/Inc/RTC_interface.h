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
typedef struct RTC_interface_struct
{
	uint8_t seconds_reg;
	uint8_t minutes_reg;
	uint8_t hours_reg;
	uint8_t day_reg;
	uint8_t date_reg;
	uint8_t month_reg;
	uint8_t year_reg;
	uint8_t A1s_reg;
	uint8_t A1m_reg;
	uint8_t A1h_reg;
	uint8_t A1dd_reg;
	uint8_t A2m_reg;
	uint8_t A2h_reg;
	uint8_t A2dd_reg;
	uint8_t control_reg;
	uint8_t status_reg;

	bool b_alarm1_triggered;
	bool b_alarm2_triggered;

	bool b_alarm1_set;
	bool b_alarm2_set;
} RTC_interface;

typedef struct RTC_memory_struct
{
	uint8_t seconds_1s: 		4;
	uint8_t seconds_10s: 		3;
	uint8_t seconds_padding: 	1;

	uint8_t minutes_1s:			4;
	uint8_t minutes_10s: 		3;
	uint8_t minutes_padding: 	1;

	uint8_t hours_1s: 			4;
	uint8_t hours_10s: 			1;
	uint8_t hours_20s_AMPM: 	1;
	uint8_t hours_12_24: 		1;
	uint8_t hours_padding: 		1;

	uint8_t day: 				3;
	uint8_t days_padding: 		5;

	uint8_t date_1s: 			4;
	uint8_t date_10s: 			2;
	uint8_t date_padding: 		2;

	uint8_t month_1s: 			4;
	uint8_t month_10: 			1;
	uint8_t month_padding: 		2;
	uint8_t century: 			1;

	uint8_t year_1s: 			4;
	uint8_t years_10s: 			4;

	uint8_t A1_seconds_1s: 		4;
	uint8_t A1_seconds_10s: 	3;
	uint8_t A1M1: 				1;

	uint8_t A1_minutes_1s: 		4;
	uint8_t A1_minutes_10s: 	3;
	uint8_t A1M2: 				1;

	uint8_t A1_hours_1s: 		4;
	uint8_t A1_hours_10s: 		1;
	uint8_t A1_hours_20s_AMPM: 	1;
	uint8_t A1_hours_12_24: 	1;
	uint8_t A1M3: 				1;

	uint8_t A1_day_date: 		4;
	uint8_t A1_date_10s: 		2;
	uint8_t A1_day_date_sel: 	1;
	uint8_t A1M4: 				1;

	uint8_t A2_minutes_1s: 		4;
	uint8_t A2_minutes_10s: 	3;
	uint8_t A2M2: 				1;

	uint8_t A2_hours_1s:	 	4;
	uint8_t A2_hours_10s: 		1;
	uint8_t A2_hours_20s_AMPM: 	1;
	uint8_t A2_hours_12_24: 	1;
	uint8_t A2M3: 				1;

	uint8_t A2_day_date: 		4;
	uint8_t A2_date_10s: 		2;
	uint8_t A2_day_date_sel: 	1;
	uint8_t A2M4: 				1;

	uint8_t control_A1IE: 		1;
	uint8_t control_A2IE: 		1;
	uint8_t control_INTCN: 		1;
	uint8_t control_NA: 		2;
	uint8_t control_CONV: 		1;
	uint8_t control_BBSQW: 		1;
	uint8_t control_EOSC: 		1;

	uint8_t status_A1F: 		1;
	uint8_t status_A2F: 		1;
	uint8_t status_BSY: 		1;
	uint8_t status_EN32KHZ: 	1;
	uint8_t status_padding: 	3;
	uint8_t status_OSF: 		1;
} RTC_memory;

typedef union RTC_mem_data_u
{
	RTC_memory mem;
	uint8_t byte;
}RTC_mem_union;
/* Time Structure -------------------------------------------------------------*/

char txData[50];
char rxData[50];
char g_user_buffer[50];

bool b_button_pressed;

bool b_message_received;

UART_HandleTypeDef huart2;

I2C_HandleTypeDef hi2c1;

void RTC_INIT_TIME(void);

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

bool RTC_is_Alarm_triggered(void);

void RTC_User_Set_Time(bool b_set_alarm);
#endif /* INC_RTC_INTERFACE_H_ */
