#ifndef __TELLO_JOY_H__
#define __TELLO_JOY_H__ 1

//#ifdef _PS3_ANDROID_JOY
#define JOY_START_BTN 9
#define JOY_SELECT_BTN 8
#define JOY_A_BTN 1
#define JOY_B_BTN 2
#define JOY_Y_BTN 3
#define JOY_X_BTN 0
#define CROSS_AXIS 2
#define LEFT_AXIS 0
#define RIGHT_AXIS 1
#define HORIZ_LEFT -32767
#define HORIZ_RIGHT 32767
#define HORIZ_UP -32767
#define HORIZ_DOWN 32767
#define JOY_R_BTN 5
#define JOY_L_BTN 4
#define JOY_ZR_BTN 7
#define JOY_ZL_BTN 6
//#endif

typedef void (*joy_button_func_t)(int button) ;
void joy_loop(const char *joypath, joy_button_func_t button_up, joy_button_func_t button_down);

#endif /* __TELLO_JOY_H__ */
