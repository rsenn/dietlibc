#include <signal.h>

int __rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, long nr);

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
  return __rt_sigaction(signum, act, oldact, _NSIG/8);
}
