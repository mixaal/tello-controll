#include <sys/types.h>         
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>
#include "tello.h"
#include "utils.h"
#include "decoder.h"

//#define DUMP_VIDEO 1

static tello_config_t tello_config = { 8889, 8890, (char*)"192.168.10.1", 0.3f, 0, 11111 };
static const char *tello_state_cmd = "pitch:%d;roll:%d;yaw:%d;vgx:%d;vgy:%d;vgz:%d;templ:%d;temph:%d;tof:%d;h:%d;bat:%d;baro:%f;time:%d;agx:%f;agy:%f;agz:%f;\r\n";

static int comm_socket;
static int video_socket;
static int state_socket;
static struct sockaddr_in comm_addr;
static struct sockaddr_in video_recv__addr;
static struct sockaddr_in state_addr;


tello_state_t tello_state;




#define MAX_VIDEO_SZ 262144
static char frame_data[MAX_VIDEO_SZ];

static void send_command(const char *cmd)
{
        struct sockaddr_in snd_addr;
	memset(&snd_addr, 0, sizeof(snd_addr));
	fprintf(stderr, "Sending to %s:%d...\n", tello_config.tello_ip, tello_config.tello_port);
        snd_addr.sin_family = AF_INET;
        snd_addr.sin_port = htons(tello_config.tello_port);
        inet_aton(tello_config.tello_ip, &snd_addr.sin_addr);

	fprintf(stderr, ">> send cmd: %s\n", cmd);
	sendto(comm_socket, cmd, strlen(cmd), 0, (struct sockaddr *) &snd_addr, sizeof(snd_addr));
}

void tello_init(const char *tello_ip, int tello_port, int video_port, int tello_state_port)
{
  const char *local_ip=(const char *)"0.0.0.0";
  tello_config.tello_ip = new_string(tello_ip);
  tello_config.tello_port = tello_port;
  tello_config.video_port = video_port;
  tello_config.tello_state_port = tello_state_port;
  comm_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if(comm_socket<0) {
	 perror("socket()");
	 exit(-1);
  }
  state_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if(state_socket<0) {
	 perror("socket()");
	 exit(-1);
  }
  video_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if(video_socket<0) {
	 perror("socket()");
	 exit(-1);
  }
  comm_addr.sin_family = AF_INET;
  comm_addr.sin_port = htons(tello_port);
  inet_aton(local_ip, &comm_addr.sin_addr);
  fprintf(stderr, "bind socket to %s:%d ...\n", local_ip, tello_port);
  int rc = bind(comm_socket, (struct sockaddr *) &comm_addr, sizeof(comm_addr));
  if(rc<0) {
	  perror("bind()");
	  close(comm_socket);
	  exit(-1);
  }
  state_addr.sin_family = AF_INET;
  state_addr.sin_port = htons(tello_state_port);
  inet_aton(local_ip, &comm_addr.sin_addr);
  fprintf(stderr, "bind socket to %s:%d ...\n", local_ip, tello_state_port);
  rc = bind(state_socket, (struct sockaddr *) &state_addr, sizeof(state_addr));
  if(rc<0) {
	  perror("bind()");
	  close(comm_socket);
	  close(state_socket);
	  exit(-1);
  }



  video_recv__addr.sin_family = AF_INET;
  video_recv__addr.sin_port = htons(video_port);
  inet_aton(local_ip, &video_recv__addr.sin_addr);
  fprintf(stderr, "bind socket to %s:%d ...\n", local_ip, video_port);
  rc = bind(video_socket, (struct sockaddr *) &video_recv__addr, sizeof(video_recv__addr));
  if(rc<0) {
	  close(video_socket);
	  close(state_socket);
	  close(comm_socket);
	  perror("bind()");
	  exit(-1);
  }

  thread_start(comm_receive_thread, NULL);
  thread_start(state_receive_thread, NULL);
  thread_start(video_receive_thread, NULL);
  thread_start(api_activate_again, NULL);
  send_command("command");
}


