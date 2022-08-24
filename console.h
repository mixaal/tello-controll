#ifndef _TELLO_CONSOLE_H_
#define _TELLO_CONSOLE_H_ 1

#include "config.h"
#include "sdl.h"

typedef enum { RENDER_GREEN, RENDER_BW, TELLO_TAKEOFF, TELLO_LAND, TELLO_FORWARD, TELLO_BACKWARD, TELLO_UP, TELLO_DOWN, TELLO_LEFT, TELLO_RIGHT, TELLO_STREAM_ON, TELLO_CW, TELLO_CCW, TELLO_STREAM_OFF, CMD_UNKNOWN} tello_cmd_t;

void console_addkeypress(char c);
void console_render(void);
void console_backspace(void);
tello_cmd_t console_get_command(void);
#endif /* _TELLO_CONSOLE_H_ */
