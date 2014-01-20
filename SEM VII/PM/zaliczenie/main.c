#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\signal.h>
#include <avr\eeprom.h>
#include <avr\pgmspace.h>
#include <avr\sleep.h>

#include "config.h" // Defines F_CPU and loads delay.
#include "editor.h"
#include "lcd.h"
#include "keyboard.h"
#include "util.h"

int main(void) {
  char refreshed;

  editor_init(&refreshed);
  editor_refresh();

  set_sleep_mode(SLEEP_MODE_STANDBY);
  sei();


  for (;;) {
    cli();
    if (refreshed) {
      sleep_enable();
      sei();
      sleep_cpu();
      sleep_disable();
    } else {
      refreshed = 1;
      sei();
      editor_refresh();
    }
  }

};