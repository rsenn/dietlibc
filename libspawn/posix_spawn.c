#define _GNU_SOURCE
#include <spawn.h>
#include <sched.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "syscall.h"
//#include "lock.h"
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include "fdop.h"

#define SIGALL_SET ((sigset_t *)(const unsigned long long [2]){ -1,-1 })

struct args {
	int p[2];
	sigset_t oldmask;
	const char *path;
	const posix_spawn_file_actions_t *fa;
	const posix_spawnattr_t *restrict attr;
	char *const *argv, *const *envp;
};

static int child(void *args_vp)
{
	int i, ret;
	struct sigaction sa = {0};
	struct args *args = args_vp;
	int p = args->p[1];
	const posix_spawn_file_actions_t *fa = args->fa;
	const posix_spawnattr_t *restrict attr = args->attr;
	sigset_t hset;

	close(args->p[0]);

	/* All signal dispositions must be either SIG_DFL or SIG_IGN
	 * before signals are unblocked. Otherwise a signal handler
	 * from the parent might get run in the child while sharing
	 * memory, with unpredictable and dangerous results. To
	 * reduce overhead, sigaction has tracked for us which signals
	 * potentially have a signal handler. */
	//__get_handler_set(&hset);
	for (i=1; i<_NSIG; i++) {
		if ((attr->__flags & POSIX_SPAWN_SETSIGDEF)
		     && sigismember(&attr->__def, i)) {
			sa.sa_handler = SIG_DFL;
		} else if (sigismember(&hset, i)) {
			if (i-32<3U) {
				sa.sa_handler = SIG_IGN;
			} else {
				sigaction(i, 0, &sa);
				if (sa.sa_handler==SIG_IGN) continue;
				sa.sa_handler = SIG_DFL;
			}
		} else {
			continue;
		}
		sigaction(i, &sa, 0);
	}

	if (attr->__flags & POSIX_SPAWN_SETSID)
		if ((ret=setsid()) < 0)
			goto fail;

	if (attr->__flags & POSIX_SPAWN_SETPGROUP)
		if ((ret=setpgid(0,attr->__pgrp)))
			goto fail;

	/* Use syscalls directly because the library functions attempt
	 * to do a multi-threaded synchronized id-change, which would
	 * trash the parent's state. */
	if (attr->__flags & POSIX_SPAWN_RESETIDS)
		if ((ret=setgid(getgid())) ||
		    (ret=setuid(getuid())) )
			goto fail;

	if (fa && fa->__actions) {
		struct fdop *op;
		int fd;
		for (op = fa->__actions; op->next; op = op->next);
		for (; op; op = op->prev) {
			/* It's possible that a file operation would clobber
			 * the pipe fd used for synchronizing with the
			 * parent. To avoid that, we dup the pipe onto
			 * an unoccupied fd. */
			if (op->fd == p) {
				ret = dup(p);
				if (ret < 0) goto fail;
				close(p);
				p = ret;
			}
			switch(op->cmd) {
			case FDOP_CLOSE:
				close(op->fd);
				break;
			case FDOP_DUP2:
				fd = op->srcfd;
				if (fd == p) {
					ret = -EBADF;
					goto fail;
				}
				if (fd != op->fd) {
					if ((ret=dup2(fd, op->fd))<0)
						goto fail;
				} else {
					ret = fcntl(fd, F_GETFD);
					ret = fcntl(fd, F_SETFD, ret & ~FD_CLOEXEC);
					if (ret<0)
						goto fail;
				}
				break;
			case FDOP_OPEN:
				fd = open(op->path, op->oflag, op->mode);
				if ((ret=fd) < 0) goto fail;
				if (fd != op->fd) {
					if ((ret=dup2(fd, op->fd))<0)
						goto fail;
					close(fd);
				}
				break;
			case FDOP_CHDIR:
				ret = chdir(op->path);
				if (ret<0) goto fail;
				break;
			case FDOP_FCHDIR:
				ret = fchdir(op->fd);
				if (ret<0) goto fail;
				break;
			}
		}
	}

	/* Close-on-exec flag may have been lost if we moved the pipe
	 * to a different fd. We don't use F_DUPFD_CLOEXEC above because
	 * it would fail on older kernels and atomicity is not needed --
	 * in this process there are no threads or signal handlers. */
	fcntl(p, F_SETFD, FD_CLOEXEC);

	pthread_sigmask(SIG_SETMASK, (attr->__flags & POSIX_SPAWN_SETSIGMASK)
		? &attr->__mask : &args->oldmask, 0);

	int (*exec)(const char *, char *const *, char *const *) =
		attr->__fn ? (int (*)())attr->__fn : execve;

	exec(args->path, args->argv, args->envp);
	ret = -errno;

fail:
	/* Since sizeof errno < PIPE_BUF, the write is atomic. */
	ret = -ret;
	if (ret) while (write(p, &ret, sizeof ret) < 0);
	_exit(127);
}


int posix_spawn(pid_t *restrict res, const char *restrict path,
	const posix_spawn_file_actions_t *fa,
	const posix_spawnattr_t *restrict attr,
	char *const argv[restrict], char *const envp[restrict])
{
	pid_t pid;
	char stack[1024+PATH_MAX];
	int ec=0, cs;
	struct args args;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);

	args.path = path;
	args.fa = fa;
	args.attr = attr ? attr : &(const posix_spawnattr_t){0};
	args.argv = argv;
	args.envp = envp;
	pthread_sigmask(SIG_BLOCK, SIGALL_SET, &args.oldmask);

	/* The lock guards both against seeing a SIGABRT disposition change
	 * by abort and against leaking the pipe fd to fork-without-exec. */
	//LOCK(__abort_lock);

	if (pipe2(args.p, O_CLOEXEC)) {
		//UNLOCK(__abort_lock);
		ec = errno;
		goto fail;
	}

	pid = __clone(child, stack+sizeof stack,
		CLONE_VM|CLONE_VFORK|SIGCHLD, &args);
	close(args.p[1]);
	//UNLOCK(__abort_lock);

	if (pid > 0) {
		if (read(args.p[0], &ec, sizeof ec) != sizeof ec) ec = 0;
		else waitpid(pid, &(int){0}, 0);
	} else {
		ec = -pid;
	}

	close(args.p[0]);

	if (!ec && res) *res = pid;

fail:
	pthread_sigmask(SIG_SETMASK, &args.oldmask, 0);
	pthread_setcancelstate(cs, 0);

	return ec;
}
