#include <unistd.h>

#include <pthread.h>
#include "thread_internal.h"

#include <termios.h>
#include <sys/ioctl.h>

int tcdrain(int fd) {
  __TEST_CANCEL();
	return ioctl(fd, TCSBRK, 1);
}
