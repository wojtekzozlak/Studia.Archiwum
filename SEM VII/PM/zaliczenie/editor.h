#ifndef _SMS_EDITOR_
#define _SMS_EDITOR_

/* Initializes the editor module.

Information about the need of refreshing the screen will be written
to the given location.
*/
void editor_init(char* refreshed_flag);


/* Refreshes the lcd screen. */
void editor_refresh(void);

#endif