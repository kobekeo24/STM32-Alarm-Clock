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

/* OLED dimmensions */
#define HEIGHT								(32u)

#define WIDTH								(128u)

/* Commands from datasheet */
#define OLED_SET_CONTRAST					(0x81)

#define OLED_SET_ENTIRE_DISPLAY_ON_RESUME	(0xA4)

#define OLED_SET_ENTIRE_DISPLAY_ON			(0xA5)

#define OLED_SET_NORMAL_DISPLAY				(0xA6)

#define OLED_SET_INVERSE_DISPLAY			(0xA7)

#define OLED_SET_DISPLAY_OFF				(0xAE)

#define OLED_SET_DISPLAY_ON					(0xAF)

/* Scrolling Command Table */
#define OLED_RIGHT_HORIZONTAL_SCROLL		(0x26)

#define OLED_LEFT_HORIZONTAL_SCROLL			(0x27)

#define OLED_RIGHT_VERTICAL_SCROLL			(0x29)

#define OLED_LEFT_VERTICAL_SCROLL			(0x2A)

#define OLED_DEACTIVATE_SCROLL				(0x2E)

#define OLED_ACTIVATE_SCROLL				(0x2F)

#define OLED_SET_VERTICAL_SCROLL			(0xA3)

/* Addressing Setting Command Table */
#define OLED_HORIZONTAL_ADDRESS_MODE		(0x20)

#define OLED_VERICAL_ADDRESS_MODE			(0x21)

#define OLED_PAGE_ADDRESS_MODE_RST			(0x22)

/* HW Configuration */
#define OLED_SET_SEGMENT_REMAP_RST			(0xA0)

#define OLED_SET_SEGMENT_REMAP_127			(0xA1)

#define OLED_SET_MUX_RATIO					(0xA8)

#define OLED_SET_COM_OUTPUT_RST				(0xC0)

#define OLED_SET_COM_OUTPUT_REMAP			(0xC8)

#define OLED_SET_DISPLAY_OFFSET				(0xD3)

#define OLED_SET_COM_PINS					(OxDA)

/* Timing & Driving Scheme Setting Command Table */
#define OLED_SET_DISPLAY_CLK				(OxD5)

#define OLED_SET_PRE_CHARGE_PERIOD			(0xD9)

#define OLED_SET_V_COMH_DESELECT_LEVEL		(0xDB)

#define OLED_NOP							(0xE3)
/* Definitions ---------------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/
void OLED_BEGIN(void);

void OLED_Write_Command(uint8_t* command, uint8_t buffer_length);

void OLED_Draw(uint8_t display_bit);

void OLED_Draw_Colon(void);
#endif /* INC_OLED_H_ */
