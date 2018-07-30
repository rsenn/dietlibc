#include <float.h>
#include <sys/types.h>

double
__poly(double x, size_t n, const double* c) {
  long double ret;
  size_t i;

  i = n;
  c += n;
  ret = 0;
  do
    ret = ret * x + *c--;
  while(i--);

  return ret;
}