#if defined( __cplusplus) && defined(HAVE_THREADS_H)
int api_activate_again(void *a)
#else
void * api_activate_again(void *a)
#endif /* __cplusplus */
{
	for(;;) {
		send_command("command");
		msleep(5000);
	}
}


#if 0
static void append_file(const char *filename, char *buf, size_t nbytes)
{
	FILE *fp = fopen(filename, "a");
	if(fp==NULL) {
		perror("fopen()");
		return;
	}
	size_t total =0;
	for(;total<nbytes;) {
		size_t nwritten=fwrite(buf, 1, nbytes, fp);
		total+=nwritten;
	}
	fclose(fp);
}
#endif


#if defined( __cplusplus) && defined(HAVE_THREADS_H)
int state_receive_thread(void *a)
#else
void * state_receive_thread(void *a)
#endif /* __cplusplus */
{

	char buf[3000];
	fprintf(stderr, "Starting state receive thread from %s:%d...\n", tello_config.tello_ip, tello_config.tello_state_port);

	for(;;) {
		ssize_t nbytes = recv(state_socket, buf, 3000, 0);
		if(nbytes<0) {
			perror("state recv():");
		}
		fprintf(stderr, "state: nbytes=%ld\n", nbytes);
		if(nbytes>0) {
			// Decode stuff
			fprintf(stderr, "state: %ld bytes of communication\n", nbytes);
			char *response = new_string_sz(buf, nbytes);
			chomp(response);
			printf("*** STATE: %s\n", response);
                        sscanf(response, tello_state_cmd, 
                                   &(tello_state.pitch),
                                   &(tello_state.roll),
                                   &(tello_state.yaw),
                                   &(tello_state.vgx),
                                   &(tello_state.vgy),
                                   &(tello_state.vgz),
                                   &(tello_state.templ),
                                   &(tello_state.temph),
                                   &(tello_state.tof),
                                   &(tello_state.height),
                                   &(tello_state.battery),
                                   &(tello_state.alt),
                                   &(tello_state.flight_time),
                                   &(tello_state.agx),
                                   &(tello_state.agy),
                                   &(tello_state.agz));
			fprintf(stderr, "state: |%s|\n", response);
                        fprintf(stderr, "bat: %d\n", tello_state.battery);
			free(response);
		}

	}


#if defined( __cplusplus) && defined(HAVE_THREADS_H)
return 0;
#else
return NULL;
#endif /* __cplusplus */
}


#ifdef DUMP_VIDEO
static void dump_bytes(char *buffer, ssize_t len) 
{
  FILE *fp = fopen("dump.data", "ab");
  if(fp!=NULL) {
     fwrite(&len, sizeof(ssize_t), 1, fp);
     fwrite(buffer, 1, len, fp);
     fclose(fp);
  }
}
#endif


#if defined( __cplusplus) && defined(HAVE_THREADS_H)
int video_receive_thread(void *a)
#else
void * video_receive_thread(void *a)
#endif /* __cplusplus */
{
	char buf[2048];
	fprintf(stderr, "Starting video receive thread from %s:%d...\n", tello_config.tello_ip, tello_config.video_port);
	size_t video_len = 0;

	for(int counter=0;;counter++) {
		ssize_t nbytes = recv(video_socket, buf, 2048, 0);
		if(nbytes<0) {
			perror("video recv():");
		}
		//fprintf(stderr, "video: nbytes=%ld\n", nbytes);
		if(nbytes>=0) {

#ifdef DUMP_VIDEO
                       dump_bytes(buf, nbytes);
#endif /* DUMP_VIDEO */
			memcpy(frame_data + video_len, buf, nbytes);
			video_len+=nbytes;
			if(nbytes!=1460) {
				if(video_len>0) {
  				    //fprintf(stderr, "Decode %lu of data\n", video_len);
				   decode(frame_data, video_len);
				}
				video_len = 0;
			}
			// Decode stuff
			//fprintf(stderr, "video: %ld bytes of video\n", nbytes);

		}
	}
#if defined( __cplusplus) && defined(HAVE_THREADS_H)
	return 0;
#else
	return NULL;
#endif /* __cplusplus */
}
 
