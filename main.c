#include "tello.h"
#include "utils.h"
#include "decoder.h"
#include "sdl.h"
#include "joy.h"
#include "console.h"
#include "wireless.h"
#include <stdio.h>

extern unsigned char *frame_data;
extern int frame_data_width;
extern int frame_data_height;

extern tello_state_t tello_state;


static SDL_Texture *tex = NULL;

#define SCR_W 1920
#define SCR_H 1080

static int font_width = 10;
static int font_height = 20;
static void tello_state_render(void)
{
        char msg[4096];
        snprintf(msg, 256, "ALT: %.2f     H:%d     TEMPL:%d     TEMPH:%d       TIME:%d        TOF:%d        BAT:%d", tello_state.alt, tello_state.height, tello_state.templ, tello_state.temph, tello_state.flight_time, tello_state.tof, tello_state.battery);
        sdl_text(msg, 140, 20, strlen(msg)*font_width, font_height);
        snprintf(msg, 256, "VGX: %d VGY:%d VGZ:%d          AGX:%.2f AGY:%.2f AGZ:%.2f", tello_state.vgx, tello_state.vgy, tello_state.vgz, tello_state.agx, tello_state.agy, tello_state.agz);
        sdl_text(msg, 140, 50, strlen(msg)*font_width, font_height);
        snprintf(msg, 256, "PITCH:%d      ROLL:%d          YAW:%d", tello_state.pitch, tello_state.roll, tello_state.yaw);
        sdl_text(msg, 140, 80, strlen(msg)*font_width, font_height);
}

static void render_func(void)
{
	sdl_battery_status(tello_state.battery);
	sdl_wifi_signal_strength(tello_state.wifi_snr);
	if(frame_data!=NULL && tex!=NULL) {
            sdl_render_texture(tex, frame_data, frame_data_width, frame_data_height, SCR_W, SCR_H);
	}
        tello_state_render();
        console_render();
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
		tello_get_wifi_snr();
		msleep(2000);
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
   if(( key>=SDLK_a && key<=SDLK_z) || (key>=SDLK_0 && key<=SDLK_9) || key==SDLK_SPACE || key==SDLK_MINUS || key==SDLK_PLUS) {
      console_addkeypress(key);
   }
   if(key==SDLK_BACKSPACE) console_backspace();
   if(key==SDLK_RETURN) {
      tello_cmd_t cmd = console_get_command();
      printf("cmd=%d\n", cmd);
      switch(cmd) {
        case TELLO_TAKEOFF:
          tello_takeoff();
          break;
        case TELLO_LAND:
          tello_land();
          break;
        case RENDER_RGB:
          render_rgb();
          break;
        case RENDER_BW:
          render_bw();
          break;
        case RENDER_GREEN:
          render_green();
          break;
        case RENDER_ENHANCE:
          render_enhance();
          break;
        case RENDER_NIGHT:
          render_night();
          break;
        case TELLO_STREAM_OFF:
          tello_stream_off();
          break;
        case TELLO_STREAM_ON:
          tello_stream_on();
          break;
        case TELLO_UP:
	  tello_move("up", 0.2f);
          break;
        case TELLO_DOWN:
	  tello_move("down", 0.2f);
          break;
        case TELLO_LEFT:
	  tello_move("left", 0.2f);
          break;
        case TELLO_RIGHT:
	  tello_move("right", 0.2f);
          break;
        case TELLO_FORWARD:
	  tello_move("forward", 0.2f);
          break;
        case TELLO_BACKWARD:
	  tello_move("back", 0.2f);
          break;
        case TELLO_CCW:
	  tello_rotate_ccw(90);
          break;
        case TELLO_CW:
	  tello_rotate_cw(90);
          break;
        default:
          fprintf(stderr, "not supported yet");
      }
   }
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
  //tello_init((const char *)"192.168.10.1", 8889, 11111, 8890);
  tello_init((const char *)"127.0.0.1", 8889, 11111, 8890);
  tello_stream_on();
  thread_start(handle_joy, NULL);
  thread_start(handle_stats, NULL);
  sdl_setup("Tello controll", SCR_W, SCR_H);
  tex = create_texture(960, 720);
//	tello_close();

  sdl_update(render_func, key_down, key_up);
}
