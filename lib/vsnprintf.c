#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "dietstdio.h"

struct str_data {
  unsigned char* str;
  size_t len;
  size_t size;
};

static int swrite(void*ptr, size_t nmemb, struct str_data* sd) {
  size_t tmp=sd->size-sd->len;
  if (tmp>0) {
    size_t len=nmemb;
    if (len>tmp) len=tmp;
    if (sd->str) {
      memcpy(sd->str+sd->len,ptr,len);
      sd->str[sd->len+len]=0;
    }
    sd->len+=len;
  }
  return nmemb;
}

int vsnprintf(char* str, size_t size, const char *format, va_list arg_ptr) {
  long n;
  struct str_data sd = { str, 0, size-1 };
  struct arg_printf ap = { &sd, (int(*)(void*,size_t,void*)) swrite };
  if (size) --sd.size;
  n=__v_printf(&ap,format,arg_ptr);
  if (str) {
    if (((long)n>(long)size) && size>0) str[size]=0;
    else str[n]=0;
  }
  return n;
}
