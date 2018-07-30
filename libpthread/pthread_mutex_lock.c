#include <unistd.h>
#include <sched.h>
#include <errno.h>

#include <pthread.h>
#include "thread_internal.h"

/* will never return EINVAL ! */

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
  _pthread_descr this;

  __THREAD_INIT();

  this = __thread_self();

  if (this==mutex->owner) {
    if (mutex->kind==PTHREAD_MUTEX_ERRORCHECK_NP)
      return EDEADLK;
    if (mutex->kind==PTHREAD_MUTEX_RECURSIVE_NP) {
      ++(mutex->count);
      return 0;
    }
  }

  /* wait for mutex to free */
  __pthread_lock(&(mutex->lock));
  mutex->owner=this;

  return 0;
}
