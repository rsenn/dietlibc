#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>

extern int __sc_nr_cpus();

long sysconf(int name)
{
  switch(name)
  {
  case _SC_OPEN_MAX:
    {
      struct rlimit limit;
      getrlimit(RLIMIT_NOFILE, &limit);
      return limit.rlim_cur;
    }
  case _SC_CLK_TCK:
#ifdef __alpha__
    return 1024;
#else
    return 100;
#endif

  case _SC_NPROCESSORS_ONLN:
    return __sc_nr_cpus();

  }
  return -1;
}