#if defined( __cplusplus) && defined(HAVE_THREADS_H)
int comm_receive_thread(void *a)
#else
void * comm_receive_thread(void *a)
#endif /* __cplusplus */
{
	char buf[3000];
	fprintf(stderr, "Starting comm receive thread from %s:%d...\n", tello_config.tello_ip, tello_config.tello_port);

	for(;;) {
		ssize_t nbytes = recv(comm_socket, buf, 3000, 0);
		if(nbytes<0) {
			perror("comm recv():");
		}
		fprintf(stderr, "comm: nbytes=%ld\n", nbytes);
		if(nbytes>0) {
			// Decode stuff
			fprintf(stderr, "received: %ld bytes of communication\n", nbytes);
			char *response = new_string_sz(buf, nbytes);
			chomp(response);
#if 0
			if(endswith(response, "s")) {
                                sscanf(response, "%ds", &tello_fly_time);
				printf("Flight time: %s\n", response);
			}	
			if(endswith(response, "dm")) {
                                sscanf(response, "%ddm", &tello_height);
				printf("Height: %s\n", response);
			}
#endif
			if(only_digits(response)) {
				printf("SNR value: %s\n", response);
				tello_state.wifi_snr = atoi(response);
			}
			fprintf(stderr, "received: |%s|\n", response);
			free(response);
		}

	}
#if defined( __cplusplus) && defined(HAVE_THREADS_H)
	return 0;
#else
	return NULL;
#endif /* __cplusplus */
}




void tello_stream_on(void)
{
	send_command("streamon");
}


void tello_stream_off(void)
{
	send_command("streamoff");
}

void tello_close(void)
{
	close(comm_socket);
	close(video_socket);
}


void tello_takeoff(void)
{
	send_command("takeoff");
}


void tello_set_speed(float speed)
{
        speed *= (tello_config.imperial) ? 44.704f : 27.7778f;
	char *cmd = format("speed %f", speed);
	send_command(cmd);
	free(cmd);
}


void tello_rotate_cw(int degrees)
{
     if(degrees==0) return;
     degrees %= 360;
     while(degrees<0) degrees+=360;
     char *cmd = format("cw %d", degrees);
     send_command(cmd);
     free(cmd);
}

void tello_rotate_ccw(int degrees)
{
     if(degrees==0) return;
     degrees %= 360;
     while(degrees<0) degrees+=360;
     char *cmd = format("ccw %d", degrees);
     send_command(cmd);
     free(cmd);

}

void tello_flip(char direction) // 'l', 'r', 'f', 'b'
{
     char *cmd = format("flip %c", direction);
     send_command(cmd);
     free(cmd);
}


void tello_get_height(void)
{
    send_command("height?");
}

void tello_get_battery(void)
{
    send_command("battery?");
}

void tello_get_wifi_snr(void)
{
    send_command("wifi?");
}


void tello_flight_time(void)
{
    send_command("time?");
}

void tello_get_speed(void)
{
    send_command("speed?");
}

void tello_land(void)
{
    send_command("land");
}


void tello_move(const char *direction, float distance)
{

        /**
	 * Moves in a direction for a distance.

        This method expects meters or feet. The Tello API expects distances
        from 20 to 500 centimeters.

        Metric: .02 to 5 meters
        Imperial: .7 to 16.4 feet

        Args:
            direction (str): Direction to move, 'forward', 'back', 'right' or 'left', 'down', 'up'.
            distance (int|float): Distance to move.

        Returns:
            str: Response from Tello, 'OK' or 'FALSE'.

        */
	int d;
	if(tello_config.imperial) {
		if(distance<0.7f || distance>16.4f) {
			fprintf(stderr, "imperial mode: 0.7 - 16.4ft expected: out of range: %f\n", distance);
			return;
		}
		d = (int)floor((distance * 30.48f));
	} else {
		if(distance<0.02f || distance>5.0f) {
			fprintf(stderr, "metric mode: 0.02 - 5m expected: out of range: %f\n", distance);
			return;
		}
		d = (int)floor((distance * 100.0f));
	}
	char *cmd = format("%s %d", direction, d);
	send_command(cmd);
	free(cmd);
}
