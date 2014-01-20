#ifndef _SMS_CONFIG_
#define _SMS_CONFIG_

#define F_CPU 8000000UL
#include <util/delay.h>

// {{{ LCD

// Hardware configuration

#define LCD_DATA_PORT PORTD
#define LCD_E_PORT    PORTD
#define LCD_RS_PORT   PORTD

#define LCD_DATA_DDR  DDRD
#define LCD_E_DDR     DDRD
#define LCD_RS_DDR    DDRD

#define OE 1
#define RS 0
#define D4 2
#define D5 3
#define D6 4
#define D7 5

// Screen configuration
#define LCD_N 1 // number of lines (0 => 1, 1 => 2)
#define LCD_F 0 // char matrix (0 => 5x8, 1 => 5x10)

// LCD }}}

#endif

