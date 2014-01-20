#ifndef _SMS_LCD_
#define _SMS_LCD_

#define LCD_ON (1 << 2)
#define LCD_CURSOR_ON (1 << 1)
#define LCD_CURSOR_BLINK (1 << 0)
#define LCD_WRITE_INC (1 << 1)
#define LCD_WINDOWS_SHIFT (1 << 0)
#define LCD_WRITE_DEC 0
#define LCD_CURSOR_SHIFT 0


/* Initializes LCD screen. */
void lcd_init(void);

/* Sends a command to the LCD screen. */
void lcd_send_command(uint8_t command);

/* Sends a byte to the LCD screen. */
void lcd_send_data(uint8_t data);

/* Sends a display control command to the LCD screen.
 *
 * Allowed flags: LCD_ON, LCD_CURSOR_ON, LCD_CURSOR BLINK */
void lcd_display_control(uint8_t config);

/* Sends a clear command to the LCD screen. */
void lcd_clear(void);

/* Sends a return home command to the LCD screen. */
void lcd_return_home(void);

/* Sets a position of the cursor to a given address. */
void lcd_set_position(uint8_t address);

/* Sets an entry mode for the lcd.

Possible flags: 
- LCD_WRITE_DEC (default)
- LCD_WRITE_INC
- LCD_CURSOR_SHIFT (default)
- LCD_WINDOWS_SHIFT

Note that default flags are in fact dummy (zeroes) in that case.
*/
void lcd_entry_mode_set(uint8_t mode);

/* Displays given string on lcd, starting from current cursor position.

Note that endline wrap is not handled (e.g. writing 5 chars from position 0x0F
is incorrect).
*/
void lcd_display_string(char* c, int len);

#endif