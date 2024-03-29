/* Copyright (C) 1996-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1996.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include <errno.h>
#include <string.h>
#include <utmp.h>
#include <time.h>
#include <sys/time.h>
#define TIMESPEC_TO_TIMEVAL(tv, ts) ( \
	(tv)->tv_sec = (ts)->tv_sec, \
	(tv)->tv_usec = (ts)->tv_nsec / 1000, \
	(void)0 )

int
logout (const char *line)
{
  struct utmp tmp, utbuf;
  struct utmp *ut;
  int result = 0;

  /* Tell that we want to use the UTMP file.  */
  utmpname (_PATH_UTMP);

  /* Open UTMP file.  */
  setutent ();

  /* Fill in search information.  */
  tmp.ut_type = USER_PROCESS;
  strncpy (tmp.ut_line, line, sizeof tmp.ut_line);

  /* Read the record.  */
  if (getutline_r (&tmp, &utbuf, &ut) >= 0)
    {
      /* Clear information about who & from where.  */
      memset (ut->ut_name, '\0', sizeof ut->ut_name);
      memset (ut->ut_host, '\0', sizeof ut->ut_host);

      struct timespec ts;
      clock_gettime (CLOCK_REALTIME, &ts);
      TIMESPEC_TO_TIMEVAL (&ut->ut_tv, &ts);
      ut->ut_type = DEAD_PROCESS;

      pututline (ut);
    }

  /* Close UTMP file.  */
  endutent ();

  return result;
}
