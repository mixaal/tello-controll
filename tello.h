#ifndef __TELLO_H__
#define __TELLO_H__

#include "utils.h"

typedef struct {
   int tello_port;
   char *tello_ip;
   float command_timeout;
   bool imperial;
   int video_port;
} tello_config_t;


void tello_init(const char *tello_ip, int tello_port, int video_port);
void tello_close(void);

#ifdef __cplusplus
int video_receive_thread(void *a);
int comm_receive_thread(void *a);
int api_activate_again(void *a);
#else
void *video_receive_thread(void *a);
void *comm_receive_thread(void *a);
void *api_activate_again(void *a);
#endif /* __cplusplus */

void tello_takeoff(void);
void tello_land(void);
void tello_set_speed(float speed);
void tello_rotate_cw(int degrees);
void tello_rotate_ccw(int degrees);
void tello_flip(char direction); // 'l', 'r', 'f', 'b'
void tello_get_height(void);
void tello_get_battery(void);
void tello_flight_time(void);
void tello_get_speed(void);
void tello_move(const char *direction, float distance);
void tello_stream_on(void);
void tello_stream_off(void);

#endif /* __TELLO_H__ */
