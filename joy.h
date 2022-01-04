#ifndef __TELLO_JOY_H__
#define __TELLO_JOY_H__ 1


#define GAME_HAT_JOY_NAME "GPIO Controller 1"


typedef enum {JOY_UNKNOWN_BTN, JOY_START_BTN, JOY_SELECT_BTN, JOY_A_BTN, JOY_B_BTN, JOY_X_BTN, JOY_Y_BTN, JOY_R_BTN, JOY_L_BTN, JOY_ZR_BTN, JOY_ZL_BTN } joy_index_t;



//#define JOY_START_BTN 9
//#define JOY_SELECT_BTN 8
//#define JOY_A_BTN 1
//#define JOY_B_BTN 2
//#define JOY_Y_BTN 3
//#define JOY_X_BTN 0
#define CROSS_AXIS 2
#define LEFT_AXIS 0
#define RIGHT_AXIS 1
#define HORIZ_LEFT -32767
#define HORIZ_RIGHT 32767
#define HORIZ_UP -32767
#define HORIZ_DOWN 32767
//#define JOY_R_BTN 5
//#define JOY_L_BTN 4
//#define JOY_ZR_BTN 7
//#define JOY_ZL_BTN 6

typedef void (*joy_button_func_t)(joy_index_t button) ;
typedef void (*joy_axis_func_t)(size_t sxis_no, short x, short y);
void joy_loop(const char *joypath, joy_button_func_t button_up, joy_button_func_t button_down, joy_axis_func_t axis_func);

#endif /* __TELLO_JOY_H__ */
