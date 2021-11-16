#include "stm32f4xx_hal.h"
#include "OLED.h"
#include "RTC_interface.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void OLED_BEGIN(void)
{
	uint8_t I2CBuff[2];

	//display off
	I2CBuff[0] = 0x00;
	I2CBuff[1] = 0xAE;//0xAF; //0xAE; //off
	OLED_Write_Command(I2CBuff, sizeof(I2CBuff));
	//memory mode: horizontal addressing
	I2CBuff[1] = 0x20;	//memory mode
	OLED_Write_Command(I2CBuff, sizeof(I2CBuff));
	I2CBuff[1] = 0x00;	//horizontal addressing
	OLED_Write_Command(I2CBuff, sizeof(I2CBuff));
	//scan starting line
	I2CBuff[1] = 0x40; //0x40;	//start line 0-63: 11XXXXXX. So 40h-7Fh (basically negative, i.e. go up by)
	OLED_Write_Command(I2CBuff, sizeof(I2CBuff));
	I2CBuff[1] = 0x00; //0x00;	//start line: horizontal? the above is vertical.
	//charge pump on (2 commands)
	OLED_Write_Command(I2CBuff, sizeof(I2CBuff));
	I2CBuff[1] = 0x8D;
	OLED_Write_Command(I2CBuff, sizeof(I2CBuff));
	I2CBuff[1] = 0x14;
	OLED_Write_Command(I2CBuff, sizeof(I2CBuff));
	I2CBuff[1] = 0xA4;//0xA5; //all-on //0xA4;
	//display on
	OLED_Write_Command(I2CBuff, sizeof(I2CBuff));
	I2CBuff[1] = 0xAF;//0xAF; //0xAE; //off
	OLED_Write_Command(I2CBuff, sizeof(I2CBuff));

	I2CBuff[1] = 0xA4;//0xA5; //all-on //0xA4;
	OLED_Write_Command(I2CBuff, sizeof(I2CBuff));
}

void OLED_Write_Command(uint8_t* command, uint8_t buffer_length)
{
	HAL_I2C_Master_Transmit(&hi2c1,OLED_ADDRESS_32H,command,buffer_length,10);
	HAL_Delay(30);
}

void OLED_Draw(uint8_t display_bit)
{
	static uint8_t buffer[2];
	buffer[0] = 0x40;
	buffer[1] = display_bit;

	HAL_I2C_Master_Transmit(&hi2c1,OLED_ADDRESS_32H,buffer,2,10);
	HAL_Delay(30);
}
