#include <sys/types.h>
#include <sys/mman.h>
#include <netdb.h>
#include <ctype.h>
#include "parselib.h"

static struct state __ps;

void setservent(int stayopen) {
  (void)stayopen;
  __prepare_parse(_PATH_SERVICES,&__ps);
}

void endservent(void) {
  __end_parse(&__ps);
}

#define ALIASES 16

/* "tcpmux		1/tcp		# TCP port multiplexer (RFC1078)" */
int getservent_r(struct servent *res, char *buf, size_t buflen,
		 struct servent **res_sig) {
  size_t i,j,n,g;
  unsigned long l;
  setservent(0);
  if (!__ps.buffirst) goto error;
  if (__ps.cur>=__ps.buflen) goto error;
  res->s_aliases=(char**)buf;
/*  getservent */
again:
  n=ALIASES*sizeof(char*); g=0;
  for (i=0; i<3; ++i) {
    char found;
    __ps.cur+=__parse_ws(&__ps);
    if (__ps.cur>=__ps.buflen) { if (i==2) break; else goto error; }
    j=__parse_nws(&__ps);
    if (!isblank(found=__ps.buffirst[__ps.cur+j])) {
      if (i==2) break;	/* it's ok, no (more) aliases necessary */
parseerror:
      while (__ps.cur+j<__ps.buflen) {
	if (__ps.buffirst[__ps.cur+j]=='\n') {
	  __ps.cur+=j+1;
	  goto again;
	}
	++j;
      }
    }
    switch (i) {
    case 0:
      res->s_name=buf+n;
copy:
      if ((size_t)buflen<=n+j) goto error;
      memcpy(buf+n,__ps.buffirst+__ps.cur,j);
      buf[n+j]=0;
      n+=j+1;
      if ((found=='\n' || found=='#') && i==1) i=3;
      break;
    case 1:
      {
	int k;
	k=scan_ulong(__ps.buffirst+__ps.cur,&l);
	if (__ps.buffirst[__ps.cur+k]!='/') goto parseerror;
	res->s_port=l;
	res->s_proto=buf+n;
	j-=k+1; __ps.cur+=k+1;
	goto copy;
      }
    case 2:
      res->s_aliases[g]=buf+n;
      ++g;
      if (g==(ALIASES-1)) break;
      --i;	/* again */
      goto copy;
    }
    __ps.cur+=j+1;
  }
  res->s_aliases[g]=0;
  *res_sig=res;
  return 0;
error:
  *res_sig=0;/* the glibc people should be taken behind the barn and shot */
  return -1;
}
