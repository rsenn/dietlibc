#include <float.h>
#include "libm.h"
#include <stdint.h>

#define EPS FLT_EPSILON
static const float toint = 1 / EPS;

float
roundf(float x) {
  union {
    float f;
    uint32_t i;
  } u = {x};
  int e = u.i >> 23 & 0xff;
  float y;

  if(e >= 0x7f + 23) return x;
  if(u.i >> 31) x = -x;
  if(e < 0x7f - 1) {
    FORCE_EVAL(x + toint);
    return 0 * u.f;
  }
  y = x + toint - toint - x;
  if(y > 0.5f)
    y = y + x - 1;
  else if(y <= -0.5f)
    y = y + x + 1;
  else
    y = y + x;
  if(u.i >> 31) y = -y;
  return y;
}
