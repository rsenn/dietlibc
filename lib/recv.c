#include <linux/net.h>

extern int socketcall(int callno,long* args);

#ifdef __i386__
int __libc_recv(int a) {
  return socketcall(SYS_RECV,(long*)&a);
#else
int __libc_recv(int a, const void * b, size_t c, int flags) {
  unsigned long args[] = { a, (long) b, c, flags };
  return socketcall(SYS_RECV, args);
#endif
}

int recv(int a, const void * b, size_t c, int flags)
  __attribute__ ((weak, alias("__libc_recv")));
