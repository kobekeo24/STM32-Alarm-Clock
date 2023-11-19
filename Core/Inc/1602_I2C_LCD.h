/*
 * 1602_I2C_LCD.h
 *
 *  Created on: Nov 19, 2023
 *      Author: 13372
 */

#ifndef INC_1602_I2C_LCD_H_
#define INC_1602_I2C_LCD_H_

#include <stdint.h>

#define LCD_I2C_ADDRESS				(0x27<<1)


void lcd_init (void);   // initialize lcd

void lcd_send_cmd (char cmd);  // send command to the lcd

void lcd_send_data (char data);  // send data to the lcd

void lcd_send_string (char *str);  // send string to the lcd

void lcd_put_cur(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);

void lcd_clear (void);

#endif /* INC_1602_I2C_LCD_H_ */
