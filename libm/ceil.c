#include <float.h>
#include "libm.h"
#include <stdint.h>

static const double toint = 1 / DBL_EPSILON;

double
ceil(double x) {
  union {
    double f;
    uint64_t i;
  } u = {x};
  int e = u.i >> 52 & 0x7ff;
  double y;

  if(e >= 0x3ff + 52 || x == 0)
    return x;
  /* y = int(x) - x, where int(x) is an integer neighbor of x */
  if(u.i >> 63)
    y = x - toint + toint - x;
  else
    y = x + toint - toint - x;
  /* special case because of non-nearest rounding modes */
  if(e <= 0x3ff - 1) {
    FORCE_EVAL(y);
    return u.i >> 63 ? -0.0 : 1;
  }
  if(y < 0)
    return x + y + 1;
  return x + y;
}
