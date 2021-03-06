#ifndef	_CTYPE_H
#define _CTYPE_H

#include <sys/cdefs.h>

__BEGIN_DECLS

extern int isascii (int c) __THROW;
extern int isblank (int c) __THROW;
extern int isalnum (int c) __THROW;
extern int isalpha (int c) __THROW;
extern int isdigit (int c) __THROW;
extern int isspace (int c) __THROW;

extern int isupper (int c) __THROW;
extern int islower (int c) __THROW;

extern int toascii(int c) __THROW;
extern int tolower(int c) __THROW;
extern int toupper(int c) __THROW;

extern int isprint(int c) __THROW;
extern int ispunct(int c) __THROW;
extern int iscntrl(int c) __THROW;

/* fscking GNU extensions! */
extern int isxdigit(int c) __THROW;

extern int isgraph(int c) __THROW;

__END_DECLS

#endif
