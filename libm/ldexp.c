#if !defined(__i386__) && !defined(__x86_64__)
#include <math.h>

double
ldexp(double x, int n) {
  return scalbn(x, n);
}
#endif /* !defined(__i386__) && !defined(__x86_64__) */
