#include <endian.h>

int abs(int i) { return i>=0?i:-i; }
#if __WORDSIZE == 64
long labs(long i) __attribute__((alias("abs")));
#endif
