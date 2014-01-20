#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\signal.h>

#include "config.h" // Defines F_CPU and loads delay.
#include "util.h"

#include "lcd.h"


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
  asm volatile("nop; nop; nop");
  LCD_E_PORT &= ~(1 << OE);

  asm volatile("nop; nop; nop");

  b = swap(b);
  LCD_E_PORT |= 1 << OE;
  send_nibble(b);
  asm volatile("nop; nop; nop");
  LCD_E_PORT &= ~(1 << OE);

  _delay_us(40);
}

void lcd_set_position(uint8_t address) {
  address &= 0b01111111;
  address |= 0b10000000;
  lcd_send_command(address);
  _delay_us(40);
}

void lcd_send_command(uint8_t command) {
  LCD_RS_PORT &= ~(1 << RS);
  send_byte(command);
}


void lcd_send_data(uint8_t data) {
  LCD_RS_PORT |= 1 << RS;
  send_byte(data);
}


void lcd_display_control(uint8_t config) {
  lcd_send_command(0b00001000 | (0b00000111 & config));
  _delay_us(40);
}

void lcd_clear(void) {
  lcd_send_command(0b00000001);
  _delay_ms(1.64);
}


void lcd_return_home(void) {
  lcd_send_command(0b00000010);
  _delay_ms(1.64);
}


void lcd_entry_mode_set(uint8_t mode) {
  lcd_send_command(0b00000100 | (0b00000011 & mode));
  _delay_us(40);
}


void lcd_display_string(char* c, int len) {
  int i;
  for(i = 0; i < len; i++) {
    lcd_send_data(*(c + i));
    _delay_us(40);
  }
}

void lcd_init(void) {
  LCD_E_DDR |= 1 << OE;
  LCD_RS_DDR |= 1 << RS;

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
  asm volatile("nop \n nop \n nop \n");
  LCD_DATA_PORT &= ~(1 << OE);
  //  wait 4.1 ms
  _delay_ms(4.1);



  // second (30) instruction
  LCD_E_PORT |= 1 << OE;
  asm volatile("nop \n nop \n nop");
  LCD_DATA_PORT &= ~(1 << OE);
  // wait 100 microsecond
  _delay_us(100);

  // third (30).16 instruction
  LCD_E_PORT |= 1 << OE;
  asm volatile("nop \n nop \n nop");
  LCD_DATA_PORT &= ~(1 << OE);
  _delay_us(100);

  // 4bit interface activation
  LCD_E_PORT |= 1 << OE;
  LCD_DATA_PORT &= ~(1 << D4);
  asm volatile("nop \n nop \n nop");
  LCD_E_PORT &= ~(1 << OE);
  _delay_us(40);

  // function set
  lcd_send_command(0b00100000 | (LCD_N << 3) | (LCD_F << 2));
  _delay_us(40);


}