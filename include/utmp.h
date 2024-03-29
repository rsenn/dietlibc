#ifndef _UTMP_H
#define _UTMP_H

#include <sys/cdefs.h>
#include <sys/types.h>
#include <time.h>
#include <paths.h>

__BEGIN_DECLS

#define UT_LINESIZE	32
#define UT_NAMESIZE	32
#define UT_HOSTSIZE	256

/* The structure describing an entry in the database of
   previous logins.  */
struct lastlog
  {
#if __WORDSIZE == 64 && defined __WORDSIZE_COMPAT32
    int32_t ll_time;
#else
    time_t ll_time;
#endif
    char ll_line[UT_LINESIZE];
    char ll_host[UT_HOSTSIZE];
  };

/* The structure describing the status of a terminated process.  This
   type is used in `struct utmp' below.  */
struct exit_status
  {
    short int e_termination;	/* Process termination status.  */
    short int e_exit;		/* Process exit status.  */
  };

/* The structure describing an entry in the user accounting database.  */
struct utmp
{
  short int ut_type;		/* Type of login.  */
  pid_t ut_pid;			/* Process ID of login process.  */
  char ut_line[UT_LINESIZE];	/* Devicename.  */
  char ut_id[4];		/* Inittab ID.  */
  char ut_user[UT_NAMESIZE];	/* Username.  */
  char ut_host[UT_HOSTSIZE];	/* Hostname for remote login.  */
#define ut_name ut_user
  struct exit_status ut_exit;	/* Exit status of a process marked
				   as DEAD_PROCESS.  */
/* The ut_session and ut_tv fields must be the same size when compiled
   32- and 64-bit.  This allows data files and shared memory to be
   shared between 32- and 64-bit applications.  */
#if __WORDSIZE == 64 && defined __WORDSIZE_COMPAT32
  int32_t ut_session;		/* Session ID, used for windowing.  */
  struct
  {
    int32_t tv_sec;		/* Seconds.  */
    int32_t tv_usec;		/* Microseconds.  */
  } ut_tv;			/* Time entry was made.  */
#else
  long int ut_session;		/* Session ID, used for windowing.  */
  struct timeval ut_tv;		/* Time entry was made.  */
#endif
#define ut_time ut_tv.tv_sec
#define ut_addr ut_addr_v6[0]
  int32_t ut_addr_v6[4];	/* Internet address of remote host.  */
  char __unused[20];		/* Reserved for future use.  */
};

#ifndef _UTMPX_H		/* untmpx.h hasn't already defined these. */
/* Values for the `ut_type' field of a `struct utmp'.  */
#define EMPTY		0	/* No valid user accounting information.  */

#define RUN_LVL		1	/* The system's runlevel.  */
#define BOOT_TIME	2	/* Time of system boot.  */
#define NEW_TIME	3	/* Time after system clock changed.  */
#define OLD_TIME	4	/* Time when system clock changed.  */

#define INIT_PROCESS	5	/* Process spawned by the init process.  */
#define LOGIN_PROCESS	6	/* Session leader of a logged in user.  */
#define USER_PROCESS	7	/* Normal process.  */
#define DEAD_PROCESS	8	/* Terminated process.  */

#define ACCOUNTING	9
#endif

/* Tell the user that we have a modern system with UT_HOST, UT_PID,
   UT_TYPE, UT_ID and UT_TV fields.  */
#define _HAVE_UT_TYPE	1
#define _HAVE_UT_PID	1
#define _HAVE_UT_ID	1
#define _HAVE_UT_TV	1
#define _HAVE_UT_HOST	1

struct utmp *getutent(void) __THROW;
struct utmp *getutid(struct utmp *ut) __THROW;
struct utmp *getutline(struct utmp *ut) __THROW;
int getutline_r(struct utmp *ut, struct utmp *ubuf, struct utmp **ubufp) __THROW;

void pututline(struct utmp *ut) __THROW;

void setutent(void) __THROW;
void endutent(void) __THROW;

void utmpname(const char *file) __THROW;

void updwtmp(const char *wtmp_file, const struct utmp *ut);
void logwtmp(const char *line, const char *name, const char *host);

__END_DECLS

#endif
