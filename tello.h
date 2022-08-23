#ifndef __TELLO_H__
#define __TELLO_H__

#include "utils.h"

typedef struct {
   int tello_port;
   int tello_state_port;
   char *tello_ip;
   float command_timeout;
   bool imperial;
   int video_port;
} tello_config_t;


typedef struct {
  // pitch:0;roll:0;yaw:34;vgx:0;vgy:0;vgz:0;templ:66;temph:68;tof:10;h:0;bat:17;baro:690.97;time:0;agx:-11.00;agy:4.00;agz:-995.00;
  // static const char *tello_state_cmd = "pitch:%d;roll:%d;yaw:%d;vgx:%d;vgy%d;vgz:%d;templ:%d;temph:%d;tof:%d;h:%d;bat:%d;baro:%.2f;time:%d;agx:%.2f;agy:%.2f;agz:%.2f;\r\n";

   int pitch;
   int roll;
   int yaw;
   int vgx, vgy, vgz;
   int templ, temph;
   int tof;
   int height;
   int battery;
   float alt;
   int flight_time;
   float agx, agy, agz;
} tello_state_t;

void tello_init(const char *tello_ip, int tello_port, int video_port, int tello_state_port);
void tello_close(void);
void tello_get_wifi_snr(void);

#ifdef __cplusplus
int video_receive_thread(void *a);
int state_receive_thread(void *a);
int comm_receive_thread(void *a);
int api_activate_again(void *a);
#else
void *video_receive_thread(void *a);
void *state_receive_thread(void *a);
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
