#include <resolv.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/nameser.h>
#include "dietfeatures.h"

extern int h_errno;

static char dnspacket[]="\xfe\xfe\001\000\000\001\000\000\000\000\000\000";

extern void __dns_make_fd(void);
extern int __dns_fd;

extern int __dns_servers;
extern struct sockaddr __dns_server_ips[];

extern void __dns_readstartfiles(void);

extern int __dns_decodename(unsigned char *packet,unsigned int offset,unsigned char *dest,unsigned int maxlen);

int res_mkquery(int op, const char *dname, int class, int type, char* data,
		int datalen, const unsigned char* newrr, char* buf, int buflen) {
  unsigned char packet[512];
  memmove(packet,dnspacket,12);
  *(unsigned short*)packet=rand();
  {
    unsigned char* x;
    const char* y,* tmp;
    x=packet+12; y=dname;
    while (*y) {
      while (*y=='.') ++y;
      for (tmp=y; *tmp && *tmp!='.'; ++tmp) ;
      *x=tmp-y;
      if (!(tmp-y)) break;
      ++x;
      if (x>=packet+510-(tmp-y)) { return -1; }
      memmove(x,y,tmp-y);
      x+=tmp-y;
      if (!*tmp) {
	*x=0;
	break;
      }
      y=tmp;
    }
    *++x= 0; *++x= type;	/* A */
    *++x= 0; *++x= class;	/* IN */
    ++x;
    if (x-packet>buflen) return -1;
    memmove(buf,packet,x-packet);
    return x-packet;
  }
}
