#include <sys/socket.h>
#include <linuxnet.h>

extern int socketcall(int callno,long* args);

#ifdef __i386__
int __libc_sendto(int a) {
  return socketcall(SYS_SENDTO, (long*)&a);
#else
int __libc_sendto(int a, const void * b, size_t c, int flags, void *to, int tolen) {
  unsigned long args[] = { a, (long) b, c, flags, (long) to, tolen };
  return socketcall(SYS_SENDTO, args);
#endif
}

int sendto(int a, const void * b, size_t c, int flags, const struct sockaddr* to, socklen_t tolen)
  __attribute__ ((weak, alias("__libc_sendto")));
