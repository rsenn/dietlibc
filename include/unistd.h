#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/cdefs.h>
#include <sys/types.h>
#include <endian.h>
#include <sys/fsuid.h>
#include <sys/select.h>

__BEGIN_DECLS

extern int optind,opterr,optopt;
extern char *optarg;
int getopt(int argc, char *const argv[], const char *options);

/* Values for the second argument to access.
   These may be OR'd together.  */
#define R_OK 4 /* Test for read permission.  */
#define W_OK 2 /* Test for write permission.  */
#define X_OK 1 /* Test for execute permission.  */
#define F_OK 0 /* Test for existence.  */

/* Test for access to NAME using the real UID and real GID.  */
int access (const char *__name, int __type) __THROW;

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#define STDIN_FILENO   0
#define STDOUT_FILENO  1
#define STDERR_FILENO  2

off_t lseek(int fildes, off_t offset, int whence) __THROW;
#if !defined(__OFF_T_MATCHES_OFF64_T)
loff_t lseek64(int fildes, loff_t offset, int whence) __THROW;
#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS == 64
#define lseek(fildes,offset,whence) lseek64(fildes,offset,whence)
#endif
#else
#define lseek64(fildes,offset,whence) lseek(fildes,offset,whence)
#endif

int chdir(const char *path) __THROW;
int fchdir(int fd) __THROW;
int rmdir(const char *pathname) __THROW;
char *getcwd(char *buf, size_t size) __THROW __attribute__((__warn_unused_result__));

#ifdef _GNU_SOURCE
char *get_current_dir_name (void) __THROW __attribute_dontuse__;

int pipe2(int pipefd[2], int flags) __THROW;
#endif

int open(const char* pathname,int flags, ...) __THROW;
int open64(const char* pathname,int flags, ...) __THROW;
int creat(const char* pathname,mode_t mode) __THROW;
int creat64(const char* pathname,mode_t mode) __THROW;
ssize_t write(int fd,const void* buf,size_t len) __THROW;
ssize_t read(int fd,void* buf,size_t len) __THROW;
int close(int fd) __THROW;

int unlink(const char *pathname) __THROW;

ssize_t pread(int fd, void *buf, size_t count, off_t offset);
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);
ssize_t pread64(int fd, void *buf, size_t count, off64_t offset);
ssize_t pwrite64(int fd, const void *buf, size_t count, off64_t offset);

int execve(const char *filename, char *const argv [], char *const envp[]) __THROW;
int execlp(const char *file, const char *arg, ...) __THROW;
int execv(const char *path, char *const argv[]) __THROW;
int execvp(const char *file, char *const argv[]) __THROW;
int execl(const char *path, const char* arg, ...) __THROW;
int execle(const char *path, const char* arg, ...) __THROW;

pid_t getpid(void) __THROW __pure;

pid_t getppid(void) __THROW;

int setpgid (pid_t pid,pid_t pgid) __THROW;
pid_t getpgid (pid_t pid) __THROW;
int setpgrp (void) __THROW;
pid_t getpgrp (void) __THROW;
pid_t getsid(pid_t pid) __THROW;
pid_t setsid (void) __THROW;
int dup (int oldfd) __THROW;
int dup2 (int oldfd,int newfd) __THROW;
#ifdef _GNU_SOURCE
int dup3(int oldfd, int newfd, int flags) __THROW;

/* flags for memfd_create(2) (unsigned int) */
#define MFD_CLOEXEC		0x0001U
#define MFD_ALLOW_SEALING	0x0002U

int memfd_create(const char* name, unsigned int flags) __THROW;

int syncfs(int fd) __THROW;
#endif

struct dirent;
struct dirent64;
int getdents(int fd, struct dirent *dirp, unsigned int count) __THROW;
int getdents64(int fd, struct dirent64 *dirp, unsigned int count) __THROW;

pid_t fork(void) __THROW;
pid_t vfork(void) __THROW;

int readlink(const char *path, char *buf, size_t bufsiz) __THROW;
int symlink(const char *oldpath, const char *newpath) __THROW;
int link(const char *oldpath, const char *newpath) __THROW;

int chown(const char *path, uid_t owner, gid_t group) __THROW;
int fchown(int fd, uid_t owner, gid_t group) __THROW;
int lchown(const char *path, uid_t owner, gid_t group) __THROW;

int fsync(int fd) __THROW;
#define _POSIX_SYNCHRONIZED_IO
int fdatasync(int fd) __THROW;

int pipe(int filedes[2]) __THROW;

char *ttyname (int desc) __THROW;

int brk(void *end_data_segment) __THROW;
void *sbrk(ptrdiff_t increment) __THROW;

