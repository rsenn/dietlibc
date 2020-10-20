#if !defined(__i386__) && !defined(__x86_64__)
#include <float.h>
#include "libm.h"
#include <stdint.h>

double
sin(double x) {
  double y[2];
  uint32_t ix;
  unsigned n;

  /* High word of x. */
  GET_HIGH_WORD(ix, x);
  ix &= 0x7fffffff;

  /* |x| ~< pi/4 */
  if(ix <= 0x3fe921fb) {
    if(ix < 0x3e500000) { /* |x| < 2**-26 */
      /* raise inexact if x != 0 and underflow if subnormal*/
      FORCE_EVAL(ix < 0x00100000 ? x / 0x1p120f : x + 0x1p120f);
      return x;
    }
    return __sin(x, 0.0, 0);
  }

  /* sin(Inf or NaN) is NaN */
  if(ix >= 0x7ff00000)
    return x - x;

  /* argument reduction needed */
  n = __rem_pio2(x, y);
  switch(n & 3) {
    case 0: return __sin(y[0], y[1], 1);
    case 1: return __cos(y[0], y[1]);
    case 2: return -__sin(y[0], y[1], 1);
    default: return -__cos(y[0], y[1]);
  }
}
#endif /* !defined(__i386__) && !defined(__x86_64__) */
