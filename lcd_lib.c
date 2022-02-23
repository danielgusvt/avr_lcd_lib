#include <generated/csr.h>
#include "lcd_lib.h"

uint8_t _displayfunction = 0;
uint8_t _lcd_params = 0;

void _lcd_send_byte(uint8_t value, uint8_t mode) {
	uint16_t tosend = (value & 0xFF);

	if (mode)
		tosend |= (1 << _LCD_RS);
	else
		tosend &= ~(1 << _LCD_RS);

	_LCD_PORT = tosend;

	_LCD_PORT &= ~(1 << _LCD_EN); //low
	busy_wait_us(1);
	_LCD_PORT |= 1 << _LCD_EN; //high
	busy_wait_us(1);    // enable pulse must be >450 ns
	_LCD_PORT &= ~(1 << _LCD_EN); //low
	busy_wait_us(100);   // commands need >37 us to settle
}

void lcd_command(uint8_t cmd) //TODO INLINE
{
	_lcd_send_byte(cmd, _LCD_COMMAND_);
}

void lcd_clear(void)
{
	lcd_command(_LCD_CLEARDISPLAY);
	busy_wait_us(2000);
}

void lcd_write_char(uint8_t letra)
{
	_lcd_send_byte(letra, _LCD_DATA_);
}

void lcd_init(void)
{
	_displayfunction = _LCD_8BITMODE | _LCD_2LINE | _LCD_5x8DOTS;

	// according to datasheet, we need at least 40 ms after power rises above 2.7 V
	// busy_wait_us(50000); 
	// Now we pull both RS and R/W low to begin commands
	_LCD_PORT = 0;

	// put the LCD into 8 bit mode
	// this is according to the Hitachi HD44780 datasheet
	// page 45 figure 23
	// Send function set command sequence, set # lines, font size, etc.
	lcd_command(_LCD_FUNCTIONSET | _displayfunction);
	busy_wait_us(4500);  // wait more than 4.1 ms

	// second try
	lcd_command(_LCD_FUNCTIONSET | _displayfunction);
	busy_wait_us(150);

	// third go
	lcd_command(_LCD_FUNCTIONSET | _displayfunction);
	busy_wait_us(37);

	// turn the display on with cursor and blinking on
	_lcd_params = _LCD_DISPLAY_ON | _LCD_CURSOR_OFF | _LCD_BLINK_OFF;
	lcd_command(_LCD_DISPLAYCONTROL | _lcd_params);
	busy_wait_us(37);

	// clear it off
	lcd_clear();

	// Initialize to default text direction (for romance languages)
	lcd_command(_LCD_ENTRYMODESET | _LCD_INCREMENT | _LCD_SHIFT_OFF);
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
	// Corresponds to 16x2
	static uint8_t local_mem[_LCD_NROW_] = {0, 0x40};
	/*
	 * We first take the DDRAM address 0x80; for the first row we just sum the columns,
	 * for the second one we sum 0b1000000 or 0x40.
	 * We subtract 1 because we want to assign the address starting from 1, and we use
	 * ternary operators in order to avoid issues if we set row or column as 0.
	 */
	lcd_command(_LCD_SET_DDRAM_ADDR | (local_mem[row - (row ? 1 : 0)] + col - (col ? 1 : 0)));
}

void lcd_write_string(const char *str)
{
	while (*str) { // while(*str != '\0')
		lcd_write_char(*str);
		str++;
	}
}