int gethostname(char *name, size_t len) __THROW;
int sethostname(const char *name, size_t len) __THROW;

int usleep(unsigned long useconds) __THROW;
unsigned int sleep(unsigned int seconds) __THROW;

unsigned int alarm(unsigned int seconds) __THROW;
int sync(void) __THROW;

int isatty(int desc) __THROW;

void _exit(int status) __THROW __attribute__((__noreturn__));

int daemon(int nochdir,int noclose) __THROW;

int pause(void) __THROW;

char* getlogin(void) __THROW;
/* warning: the diet libc getlogin() simply returns getenv("LOGNAME") */

int chroot(const char *path) __THROW;

uid_t getuid(void) __THROW;
uid_t geteuid(void) __THROW;
gid_t getgid(void) __THROW;
gid_t getegid(void) __THROW;
int setuid(uid_t uid) __THROW;
/* int seteuid(uid_t uid) __THROW; */
int setgid(gid_t gid) __THROW;
/* int setegid(gid_t gid) __THROW; */
int setregid(gid_t rgid, gid_t egid) __THROW;
int setreuid(uid_t ruid, uid_t euid) __THROW;
#define seteuid(euid) setreuid(-1,euid)
#define setegid(egid) setregid(-1,egid)

int truncate(const char *path, off_t length) __THROW;
int ftruncate(int fd, off_t length) __THROW;
#if __WORDSIZE == 32
int truncate64(const char *path, loff_t length) __THROW;
int ftruncate64(int fd, loff_t length) __THROW;
#endif

int nice(int inc) __THROW;

char *crypt(const char *key, const char *salt) __THROW;
void encrypt(char block[64], int edflag) __THROW;
void setkey(const char *key) __THROW;

#ifdef _GNU_SOURCE
char* md5crypt(const char* key, const char* salt) __THROW;
char* sha256_crypt(const char* key, const char* salt) __THROW;
char* sha512_crypt(const char* key, const char* salt) __THROW;
#endif

int getpagesize(void) __THROW __attribute__((__const__));

int getdomainname(char *name, size_t len) __THROW;
int setdomainname(const char *name, size_t len) __THROW;

int getgroups(int size, gid_t list[]) __THROW;
int getdtablesize(void) __THROW;
char *getpass(const char * prompt) __THROW;

/* warning: linux specific: */
int llseek(int fildes, unsigned long hi, unsigned long lo, loff_t* result,int whence) __THROW;

/* include <linux/sysctl.h> to get all the definitions! */
struct __sysctl_args;
int _sysctl(struct __sysctl_args *args) __THROW;

#define _CS_PATH 1
size_t confstr(int name,char*buf,size_t len) __THROW;

