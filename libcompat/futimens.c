#define _ATFILE_SOURCE
#include <sys/stat.h>
#include <sys/time.h>

int futimens(int fd, const struct timespec times[2])
{
	return utimensat(fd, 0, (void*)times, 0);
}
