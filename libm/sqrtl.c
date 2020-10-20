#if !defined(__i386__) && !defined(__x86_64__)
#include <math.h>

long double
sqrtl(long double x) {
  /* FIXME: implement in C, this is for LDBL_MANT_DIG == 64 only */
  return sqrt(x);
}
#endif /* !defined(__i386__) && !defined(__x86_64__) */
