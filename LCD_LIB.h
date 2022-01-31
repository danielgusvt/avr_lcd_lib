/*
 * LCD_LIB.h
 *
 * Created: 7/01/2022 22:46:59
 *  Author: Daniel Vásquez
 */

#ifndef LCD_LIB_H_
#define LCD_LIB_H_

#define F_CPU 20000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdarg.h>

/*
 * RS behaviour
 */
#define _LCD_COMMAND_ 0
#define _LCD_DATA_ 1

/*
 * Define the MCU port to be connected to the display
 */
#define _LCD_DDR DDRB
#define _LCD_PORT PORTB

/************************************************************************/
/*	In order to save memory, the pins to connect to the LCD are sort of	*/
/*	"hardcoded", this library uses the first 6 pins of any MCU port as	*/
/*	follows:															*/
/*	  _LCD_RS 0															*/
/*	  _LCD_EN 1															*/
/*	  _LCD_D4 2															*/
/*	  _LCD_D5 3															*/
/*	  _LCD_D6 4															*/
/*	  _LCD_D7 5															*/
/*	It's possible to move the RS and EN pins to different positions of	*/
/*	the port as long as the data pins (D4 to D7) are grouped together.	*/
/************************************************************************/

typedef struct lcd_port_map_t{
	uint8_t LCD_RS :1;
	uint8_t LCD_EN :1;
	uint8_t LCD_DATA_NIBBLE :4;
	uint8_t :1;
	uint8_t :1;
}lcd_port_map;

#if (!defined _LCD_DDR || !defined _LCD_PORT)
  #warning "LCD port assigned by default!"
  #define _LCD_DDR DDRB
  #define _LCD_PORT PORTB
#endif /* (!defined _LCD_DDR || !defined _LCD_PORT) */

#define _LCD_PORTbits (*(volatile lcd_port_map *)_SFR_MEM_ADDR(_LCD_PORT))

#define _LCD_NCOL_ 16
#define _LCD_NROW_ 2

/************************************************************************/
/*		INSTRUCTIONS													*/
/************************************************************************/
// Clear display
#define _LCD_CLEARDISPLAY 0x01

// Return home
#define _LCD_RETURNHOME 0x02

/************************************************************************/
/*       Entry mode set:  D7 D6 D5 D4 D3 D2 D1  D0						*/
/*      				  0  0  0  0  0  1  I/D  S						*/
/*----------------------------------------------------------------------*/
/*      I/D = 1: Inc                                                    */
/*		      0: Dec                                                    */
/*		S   = 1: SHIFT ON                                               */
/*            0: SHIFT OFF                                              */
/************************************************************************/
#define _LCD_ENTRYMODESET	0x04
#define _LCD_INCREMENT		0x02
#define _LCD_DECREMENT		0
#define _LCD_SHIFT_ON		0x01
#define _LCD_SHIFT_OFF		0

/************************************************************************/
/*      Display control:  D7 D6 D5 D4  D3 D2 D1 D0						*/
/*      				  0  0  0  0   1  D  C  B						*/
/*----------------------------------------------------------------------*/
/*      D   = 1: DISPLAY ON                                             */
/*		      0: DISPLAY OFF                                            */
/*		C   = 1: CURSOR ON                                              */
/*		      0: CURSOR OFF                                             */
/*		B   = 1: BLINK                                                  */
/*		      0: NO BLINK                                               */
/************************************************************************/
#define _LCD_DISPLAYCONTROL	0x08
#define _LCD_DISPLAY_ON		0x04
#define _LCD_DISPLAY_OFF	0
#define _LCD_CURSOR_ON		0x02
#define _LCD_CURSOR_OFF		0
#define _LCD_BLINK_ON		0x01
#define _LCD_BLINK_OFF		0

