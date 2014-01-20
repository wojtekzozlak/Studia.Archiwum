#include <avr\eeprom.h>
#include <avr\pgmspace.h>
#include <stdio.h>
#include <string.h>

#include "config.h" // Defines F_CPU and loads delay.
#include "editor.h"
#include "keyboard.h"
#include "lcd.h"

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

#define DELETE_KEY 3
#define SWITCH_MODE_KEY 12
#define LEFT_KEY 14
#define RIGHT_KEY 15
#define UP_KEY 7
#define DOWN_KEY 11
#define CHAR_KEYS_MASK ((uint16_t)0b0010011101110111)

#define MAX_TEXT_LENGTH 128
#define LINE_SIZE 16
                                               	
enum EditorState {
  IDLE,
  CHAR_SELECTION
};


enum TransitionType {
  PROCEED = 0,
  STOP = 1,
};


enum TypingMode {
  LOWER = 0,
  FIRST_UPPER = 1,
  UPPER = 2,
  DIGITS = 3
};
const int TYPING_MODES = 4;


const uint8_t CHARS_MAPPINGS[] EEMEM =
{ 0, 5,   5, 4,   9, 4,   0, 0,
  13, 4,  17, 4,  21, 4,  0, 0,
  25, 5,  30, 4,  34, 5,  0, 0,
  0, 0,   39, 1,   0, 0,   0, 0
};


const uint8_t DIGITS_MAPPINGS[] EEMEM =
{
  4, 1,   8, 1,   12, 1,  0, 0,
  16, 1,  20, 1,  24, 1,  0, 0,
  29, 1,  33, 1,  38, 1,  0, 0,
  0, 0,   39, 1,   0, 0,   0, 0
};


const uint8_t LOWER_CHARS[] EEMEM =
{ ' ', ',', '.', ':', '1',
  'a', 'b', 'c', '2',
  'd', 'e', 'f', '3',
  'g', 'h', 'i', '4',
  'j', 'k', 'l', '5',
  'm', 'n', 'o', '6',
  'p', 'q', 'r', 's', '7',
  't', 'u', 'v', '8',
  'w', 'x', 'y', 'z', '9',
  '0' };


const uint8_t UPPER_CHARS[] EEMEM =
{ ' ', ',', '.', ':', '1',
  'A', 'B', 'C', '2',
  'D', 'E', 'F', '3',
  'G', 'H', 'I', '4',
  'J', 'K', 'L', '5',
  'M', 'N', 'O', '6',
  'P', 'Q', 'R', 'S', '7',
  'T', 'U', 'V', '8',
  'W', 'X', 'Y', 'Z', '9',
  '0' };


static char text_front[MAX_TEXT_LENGTH], text_back[MAX_TEXT_LENGTH];
static int text_front_p, text_back_p, window_position;
static enum EditorState editor_state;
static int pressed_key;
static uint8_t char_ix;
static enum TypingMode typing_mode;

char* refreshed;


void editor_handle_key(int);


void editor_init(char* refreshed_flag) {
  editor_state = IDLE;
  pressed_key = -1;
  refreshed = refreshed_flag;
  *refreshed = 0;
  typing_mode = LOWER;
  text_front_p = text_back_p = 0;
  window_position = 0;

  lcd_init();
  lcd_display_control(LCD_ON | LCD_CURSOR_ON | LCD_CURSOR_BLINK);
  lcd_clear();
  lcd_return_home();
  lcd_entry_mode_set(LCD_WRITE_INC);

  keyboard_init();
  keyboard_set_repeat_mask(1 << DELETE_KEY);
  keyboard_set_handler(editor_handle_key);
};


void editor_render_gui(void) {
  char buf[8];
  uint8_t length = text_front_p + text_back_p;

  // Characters counter.
  sprintf(buf, "%d/%d    ", text_front_p + 1, length);
  lcd_set_position( (0x40) );
  lcd_display_string(buf, 7);

  // Typing mode
  lcd_set_position(0x40 + 14);
  switch (typing_mode) {
    case LOWER:
      lcd_display_string("ab", 2);
      break;
    case FIRST_UPPER:
      lcd_display_string("Ab", 2);
      break;
    case UPPER:
      lcd_display_string("AB", 2);
      break;
    case DIGITS:
      lcd_display_string("12", 2);
      break;
    default:
      break;
  }
};


void editor_refresh(void) {
  int front_length = MIN(text_front_p - window_position, LINE_SIZE),
      back_length = MIN(LINE_SIZE - front_length, text_back_p),
      blank_length = 16 - front_length - back_length,
      i;

  if (editor_state == CHAR_SELECTION)
    lcd_display_control(LCD_ON | LCD_CURSOR_ON | LCD_CURSOR_BLINK);
  else
    lcd_display_control(LCD_ON | LCD_CURSOR_ON);

  lcd_set_position(0x0);
  lcd_display_string(text_front + window_position, front_length);
  for (i = 0; i < back_length; i++)
    lcd_display_string(&text_back[text_back_p - 1 - i], 1);
  for (i = 0; i < blank_length; i++)
    lcd_display_string(" ", 1);

  editor_render_gui();
};


