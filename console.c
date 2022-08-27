#include "console.h"

#define INIT_X 140
#define INIT_Y 240
#define MAX_CMD_SZ 100

#define iscmd(x) !strncmp(message, x, strlen(x))

static int lineY = INIT_Y;
static int lineX = INIT_X;

static char message[MAX_CMD_SZ];
static int top = 0;

static void console_reset(void)
{
    lineY = INIT_Y;
    lineX = INIT_X;
    memset(message, 0, MAX_CMD_SZ);
    top = 0;
}

void console_addkeypress(char c)
{
    if(top >= MAX_CMD_SZ) return;
    message[top] = c;
    top++;
}

void console_backspace(void)
{
    if(top <= 0) return;
    top--;
    message[top]='\0';
}

void console_render(void)
{
    sdl_text(message, lineX, lineY, top*20, 30);
}

tello_cmd_t console_get_command(void)
{

    tello_cmd_t cmd = CMD_UNKNOWN;
    printf("message=|%s|\n", message);
    if(iscmd("takeoff")) cmd = TELLO_TAKEOFF;
    if(iscmd("land"))    cmd = TELLO_LAND;
    if(iscmd("green"))   cmd = RENDER_GREEN;
    if(iscmd("bw"))      cmd = RENDER_BW;
    if(iscmd("rgb"))     cmd = RENDER_RGB;
    if(iscmd("enhance")) cmd = RENDER_ENHANCE;
    if(iscmd("night"))   cmd = RENDER_NIGHT;
    if(iscmd("streamon")) cmd = TELLO_STREAM_ON;
    if(iscmd("streamoff")) cmd = TELLO_STREAM_OFF;

    if(iscmd("up")) cmd = TELLO_UP;
    if(iscmd("down")) cmd = TELLO_DOWN;
    if(iscmd("left")) cmd = TELLO_LEFT;
    if(iscmd("right")) cmd = TELLO_RIGHT;
    if(iscmd("fwd")) cmd = TELLO_FORWARD;
    if(iscmd("back")) cmd = TELLO_BACKWARD;
    if(iscmd("cw")) cmd = TELLO_CW;
    if(iscmd("ccw")) cmd = TELLO_CCW;

    console_reset();
    return cmd;
}
