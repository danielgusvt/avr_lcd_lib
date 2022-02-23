#include <stdint.h>

#define _LCD_RS 8
#define _LCD_EN 10
#define _LCD_PORT MMPTR(CSR_GPIO_BASE)
#define _LCD_FUNCTIONSET			0x20
#define _LCD_ENTRYMODESET	0x04
#define _LCD_COMMAND_ 0
#define _LCD_DATA_ 1
#define _LCD_INCREMENT		0x02
#define _LCD_SHIFT_OFF		0
#define _LCD_CLEARDISPLAY 0x01
#define _LCD_DISPLAYCONTROL	0x08
#define _LCD_DISPLAY_ON		0x04
#define _LCD_DISPLAY_OFF	0
#define _LCD_CURSOR_ON		0x02
#define _LCD_CURSOR_OFF		0
#define _LCD_BLINK_ON		0x01
#define _LCD_BLINK_OFF		0
#define _LCD_8BITMODE				0x10
#define _LCD_2LINE					0x08
#define _LCD_1LINE					0
#define _LCD_5x10DOTS				0x04
#define _LCD_5x8DOTS				0
#define _LCD_NCOL_ 16
#define _LCD_NROW_ 2
#define _LCD_SET_DDRAM_ADDR  0x80

void _lcd_send_byte(uint8_t value, uint8_t mode);
void lcd_command(uint8_t cmd);
void lcd_clear(void);
void lcd_write_char(uint8_t letra);
void lcd_init(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_write_string(const char *str);