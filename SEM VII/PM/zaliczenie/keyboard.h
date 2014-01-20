#ifndef _SMS_KEYBOARD_
#define _SMS_KEYBOARD_

#define KEYBOARD_KEYUP -1
#define KEYBOARD_KEYLONGPAUSE -2

/* Initializes the keyboard module. */
void keyboard_init(void);

/* Registers a function as a key event handler.

Possible values for event:
- 0-15 : Keypad key number, starting from left upper corner, going right,
         then down.
- KEYBOARD_KEYUP : Indicates, that previously pushed key is now up.
- KEYBOARD_KEYLONGPAUSE : Indicates, that certain time has passed since the
						  last KEYBOARD_KEYUP event.
*/
void keyboard_set_handler(void (*handler)(int));

/* Returns a repeat mask.

If bit 'k' is set to 1, the key number 'k' will be repeated. */
uint16_t keyboard_get_repeat_mask(void);

/* Sets a repeat mask. */
void keyboard_set_repeat_mask(uint16_t mask);

#endif
