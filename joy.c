#include <linux/joystick.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "joy.h"

struct axis_state {
    short x, y;
};


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




void joy_loop(const char *device, joy_button_func_t button_up, joy_button_func_t button_down)
{
    int js = open(device, O_RDONLY);

     struct js_event event;
    struct axis_state axes[3] = {0};
    size_t axis;

    if (js == -1) {
        perror("Could not open joystick");
	exit(-1);
    }

      while (read_event(js, &event) == 0)
    {
        switch (event.type)
        {
            case JS_EVENT_BUTTON:
                printf("Button %u %s\n", event.number, event.value ? "pressed" : "released");
		if(event.value) {
			// button press
			button_down(event.number);
		} else {
			// button release
			button_up(event.number);
		}
                break;
            case JS_EVENT_AXIS:
                axis = get_axis_state(&event, axes);
                if (axis < 3)
                    printf("Axis %zu at (%6d, %6d)\n", axis, axes[axis].x, axes[axis].y);
                break;
            default:
                /* Ignore init events. */
                break;
        }

        fflush(stdout);
    }

    close(js);
}
