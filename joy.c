#ifdef HAVE_LINUX_JOYSTICK_H_
#include <linux/joystick.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#include "joy.h"

struct axis_state {
    short x, y;
};

#ifdef HAVE_LINUX_JOYSTICK_H_
static const char *joy_name = NULL;

static int gamehat_mapping[] = {11, 10, 0, 1, 3, 4, -1, -1, 6, 7};
static int ps3_android__mapping[] = {9, 8, 1, 2, 0, 3, 5, 4, 7, 6};

// Thanks to : https://gist.github.com/jasonwhite/c5b2048c15993d285130


static int read_event(int fd, struct js_event *event)
{
    ssize_t bytes;

    bytes = read(fd, event, sizeof(*event));

    if (bytes == sizeof(*event))
        return 0;
    printf("bytes=%ld\n", bytes);
    /* Error, could not read full event. */
    return -1;
}

size_t get_axis_state(struct js_event *event, struct axis_state axes[3])
{
    size_t axis = event->number / 2;

    if (axis < 3)
    {
        if (event->number % 2 == 0)
            axes[axis].x = event->value;
        else
            axes[axis].y = event->value;
    }

    return axis;
}


static void joy_mapper(int fd)
{
	char name[128];
	if (ioctl(fd, JSIOCGNAME(sizeof(name)), name) < 0) {
		perror("ioctl()");
		exit(-1);
		//strncpy(name, "Unknown", sizeof(name));
	}
	printf("Name: %s\n", name);
	joy_name = strdup(name);
	if(joy_name==NULL) {
		perror("strdup()");
		exit(-1);
		
	}
}

static joy_index_t joy_get_mapping(int button)
{
	int *arr = ps3_android__mapping;

	if(!strcmp(joy_name, GAME_HAT_JOY_NAME)) {
		arr = gamehat_mapping;
	}

	for(int i=JOY_START_BTN; i<=JOY_ZL_BTN; i++) {
		if(arr[i]==button) {
			return (joy_index_t)i;
		}
	}
	fprintf(stderr, "joy.c: Unknown mapping obtained: %s, %d\n", joy_name, button);
	return JOY_UNKNOWN_BTN;
}


void joy_loop(const char *device, joy_button_func_t button_up, joy_button_func_t button_down, joy_axis_func_t axis_func)
{

     struct js_event event;
    struct axis_state axes[3] = {0};
    size_t axis;
    joy_index_t button;

    int js = open(device, O_RDONLY);
    if (js == -1) {
        perror("Could not open joystick");
	exit(-1);
    }
    joy_mapper(js);

      while (read_event(js, &event) == 0)
    {
        switch (event.type)
        {
            case JS_EVENT_BUTTON:
		button = joy_get_mapping(event.number);
                printf("Button %u %s\n", event.number, event.value ? "pressed" : "released");
		if(event.value) {
			// button press
			button_down(button);
		} else {
			// button release
			button_up(button);
		}
                break;
            case JS_EVENT_AXIS:
                axis = get_axis_state(&event, axes);
                if (axis < 3) {
                    printf("Axis %zu at (%6d, %6d)\n", axis, axes[axis].x, axes[axis].y);
		    axis_func(axis, axes[axis].x, axes[axis].y);
		}
                break;
            default:
                /* Ignore init events. */
                break;
        }

        fflush(stdout);
    }

    close(js);
}
#endif