/************************************************************************/
/*		Cursor or display shift:	 D7 D6 D5 D4 D3  D2  D1 D0			*/
/*      						     0  0  0  1  S/C R/L  *  *			*/
/*----------------------------------------------------------------------*/
/*      S/C = 1: display shift                                          */
/*		      0: cursor move                                            */
/*		R/L = 1: shift to the right                                     */
/*		      0: shift to the left                                      */
/************************************************************************/
#define _LCD_CURSORDISPLAYSHIFT 0x10
#define _LCD_DISPLAY_SHIFT      0x08
#define _LCD_CURSOR_SHIFT       0
#define _LCD_MOVERIGHT          0x04
#define _LCD_MOVELEFT           0

/************************************************************************/
/*      Functions set:		D7 D6 D5 D4 D3 D2 D1  D0					*/
/*      					0  0  1  D/L N F  FT1 FT0					*/
/*----------------------------------------------------------------------*/
/*      D/L				= 1: 8 bits mode								*/
/*		(data length)     0: 4 bits mode								*/
/*		N				= 1: 2 lines mode								*/
/*						  0: 1 lines mode								*/
/*		F				= 1: 5x10 matrix                                */
/*						  0: 5x7/5x8 matrix								*/
/*																		*/
/*		Font Table (OLED-0010 exclusive):								*/
/*		------------------------------------							*/
/*		FT[1:0] = 00 ENGLISH_JAPANESE CHARACTER FONT TABLE (default)	*/
/*		FT[1:0] = 01 WESTERN_EUROPEAN CHARACTER FONT TABLE				*/
/*		FT[1:0] = 10 ENGLISH_RUSSIAN CHARACTER FONT TABLE				*/
/*		FT[1:0] = 11 N/A												*/
/************************************************************************/
#define _LCD_FUNCTIONSET			0x20
#define _LCD_8BITMODE				0x10
#define _LCD_4BITMODE				0x00
#define _LCD_2LINE					0x08
#define _LCD_1LINE					0
#define _LCD_5x10DOTS				0x04
#define _LCD_5x8DOTS				0

#define _LCD_FT_ENGLISH_JAPANESE    0
#define _LCD_FT_WESTERN_EUROPEAN    1
#define _LCD_FT_ENGLISH_RUSSIAN     0x02
// Remap characters for lower/uppercase Spanish accents and ñ character
#define LCD_REMAP_WESTERN_CHARS 0

/************************************************************************/
/*      SET CGRAM:  D7 D6  D5  D4   D3   D2   D1   D0                   */
/*      			0  1   ACG ACG  ACG  ACG  ACG  ACG                  */
/*----------------------------------------------------------------------*/
/*      ACG -> CGRAM ADDRESS                                            */
/************************************************************************/
#define _LCD_SET_CGRAM_ADDR  0x40

/************************************************************************/
/*      SET DDRAM:  DB7 DB6  DB5  DB4  DB3  DB2  DB1  DB0				*/
/*      			1   ADD6 ADD5 ADD4 ADD3 ADD2 ADD1 ADD0				*/
/*----------------------------------------------------------------------*/
/*      ADD -> DDRAM ADDRESS                                            */
/************************************************************************/
#define _LCD_SET_DDRAM_ADDR  0x80

// Function prototypes
void _lcd_write_nibble(uint8_t nibble);
void _lcd_send_byte(uint8_t value, uint8_t mode);
void lcd_init(void);
void lcd_command(uint8_t cmd);
void lcd_write_char(uint8_t letra);
void lcd_write_string(const char *str);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_printf(char *str, ...);

// Commands
void lcd_clear(void);
void lcd_home(void);
void lcd_on(void);
void lcd_off(void);
void lcd_enable_blink(void);
void lcd_disable_blink(void);
void lcd_enable_cursor(void);
void lcd_disable_cursor(void);
void lcd_scroll_left(void);
void lcd_scroll_right(void);

void lcd_custom_char(uint8_t mem, uint8_t *charmap);

#endif /* LCD_LIB_H_ */
