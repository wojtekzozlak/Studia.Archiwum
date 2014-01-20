// ***********************************************************
// Project:
// Author:
// Module description:
// ***********************************************************
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 8000000UL
#include <util/delay.h>

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

uint8_t swap(uint8_t b) {
  return (b << 4) | (b >> 4);
};

void send_nibble(uint8_t b) {
  LCD_DATA_PORT &= ~(1 << D4);
  LCD_DATA_PORT &= ~(1 << D5);
  LCD_DATA_PORT &= ~(1 << D6);
  LCD_DATA_PORT &= ~(1 << D7);

  if ((1 << 4) & b)
    LCD_DATA_PORT |= (1 << D4);
  if ((1 << 5) & b)
    LCD_DATA_PORT |= (1 << D5);
  if ((1 << 6) & b)
    LCD_DATA_PORT |= (1 << D6);
  if ((1 << 7) & b)
    LCD_DATA_PORT |= (1 << D7);
};

void send_byte(uint8_t b) {
  LCD_E_PORT |= 1 << OE;
  send_nibble(b);
  asm volatile("nop;nop;nop;");
  LCD_E_PORT &= ~(1 << OE);

  b = swap(b);
  LCD_E_PORT &= ~(1 << OE);
  send_nibble(b);
  asm volatile("nop;nop;nop;");
  LCD_E_PORT &= ~(1 << OE);

  _delay_ms(40);
}

void send_command(uint8_t command) {
  CBI(LCD_RS_PORT, RS);
  send_byte(command);
}

void send_data(uint8_t data) {
  LCD_RS_PORT |= 1 << RS;
  send_byte(data);
}

void lcd_init(void) {
  LCD_RS_DDR |= 1 << RS;
  LCD_E_DDR |= 1 << OE;

  LCD_DATA_DDR |= 1 << D4;
  LCD_DATA_DDR |= 1 << D5;
  LCD_DATA_DDR |= 1 << D6;
  LCD_DATA_DDR |= 1 << D7;

  LCD_RS_PORT &= ~(1 << RS);
  LCD_E_PORT &= ~(1 << OE);
  _delay_ms(40);


  // first (30) instruction
  LCD_E_PORT |= 1 << OE;
  LCD_DATA_PORT |= 1 << D4;
  LCD_DATA_PORT |= 1 << D5;
  LCD_DATA_PORT &= ~(1 << D6);
  LCD_DATA_PORT &= ~(1 << D7);
  asm volatile("nop;nop;nop;");
  LCD_E_PORT &= ~(1 << OE);
  _delay_ms(5);

  // second (30) instruction
  LCD_E_PORT |= 1 << OE;
  asm volatile("nop;nop;nop;");
  LCD_E_PORT &= ~(1 << OE);
  _delay_us(100);

  // third (30) instruction
  LCD_E_PORT |= 1 << OE;
  asm volatile("nop;nop;nop;");
  LCD_E_PORT &= ~(1 << OE);
  _delay_us(100);

  // 4bit interface activation
  LCD_E_PORT |= 1 << OE;
  LCD_DATA_PORT &= ~(1 << D4);
  asm volatile("nop;nop;nop;");
  LCD_E_PORT &= ~(1 << OE);
  _delay_us(40);
}

int main(void) {
  lcd_init();
  _delay_ms(2);


  send_command(0b00101000);
  _delay_ms(2);

/*  send_command(0b00101000);
  _delay_ms(2);

  send_command(0b00000001);
  _delay_ms(2);

  send_command(0b00000011);
  _delay_ms(2);

  send_command(0b00000110);
  _delay_ms(2);

  send_data('a');
  _delay_ms(2); */


  for (;;);
}


