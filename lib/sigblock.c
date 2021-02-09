#include <errno.h>
#include <signal.h>

static inline int
sigset_set_old_mask(sigset_t* set, int mask) {
  unsigned long int* ptr;
  int cnt;

  ptr = &set->sig[0];

  *ptr++ = (unsigned int)mask;

  cnt = _SIGSET_WORDS - 2;
  do
    *ptr++ = 0ul;
  while(--cnt >= 0);

  return 0;
}

static inline int
sigset_get_old_mask(const sigset_t* set) {
  return (unsigned int)set->sig[0];
}

/* Block signals in MASK, returning the old mask.  */
int
sigblock(int mask) {
  sigset_t set, oset;

  if(sigset_set_old_mask(&set, mask) < 0)
    return -1;

  if(sigprocmask(SIG_BLOCK, &set, &oset) < 0)
    return -1;

  return sigset_get_old_mask(&oset);
}
