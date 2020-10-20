#include <termios.h>
#include <sys/ioctl.h>
#include <errno.h>

__THROW int tcsetattr(int fd, int act, struct termios *tio) {
	if (act < 0 || act > 2) {
		errno = EINVAL;
		return -1;
	}
	return ioctl(fd, TCSETS+act, tio);
}
