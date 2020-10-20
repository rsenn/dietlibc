#if !defined(__i386__) && !defined(__x86_64__)
#include <math.h>
#include <stdint.h>

float
fabsf(float x) {
  union {
    float f;
    uint32_t i;
  } u = {x};
  u.i &= 0x7fffffff;
  return u.f;
}
#endif /* !defined(__i386__) && !defined(__x86_64__) */
