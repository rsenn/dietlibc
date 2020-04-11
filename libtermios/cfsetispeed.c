#define _BSD_SOURCE
#include <termios.h>
#include <sys/ioctl.h>
#include <errno.h>

int cfsetispeed(struct termios *tio, speed_t speed)
{
	return speed ? cfsetospeed(tio, speed) : 0;
}

