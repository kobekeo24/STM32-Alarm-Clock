/*
 * OLED.h
 *
 *  Created on: Nov 14, 2021
 *      Author: 13372
 */

#ifndef INC_OLED_H_
#define INC_OLED_H_

/* Definitions ---------------------------------------------------------------*/
#define OLED_ADDRESS_32H					(0x78)

/* Commands from datasheet */
#define OLED_SET_CONTRAST					(0x81)

#define OLED_SET_ENTIRE_DISPLAY_ON_RESUME	(0xA4)

#define OLED_SET_ENTIRE_DISPLAY_ON			(0xA5)

#define OLED_SET_NORMAL_DISPLAY				(0xA6)

#define OLED_SET_INVERSE_DISPLAY			(0xA7)

#define OLED_SET_DISPLAY_OFF				(0xAE)

#define OLED_SET_DISPLAY_ON					(0xAF)
/* Definitions ---------------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/
void OLED_BEGIN(void);

void OLED_Write_Command(uint8_t* command, uint8_t buffer_length);

void OLED_Draw(uint8_t display_bit);
#endif /* INC_OLED_H_ */