void cursor_shift_left(void) {
  if (text_front_p > 0) {
    text_front_p--;
    text_back[text_back_p] = text_front[text_front_p];
    text_back_p++;
  }
  if (window_position > text_front_p)
    window_position = text_front_p;
  *refreshed = 0;
};


void cursor_shift_right(void) {
  if (text_back_p > 0) {
    text_back_p--;
    text_front[text_front_p] = text_back[text_back_p];
    text_front_p++;
  }
  if (window_position + LINE_SIZE < text_front_p + 1)
    window_position = text_front_p - LINE_SIZE + 1;
  *refreshed = 0;
};


void window_shift_left(void) {
  int i;
  window_position = window_position > LINE_SIZE ?
      window_position - LINE_SIZE : 0;
  for (i = 0; i < LINE_SIZE; i++)
    cursor_shift_left();
  *refreshed = 0;
};


void window_shift_right(void) {
  int i;
  if (window_position + LINE_SIZE < text_front_p + text_back_p)
    window_position += LINE_SIZE;
  for (i = 0; i < LINE_SIZE; i++)
    cursor_shift_right();
  *refreshed = 0;
};


void insert_char(uint8_t offset) {
  char c = typing_mode == LOWER ?
    (char) eeprom_read_byte(LOWER_CHARS + offset + char_ix) :
    (char) eeprom_read_byte(UPPER_CHARS + offset + char_ix);
  text_back[text_back_p] = c;
  text_back_p++;
  *refreshed = 0;
};


void update_char(uint8_t offset) {
  char c = typing_mode == LOWER ?
    (char) eeprom_read_byte(LOWER_CHARS + offset + char_ix) :
    (char) eeprom_read_byte(UPPER_CHARS + offset + char_ix);
  text_back[text_back_p - 1] = c;
  *refreshed = 0;
};


void delete_char(void) {
  if (text_front_p > 0)
    text_front_p--;
  if (window_position > text_front_p) {
    window_position = MAX(0, text_front_p - (LINE_SIZE / 2));
  }
  *refreshed = 0;
};


void editor_get_charkey_info(int key, uint8_t* offset, uint8_t* size) {
  uint8_t* map_addr =
    (uint8_t*) (typing_mode == DIGITS ? DIGITS_MAPPINGS : CHARS_MAPPINGS);

  *offset = eeprom_read_byte(map_addr + 2 * key);
  *size = eeprom_read_byte(map_addr + 2 * key + 1);
};


int editor_handle_idle(int key) {
  uint8_t offset = 0, size = 0;

  if (key >= 0 && (1 << key) & CHAR_KEYS_MASK) {
    editor_get_charkey_info(key, &offset, &size);
    // Display first character from set.
    pressed_key = key;
    char_ix = 0;
    if (text_front_p + text_back_p < MAX_TEXT_LENGTH)
      insert_char(offset);
    // Move right if it's the only possibility.
    if (size == 1)
      cursor_shift_right();
    else
      editor_state = CHAR_SELECTION;
  } else if (key == DELETE_KEY) {
    delete_char();
  } else if (key == SWITCH_MODE_KEY) {
    typing_mode = (typing_mode + 1) % TYPING_MODES;
    *refreshed = 0;
  } else if (key == RIGHT_KEY) {
    cursor_shift_right();
  } else if (key == LEFT_KEY) {
    cursor_shift_left();
  } else if (key == UP_KEY) {
    window_shift_left();
  } else if (key == DOWN_KEY) {
    window_shift_right();
  }

  return STOP;
};


void editor_maybe_switch_to_lower(void) {
  if (typing_mode == FIRST_UPPER) {
  	typing_mode = LOWER;
    *refreshed = 0;
  }
};

int editor_handle_char_selection(int key) {
  uint8_t offset = 0, size = 0;
  if (key >= 0 && (1 << key) & CHAR_KEYS_MASK) {
    if (key == pressed_key) {
      // Iterate through possible chars.
      editor_get_charkey_info(key, &offset, &size);
      char_ix = (char_ix + 1) % size;
      update_char(offset);
    } else {
      // Shift position, then handle as idle in with shifted cursor.
      editor_maybe_switch_to_lower();
      editor_state = IDLE;
      cursor_shift_right();
      return PROCEED;
    }
  } else if (key == KEYBOARD_KEYLONGPAUSE) {
    // Just shift to right.
    editor_state = IDLE;
    cursor_shift_right();
  } else if (key == DELETE_KEY) {
    // Clear char.
    cursor_shift_right();
    delete_char();
    editor_state = IDLE;
  } else if (key == SWITCH_MODE_KEY) {
    // Shift to right before handling like in idle state.
    cursor_shift_right();
    editor_state = IDLE;
    return PROCEED;
  } else if (key != KEYBOARD_KEYUP) {
    // Directional keys works like in idle state.
    editor_state = IDLE;
    return PROCEED;
  };

  return STOP;
};


void editor_handle_key(int key) {
  enum TransitionType next = STOP;
  do {
    if (editor_state == CHAR_SELECTION)
      next = editor_handle_char_selection(key);
    else if (editor_state == IDLE)
      next = editor_handle_idle(key);
  } while (next == PROCEED);
};