#define _SC_CLK_TCK 1
#define _SC_ARG_MAX 2
#define _SC_NGROUPS_MAX 3
#define _SC_OPEN_MAX 4
#define _SC_PAGESIZE 5
#define _SC_NPROCESSORS_ONLN 6
#define _SC_NPROCESSORS_CONF _SC_NPROCESSORS_ONLN
#define _SC_PHYS_PAGES 7
#define _SC_GETPW_R_SIZE_MAX 8
#define _SC_GETGR_R_SIZE_MAX 9
#define _SC_PRIORITY_SCHEDULING 10
#define _SC_TIMERS 11
#define _SC_ASYNCHRONOUS_IO 12
#define _SC_PRIORITIZED_IO 13
#define _SC_SYNCHRONIZED_IO 14
#define _SC_FSYNC 15
#define _SC_MAPPED_FILES 16
#define _SC_MEMLOCK 17
#define _SC_MEMLOCK_RANGE 18
#define _SC_MEMORY_PROTECTION 19
#define _SC_MESSAGE_PASSING 20
#define _SC_SEMAPHORES 21
#define _SC_SHARED_MEMORY_OBJECTS 22
#define _SC_AIO_LISTIO_MAX 23
#define _SC_AIO_MAX 24
#define _SC_AIO_PRIO_DELTA_MAX 25
#define _SC_DELAYTIMER_MAX 26
#define _SC_MQ_OPEN_MAX 27
#define _SC_MQ_PRIO_MAX 28
#define _SC_VERSION 29
#define _SC_PAGE_SIZE 30
#define _SC_PAGESIZE 30
#define _SC_RTSIG_MAX 31
#define _SC_SEM_NSEMS_MAX 32
#define _SC_SEM_VALUE_MAX 33
#define _SC_SIGQUEUE_MAX 34
#define _SC_TIMER_MAX 35
#define _SC_BC_BASE_MAX 36
#define _SC_BC_DIM_MAX 37
#define _SC_BC_SCALE_MAX 38
#define _SC_BC_STRING_MAX 39
#define _SC_COLL_WEIGHTS_MAX 40
#define _SC_EXPR_NEST_MAX 42
#define _SC_LINE_MAX 43
#define _SC_RE_DUP_MAX 44
#define _SC_2_VERSION 46
#define _SC_2_C_BIND 47
#define _SC_2_C_DEV 48
#define _SC_2_FORT_DEV 49
#define _SC_2_FORT_RUN 50
#define _SC_2_SW_DEV 51
#define _SC_2_LOCALEDEF 52
#define _SC_UIO_MAXIOV 60
#define _SC_IOV_MAX 60
#define _SC_THREADS 67
#define _SC_THREAD_SAFE_FUNCTIONS 68
#define _SC_GETGR_R_SIZE_MAX 69
#define _SC_GETPW_R_SIZE_MAX 70
#define _SC_LOGIN_NAME_MAX 71
#define _SC_TTY_NAME_MAX 72
#define _SC_THREAD_DESTRUCTOR_ITERATIONS 73
#define _SC_THREAD_KEYS_MAX 74
#define _SC_THREAD_STACK_MIN 75
#define _SC_THREAD_THREADS_MAX 76
#define _SC_THREAD_ATTR_STACKADDR 77
#define _SC_THREAD_ATTR_STACKSIZE 78
#define _SC_THREAD_PRIORITY_SCHEDULING 79
#define _SC_THREAD_PRIO_INHERIT 80
#define _SC_THREAD_PRIO_PROTECT 81
#define _SC_THREAD_PROCESS_SHARED 82
#define _SC_NPROCESSORS_CONF 83
#define _SC_NPROCESSORS_ONLN 84
#define _SC_PHYS_PAGES 85
#define _SC_AVPHYS_PAGES 86
#define _SC_ATEXIT_MAX 87
#define _SC_PASS_MAX 88
#define _SC_XOPEN_VERSION 89
#define _SC_XOPEN_XCU_VERSION 90
#define _SC_XOPEN_UNIX 91
#define _SC_XOPEN_CRYPT 92
#define _SC_XOPEN_ENH_I18N 93
#define _SC_XOPEN_SHM 94
#define _SC_2_CHAR_TERM 95
#define _SC_2_UPE 97
#define _SC_XOPEN_XPG2 98
#define _SC_XOPEN_XPG3 99
#define _SC_XOPEN_XPG4 100
#define _SC_NZERO 109
#define _SC_XBS5_ILP32_OFF32 125
#define _SC_XBS5_ILP32_OFFBIG 126
#define _SC_XBS5_LP64_OFF64 127
#define _SC_XBS5_LPBIG_OFFBIG 128
#define _SC_XOPEN_LEGACY 129
#define _SC_XOPEN_REALTIME 130
#define _SC_XOPEN_REALTIME_THREADS 131
#define _SC_ADVISORY_INFO 132
#define _SC_BARRIERS 133
#define _SC_CLOCK_SELECTION 137
#define _SC_CPUTIME 138
#define _SC_THREAD_CPUTIME 139
#define _SC_MONOTONIC_CLOCK 149
#define _SC_READER_WRITER_LOCKS 153
#define _SC_SPIN_LOCKS 154
#define _SC_REGEXP 155
#define _SC_SHELL 157
#define _SC_SPAWN 159
#define _SC_SPORADIC_SERVER 160
#define _SC_THREAD_SPORADIC_SERVER 161
#define _SC_TIMEOUTS 164
#define _SC_TYPED_MEMORY_OBJECTS 165
#define _SC_2_PBS 168
#define _SC_2_PBS_ACCOUNTING 169
#define _SC_2_PBS_LOCATE 170
#define _SC_2_PBS_MESSAGE 171
#define _SC_2_PBS_TRACK 172
#define _SC_SYMLOOP_MAX 173
#define _SC_STREAMS 174
#define _SC_2_PBS_CHECKPOINT 175
#define _SC_V6_ILP32_OFF32 176
#define _SC_V6_ILP32_OFFBIG 177
#define _SC_V6_LP64_OFF64 178
#define _SC_V6_LPBIG_OFFBIG 179
#define _SC_HOST_NAME_MAX 180
#define _SC_TRACE 181
#define _SC_TRACE_EVENT_FILTER 182
#define _SC_TRACE_INHERIT 183
#define _SC_TRACE_LOG 184
#define _SC_IPV6 235
#define _SC_RAW_SOCKETS 236
#define _SC_V7_ILP32_OFF32 237
#define _SC_V7_ILP32_OFFBIG 238
#define _SC_V7_LP64_OFF64 239
#define _SC_V7_LPBIG_OFFBIG 240
#define _SC_SS_REPL_MAX 241
#define _SC_TRACE_EVENT_NAME_MAX 242
#define _SC_TRACE_NAME_MAX 243
#define _SC_TRACE_SYS_MAX 244
#define _SC_TRACE_USER_EVENT_MAX 245
#define _SC_XOPEN_STREAMS 246
#define _SC_THREAD_ROBUST_PRIO_INHERIT 247
#define _SC_THREAD_ROBUST_PRIO_PROTECT 248

