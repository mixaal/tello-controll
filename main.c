#include "tello.h"
#include "utils.h"
#include "decoder.h"
#include "sdl.h"
#include "joy.h"
#include "wireless.h"
#include <stdio.h>

extern unsigned char *frame_data;
extern int frame_data_width;
extern int frame_data_height;
extern int tello_battery_power;

static SDL_Texture *tex = NULL;

#define SCR_W 1920
#define SCR_H 1080

static void render_func(void)
{
	sdl_battery_status(tello_battery_power);
	sdl_wifi_signal_strength(-65);
	if(frame_data!=NULL && tex!=NULL) {
            sdl_render_texture(tex, frame_data, frame_data_width, frame_data_height, SCR_W, SCR_H);
	}
}

static void joy_button_down(joy_index_t button)
{
	switch(button) {
		case JOY_START_BTN:
			printf("start pressed\n");
			tello_takeoff();
			break;
		case JOY_SELECT_BTN:
			printf("select pressed\n");
			tello_land();
			break;
		case JOY_A_BTN:
			printf("a pressed\n");
			tello_rotate_cw(10);
			break;
		case JOY_B_BTN:
			printf("b pressed\n");
			break;
		case JOY_X_BTN:
			printf("x pressed\n");
			break;
		case JOY_Y_BTN:
			printf("y pressed\n");
			tello_rotate_ccw(10);
			break;
		case JOY_R_BTN:
			printf("r pressed\n");
			tello_move((const char *)"forward", 0.7);
			break;
		case JOY_L_BTN:
			printf("l pressed\n");
			tello_move((const char *)"back", 0.7);
			break;
		case JOY_ZR_BTN:
			printf("zr pressed\n");
			break;
		case JOY_ZL_BTN:
			printf("zl pressed\n");
			break;
		case JOY_UNKNOWN_BTN:
			printf("Unknown\n");
			break;
	}

}


static void joy_button_up(joy_index_t button)
{
	switch(button) {
		case JOY_START_BTN:
			printf("start release\n");
			break;
		case JOY_SELECT_BTN:
			printf("select release\n");
			break;
		case JOY_A_BTN:
			printf("a release\n");
			break;
		case JOY_B_BTN:
			printf("b release\n");
			break;
		case JOY_X_BTN:
			printf("x release\n");
			break;
		case JOY_Y_BTN:
			printf("y release\n");
			break;
		case JOY_R_BTN:
			printf("r release\n");
			break;
		case JOY_L_BTN:
			printf("l release\n");
			break;
		case JOY_ZR_BTN:
			printf("zr release\n");
			break;
		case JOY_ZL_BTN:
			printf("zl release\n");
			break;
		case JOY_UNKNOWN_BTN:
			printf("Unknown\n");
			break;
	}


}

static void joy_axes(size_t axes, short x, short y)
{
}


#if defined( __cplusplus) && defined(HAVE_THREADS_H)
static int handle_joy(void *arg)
#else
static void *handle_joy(void *arg)
#endif
{
  joy_loop("/dev/input/js0", joy_button_down, joy_button_up, joy_axes);	

#if defined( __cplusplus) && defined(HAVE_THREADS_H)
  return 0;
#else
  return NULL;
#endif
}


#if defined( __cplusplus) && defined(HAVE_THREADS_H)
static int handle_stats(void *arg)
#else
static void *handle_stats(void *arg)
#endif
{

	for(;;) {
		tello_get_battery();
		msleep(10000);
		tello_get_height();
		msleep(10000);
	        tello_flight_time();
		msleep(10000);
	}


#if defined( __cplusplus) && defined(HAVE_THREADS_H)
  return 0;
#else
  return NULL;
#endif
}

static void key_up(SDL_Keycode key)
{
}

static void key_down(SDL_Keycode key) 
{

}
int main(int argc, char **argv)
{
//  wireless_scan_all("wlxf4ec38895f5a");
  //
 // int wifi_socket = wifi_find();
 // if(wifi_socket<0) {
//	  fprintf(stderr, "can't find wifi interface\n");
//	  exit(-1);
  //}
  decoder_init();
  tello_init((const char *)"192.168.10.1", 8889, 11111);
  tello_stream_on();
  thread_start(handle_joy, NULL);
  thread_start(handle_stats, NULL);
  sdl_setup("Tello controll", SCR_W, SCR_H);
  tex = create_texture(960, 720);
//	tello_close();

  sdl_update(render_func, key_down, key_up);
}
