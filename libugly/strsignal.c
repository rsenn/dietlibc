#define _GNU_SOURCE
#include <string.h>
#include <signal.h>

const char* strsignal(int sig) {
  if (sig<=SIGUNUSED) return sys_siglist[sig];
  else if ((sig>=SIGRTMIN)&&(sig<=SIGRTMAX))
    return "Real time signal";
  else
    return "(unknown signal)";
}
