#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

#include "_dl_int.h"

struct _dl_handle * _dl_root_handle=(struct _dl_handle*)0;
struct _dl_handle * _dl_top_handle=(struct _dl_handle*)0;
static struct _dl_handle * _dl_free_list=(struct _dl_handle*)0;

void _dl_free_handle(struct _dl_handle* dh) {
  if (_dl_root_handle == dh) _dl_root_handle = dh->next;
  if (_dl_top_handle == dh)  _dl_top_handle  = dh->prev;

  if (dh->next) dh->next->prev=dh->prev;
  if (dh->prev) dh->prev->next=dh->next;

  memset(dh,0,sizeof(struct _dl_handle));
  dh->next=_dl_free_list;
  _dl_free_list=dh;
}

struct _dl_handle* _dl_get_handle() {
  struct _dl_handle* tmp;

  if (_dl_free_list==0) {
    register int i,m;
    tmp = (struct _dl_handle *)mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    m=4096/sizeof(struct _dl_handle);
    for (i=m;i;) {
      _dl_free_handle(tmp+(--i));
    }
  }

  tmp = _dl_free_list;
  _dl_free_list = tmp->next;

  tmp->next=0;
  if (_dl_root_handle) {
    _dl_top_handle->next=tmp;
    tmp->prev=_dl_top_handle;
  } else
    _dl_root_handle = tmp;

  _dl_top_handle=tmp;

  return tmp;
}
