#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include "dietwarning.h"
#include "dietfeatures.h"

#define SHELL_PATH      "/bin/sh"       /* Path of the shell.  */
#define SHELL_NAME      "sh"            /* Name to give it.  */

extern char **environ;

int __libc_fork(void);
int __libc_waitpid(int pid, int *status, int options);
int execve(const char*filename, char *const argv[], char *const envp[]);
int sigaction(int signum,  const  struct  sigaction  *act, struct sigaction *oldact);

int __libc_system (const char *line)
{
  struct sigaction sa, intr, quit;
  int save,pid,ret=-1;

  if (line == 0) return __libc_system("exit 0") == 0;

  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sigemptyset (&sa.sa_mask);

  if (sigaction(SIGINT,  &sa, &intr)<0) return -1;
  if (sigaction(SIGQUIT, &sa, &quit)<0) {
    save = errno;
    sigaction (SIGINT, &intr, (struct sigaction*)0);
    errno=save;
    return -1;
  }

  pid=__libc_fork();
  if (pid>0)
  { /* parent */
    int n;
    do
      n=__libc_waitpid(pid, &ret, 0);
    while ((n==-1) && (errno==EINTR));
    if (n!=pid) ret=-1;
  }
  else if (!pid)
  { /* child */
    const char *nargs[4];
    nargs[0] = SHELL_NAME;
    nargs[1] = "-c";
    nargs[2] = line;
    nargs[3] = 0;

    sigaction(SIGINT,  &intr, (struct sigaction*)0);
    sigaction(SIGQUIT, &quit, (struct sigaction*)0);

    execve(SHELL_PATH,(char *const *)nargs, environ);
    _exit(127);
  }
  save = errno;
  sigaction (SIGINT,  &intr, (struct sigaction *)0);
  sigaction (SIGQUIT, &quit, (struct sigaction *)0);
  errno=save;
  return ret;
}

int system (const char *line) __attribute__((weak,alias("__libc_system")));

link_warning("system","warning: system() is a security risk.  Use fork and execvp instead!")
