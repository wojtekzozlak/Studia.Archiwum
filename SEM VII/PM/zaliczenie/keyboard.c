#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\sleep.h>

#include <string.h>
#include <stdio.h>

#include "config.h" // Defines F_CPU and loads delay.
#include "util.h"
#include "keyboard.h"
#include "lcd.h"

#define TIMER_PRESCALLER (1 << CS22 | 1 << CS20) // 1024
#define TICKS_TILL_REPEAT 100
#define TICKS_TILL_SURE 1 
#define TICKS_PER_UP_CHECK 1
#define TICKS_TILL_LONG_PAUSE 200

enum TimerState {
  IDLE = 0,
  DOWN_CHECK_START = 1,
  DOWN_CHECK_END = 2,
  UP_CHECK_START = 3,
  UP_CHECK_END = 4,
  LONG_PAUSE_CHECK = 5,
  REPEAT_CHECK_START = 6,
  REPEAT_CHECK_END = 7
};

static enum TimerState updown_state, repeater_state;
static int updown_counter, repeater_counter;
static int updown_ocr, repeater_ocr;
static int pressed_key;
static uint16_t repeat_key;
static void (*key_handler)(int) = 0;


uint16_t keyboard_get_repeat_mask(void) {
  return repeat_key;
};

void keyboard_set_repeat_mask(uint16_t mask) {
  repeat_key = mask;
};


int scan_keyboard(void) {
  GICR &= ~(1 << (INT2));
  int rows, cols, row, col, i;
  row = col = 0;

  // Scan rows.
  for (i = 0, rows = 0; i < 4; i++) {
    if (!(PINA & (1 << i))) {
      rows++;
      row = i;
    }
  }

  DDRA = 0b00001111;
  PORTA = 0b11110000;
  asm("nop;");
  
  // Scan columns.
  for (i = 4, cols = 0; i < 8; i++) {
    if (!(PINA & (1 << i))) {
      cols++;
      col = i - 4;
    }	
  }

  DDRA = 0b11110000;
  PORTA = 0b00001111;
  asm("nop");

  GIFR |= 1 << INT2;
  GICR |= 1 << INT2;

  if (rows == 1 && cols == 1) {
    return row * 4 + col;
  }
  return -1;
};


void keyboard_init(void) {
  uint8_t tmp;

  // Keyboard pins configuration.
  DDRA = 0b11110000;
  PORTA = 0b00001111;

  tmp = MCUCR;
  tmp &= ~(1 << ISC00);
  tmp |= 1 << ISC01;

  MCUCSR &= ~(1 << ISC2);

  GICR |= 1 << INT2;
  GIFR |= 1 << INT2;

  // Timer2 configuration.
  TCCR2 = TIMER_PRESCALLER;

  tmp = TIMSK;
  tmp |= 1 << TOIE2; // Interruption on overflow.
  TIMSK = tmp;

  // Internal counters.
  updown_state = IDLE;
  updown_counter = 0;
  updown_ocr = 0;

  repeater_state = IDLE;
  repeater_counter = 0;
  repeater_ocr = 0;

  pressed_key = -1;
};


void keyboard_set_handler(void (*handler)(int)) {
  key_handler = handler;
}


void handle_updown(void) {
  int key = scan_keyboard();
  if (updown_state == DOWN_CHECK_START) {
    if (key >= 0) {
      pressed_key = key;
      updown_state = DOWN_CHECK_END;
      updown_ocr = TICKS_TILL_SURE;
    } else {
      updown_state = IDLE;
      return ;
    }
  } else if (updown_state == DOWN_CHECK_END) {
    if (key >= 0 && key == pressed_key) {
      key_handler(pressed_key);
      if (repeat_key & 1 << pressed_key) {
        repeater_state = REPEAT_CHECK_START;
        repeater_counter = 0;
        repeater_ocr = TICKS_TILL_REPEAT;
      }
      updown_state = UP_CHECK_START;
      updown_ocr = TICKS_PER_UP_CHECK;
    } else {
      updown_state = IDLE;
    }
  } else if (updown_state == UP_CHECK_START) {
    if (key == -1) {
      updown_state = UP_CHECK_END;
      updown_ocr = TICKS_TILL_SURE;
    } else {
      updown_state = UP_CHECK_START;
      updown_ocr = TICKS_PER_UP_CHECK;
    }
  } else if(updown_state == UP_CHECK_END) {
    if (key == -1) {
      key_handler(KEYBOARD_KEYUP);
      pressed_key = -1;
      repeater_state = IDLE;
      updown_state = LONG_PAUSE_CHECK;
      updown_ocr = TICKS_TILL_LONG_PAUSE;
    } else {
      updown_state = UP_CHECK_START;
      updown_ocr = TICKS_PER_UP_CHECK;
    }
  } else if(updown_state == LONG_PAUSE_CHECK) {
    key_handler(KEYBOARD_KEYLONGPAUSE);
    updown_state = IDLE;
    set_sleep_mode(SLEEP_MODE_STANDBY);
  }
};


void handle_repeater(void) {
  int key = scan_keyboard();
  if (key == pressed_key) {
    // key pressed
    if (repeater_state == REPEAT_CHECK_START) {
      // wait till sure
      repeater_state = REPEAT_CHECK_END;
      repeater_ocr = TICKS_TILL_SURE;
    } else {
      key_handler(pressed_key);
      //PORTA ^= 1 << PA2;
      repeater_state = REPEAT_CHECK_START;
      repeater_ocr = TICKS_TILL_REPEAT;
    }
  } else {
    // bad key pressed, abort repeating
    repeater_state = IDLE;
  }
};


ISR(TIMER2_OVF_vect) {
  if (updown_state == IDLE) return;

  //PORTA ^= 1 << PA4;

  updown_counter++;
  repeater_counter++;
  if (updown_state != IDLE && updown_counter >= updown_ocr) {
    handle_updown();
    updown_counter = 0;
  }
  if (repeater_state != IDLE && repeater_counter >= repeater_ocr) {
    handle_repeater();
    repeater_counter = 0;
  }
};


ISR(INT2_vect) {
  if (updown_state == IDLE || updown_state == LONG_PAUSE_CHECK) {
    set_sleep_mode(SLEEP_MODE_IDLE);
    updown_state = DOWN_CHECK_START;
    updown_counter = 0;
    updown_ocr = TICKS_TILL_SURE;
  }

  GIFR |= 1 << INT2;
};

