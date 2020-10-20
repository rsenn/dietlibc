#if !defined(__i386__) && !defined(__x86_64__)
#include <math.h>
#include <stdint.h>

double
fabs(double x) {
  union {
    double f;
    uint64_t i;
  } u = {x};
  u.i &= -1ULL / 2;
  return u.f;
}
#endif /* !defined(__i386__) && !defined(__x86_64__) */
