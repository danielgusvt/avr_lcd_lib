/*
 * LCD_LIB.c
 *
 * Created: 7/01/2022 22:47:09
 * Author: Daniel Vásquez
 */

#include "LCD_LIB.h"

static uint8_t _lcd_params;
static char _lcd_buffer_[_LCD_NCOL_ + 1];

void _lcd_write_nibble(uint8_t nibble)
{
	_LCD_PORTbits.LCD_DATA_NIBBLE = nibble;

	_LCD_PORTbits.LCD_EN = 0; //set low
	_delay_us(1);

	_LCD_PORTbits.LCD_EN = 1; //set high
	_delay_us(1);    // enable pulse must be >450 ns

	_LCD_PORTbits.LCD_EN = 0; //set low
	_delay_us(100);   // Commands need to be greater than 37 us to settle
}

// RS 0 for _LCD_COMMAND_ and RS 1 for _LCD_DATA_
void _lcd_send_byte(uint8_t value, uint8_t mode)
{
	_LCD_PORTbits.LCD_RS = mode;

	_lcd_write_nibble(value >> 4);
	_lcd_write_nibble(value & 0xF);
}

void lcd_write_char(uint8_t letra)
{
	_lcd_send_byte(letra, _LCD_DATA_);
}

void lcd_command(uint8_t cmd)
{
	_lcd_send_byte(cmd, _LCD_COMMAND_);
}

void lcd_init(void)
{
/*
	_LCD_DDR |= (1 << LCD_RS) | (1 << LCD_EN) | (1 << LCD_D4)
				| (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7);
	Equals to 0b00111111, hardcode to save 2 bytes
*/
	_LCD_DDR |= 0x3F;

/*
	_LCD_PORT &= ~((1 << LCD_RS) | (1 << LCD_EN) | (1 << LCD_D4)
				| (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7));
	Equals to 0b11000000, hardcode to 0 and save 4 bytes
*/
	_LCD_PORT &= 0xC0;

	_delay_us(50000);

	// We start in 8bit mode, try to set 4 bit mode
	_lcd_write_nibble(0x03);
	_delay_us(4500); // wait min 4.1ms

	// Second try
	_lcd_write_nibble(0x03); // wait min 4.1 ms
	_delay_us(4500);

	// Third go
	_lcd_write_nibble(0x03);
	_delay_us(150);

	_lcd_write_nibble(0x02); // set 4bit

	// Function set
	lcd_command(_LCD_FUNCTIONSET | _LCD_4BITMODE | _LCD_2LINE | _LCD_5x8DOTS | _LCD_FT_WESTERN_EUROPEAN);
	_delay_us(37);

	// Display control
	_lcd_params = _LCD_DISPLAY_ON | _LCD_CURSOR_OFF | _LCD_BLINK_OFF;
	lcd_command(_LCD_DISPLAYCONTROL | _lcd_params);
	_delay_us(37);

	lcd_clear();

	// Initialize to default text direction (for romance languages)
	lcd_command(_LCD_ENTRYMODESET | _LCD_INCREMENT | _LCD_SHIFT_OFF);
}

void lcd_write_string(const char *str)
{
	while (*str) { // while(*str != '\0')
#if LCD_REMAP_WESTERN_CHARS
		switch (*str) {
		case 'á':
			lcd_write_char(0x9D);
			break;
		case 'é':
			lcd_write_char(0x96);
			break;
		case 'í':
			lcd_write_char(0xA3);
			break;
		case 'ó':
			lcd_write_char(0x8D);
			break;
		case 'ú':
			lcd_write_char(0x86);
			break;
// 		case 'Á':
// 			lcd_write_char(0b);
// 			break;
		case 'É':
			lcd_write_char(0x92);
			break;
// 		case 'Í':
// 			lcd_write_char(0b);
// 			break;
		case 'Ó':
			lcd_write_char(0x89);
			break;
		case 'Ú':
			lcd_write_char(0x82);
			break;
		case 'ñ':
			lcd_write_char(0xA7);
			break;
		case 'Ñ':
			lcd_write_char(0xA6);
			break;
		default:
#endif /* LCD_REMAP_WESTERN_CHARS */
			lcd_write_char(*str);
#if LCD_REMAP_WESTERN_CHARS
			break;
		}
#endif /* LCD_REMAP_WESTERN_CHARS */
		str++;
	}
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

void lcd_printf(char *str, ...)
{
	va_list args;
	va_start(args, str);
	vsnprintf(_lcd_buffer_, _LCD_NCOL_ + 1, str, args);

	va_end(args);

	lcd_write_string(_lcd_buffer_);
}

void lcd_clear(void)
{
	lcd_command(_LCD_CLEARDISPLAY);
	_delay_us(2000);
}

void lcd_home(void)
{
	lcd_command(_LCD_RETURNHOME);
	_delay_us(2000);
}

void lcd_on(void)
{
	_lcd_params |= _LCD_DISPLAY_ON;
	lcd_command(_LCD_DISPLAYCONTROL | _lcd_params);
	_delay_us(37);
}

void lcd_off(void)
{
	_lcd_params &= _LCD_DISPLAY_OFF;
	lcd_command(_LCD_DISPLAYCONTROL | _lcd_params);
	_delay_us(37);
}

void lcd_enable_blink(void)
{
	_lcd_params |= _LCD_BLINK_ON;
	lcd_command(_LCD_DISPLAYCONTROL | _lcd_params);
	_delay_us(37);
}

void lcd_disable_blink(void)
{
	_lcd_params &= ~_LCD_BLINK_ON;
	lcd_command(_LCD_DISPLAYCONTROL | _lcd_params);
	_delay_us(37);
}

void lcd_enable_cursor(void)
{
	_lcd_params |= _LCD_CURSOR_ON;
	lcd_command(_LCD_DISPLAYCONTROL | _lcd_params);
	_delay_us(37);
}

void lcd_disable_cursor(void)
{
	_lcd_params &= ~_LCD_CURSOR_ON;
	lcd_command(_LCD_DISPLAYCONTROL | _lcd_params);
	_delay_us(37);
}

void lcd_scroll_left(void)
{
	lcd_command(_LCD_CURSORDISPLAYSHIFT | _LCD_DISPLAY_SHIFT | _LCD_MOVELEFT);
	_delay_us(37);
}

void lcd_scroll_right(void)
{
	lcd_command(_LCD_CURSORDISPLAYSHIFT | _LCD_DISPLAY_SHIFT | _LCD_MOVERIGHT);
	_delay_us(37);
}

void lcd_custom_char(uint8_t mem, uint8_t *charmap)
{
	uint8_t i;

	lcd_command(_LCD_SET_CGRAM_ADDR | ((mem & 0x7) << 3));

	for (i = 0; i < 8; ++i)
		lcd_write_char(charmap[i]);

	lcd_command(_LCD_SET_DDRAM_ADDR);
}
