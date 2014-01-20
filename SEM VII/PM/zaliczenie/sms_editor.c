#include "sms_editor.h"
#include "keyboard.h"

#define DELETE_KEY 3
#define SWITCH_MODE_KEY 12
#define LEFT_KEY 13
#define RIGHT_KEY 14
#define UP_KEY 7
#define DOWN_KEY 8

#define MAX_TEXT_LENGTH 256
#define LINE_SIZE 16

enum EditorState {
	IDLE,
	CHAR_SELECTION
};

const char text[MAX_TEXT_LENGTH];

static const uint16_t CHAR_KEYS_MASK = 0b1110111011100100;
static int length;
static int cursor_position;
static enum editor_state;
static int pressed_key;

char maybe_refresh;

void editor_init(void) {
  length = 0;
  cursor_position = 0;
  editor_state = IDLE;
  pressed_key = -1;
  maybe_refresh = 0;
};


void editor_refresh(void) {
  
};


void cursor_shift_left(void) {
  cursor_position = cursor_position == 0 ?
    0 : cursor_position - 1;
};


void cursor_shift_right(void) {
  if (cursor_position <= MAX_TEXT_LENGTH && cursor_position + 1 <= length)
  	cursor_position++;
};


char handle_idle(int key) {
  if (key == KEYBOARD_LONGPAUSE || KEY == SWITCH_MODE_KEY ||
  	  key == RIGHT_KEY) {
  	// Accept key and shift cursor to right.
    pressed_key = -1;
    cursor_shift_right();
  } else if (key == LEFT_KEY) {
  	// Accept and shift left
  	pressed_key = -1;
  	cursor_shift_left();
  }
};

char handle_char_selection(int key) {
  if (key & CHAR_KEYS_MASK) {

  } else handle_idle(key);
};


void handle_key(int key) {
  char result = 0;
  do {
    if (editor_state == CHAR_SELECTION)
  	  result = handle_char_selection(key);
    else if (editor_state == IDLE)
  	  result = handle_idle(key); 
  } while (result != 0);

  maybe_refresh = 1;
};