long sysconf(int name) __THROW;
#define _PC_PATH_MAX 1
#define _PC_VDISABLE 2

pid_t tcgetpgrp(int fd) __THROW;
int tcsetpgrp(int fd, pid_t pgrpid) __THROW;

int profil(unsigned short *buf, size_t bufsiz, size_t offset, unsigned int scale);

/* Linux only: */
int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid) __THROW;
int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid) __THROW;
int setresuid(uid_t ruid, uid_t euid, uid_t suid) __THROW;
int setresgid(gid_t rgid, gid_t egid, gid_t sgid) __THROW;

/* 32-bit uid support */
int chown32(const char *path, uid32_t owner, gid32_t group) __THROW;
int fchown32(int fd, uid32_t owner, gid32_t group) __THROW;
int lchown32(const char *path, uid32_t owner, gid32_t group) __THROW;
uid32_t getuid32(void) __THROW;
uid32_t geteuid32(void) __THROW;
gid32_t getgid32(void) __THROW;
gid32_t getegid32(void) __THROW;
int setuid32(uid32_t uid) __THROW;
int setgid32(gid32_t gid) __THROW;
int setreuid32(uid32_t ruid, uid32_t euid) __THROW;
int setregid32(gid32_t rgid, gid32_t egid) __THROW;
#define seteuid32(euid) setreuid32(-1,euid)
#define setegid32(egid) setregid32(-1,egid)
int getgroups32(int size, gid32_t list[]) __THROW;
int getresuid32(uid32_t *ruid, uid32_t *euid, uid32_t *suid);
int getresgid32(gid32_t *rgid, gid32_t *egid, gid32_t *sgid);
int setresuid32(uid32_t ruid, uid32_t euid, uid32_t suid) __THROW;
int setresgid32(gid32_t rgid, gid32_t egid, gid32_t sgid) __THROW;

#ifdef _BSD_SOURCE
char *getusershell(void) __attribute_dontuse__;
void setusershell(void) __attribute_dontuse__;
void endusershell(void) __attribute_dontuse__;
#endif

/* this is so bad, we moved it to -lcompat */
#define   L_cuserid   17
char* cuserid(char * string); /* ugh! */

#define   _POSIX_VERSION  199506L

#define F_ULOCK 0	/* Unlock a previously locked region.  */
#define F_LOCK  1	/* Lock a region for exclusive use.  */
#define F_TLOCK 2	/* Test and lock a region for exclusive use.  */
#define F_TEST  3	/* Test a region for other processes locks.  */

int lockf (int __fd, int __cmd, off_t __len) __THROW;
int lockf64 (int __fd, int __cmd, off64_t __len) __THROW;

void swab(const void *src, void *dest, ssize_t nbytes) __THROW;

int vhangup(void) __THROW;

extern char **__environ;

#if __WORDSIZE == 32
#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS == 64
#define open open64
#define creat creat64
#define truncate truncate64
#define ftruncate ftruncate64
#define getdents getdents64
#endif
#endif

#if defined(_LINUX_SOURCE)
int pivot_root(const char *new_root, const char *put_old) __THROW;
/* Linux 2.6 module loading infrastructure:
 * init_module takes a buffer where you read the module file into */
long init_module(void *module, unsigned long len, const char *options) __THROW;
/* flags can be O_EXCL | O_NONBLOCK | O_TRUNC (forced unloading)
 * O_EXCL is there so the kernel can spot old rmmod versions */
long delete_module(const char* name,unsigned int flags) __THROW;
pid_t gettid(void) __THROW __pure;
int tkill(pid_t tid, int sig) __THROW;
int tgkill(pid_t tgid, pid_t tid, int sig) __THROW;
/* see linux/fadvise.h */
long fadvise64(int fd,off64_t offset,size_t len,int advice) __THROW;
long fadvise64_64(int fd,off64_t offset,off64_t len,int advice) __THROW;

#endif

#if defined(_ATFILE_SOURCE) || ((_XOPEN_SOURCE + 0) >= 700) || ((_POSIX_C_SOURCE + 0) >= 200809L)
/* also include fcntl.h for the AT_* constants */

int faccessat(int dirfd, const char *pathname, int mode, int flags) __THROW;
int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) __THROW;
int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags) __THROW;
int readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz) __THROW;
#endif

__END_DECLS

#endif
