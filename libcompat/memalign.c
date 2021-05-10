#include <stdlib.h>

int posix_memalign(void **res, size_t align, size_t len);

void*
memalign(size_t align, size_t len) { 
  void *ptr;
  if(posix_memalign(&ptr, align, len))
    return 0;
  return ptr;
}
