#include <signal.h>

int __libc_sigsuspend(const sigset_t *mask) {
  return __rt_sigsuspend(mask, _NSIG/8);
}

int sigsuspend(const sigset_t *mask) __attribute__((weak,alias("__libc_sigsuspend")));
