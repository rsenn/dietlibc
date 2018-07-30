#include <unistd.h>
#include <errno.h>

#include <pthread.h>
#include "thread_internal.h"

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
			   const struct timespec *abstime)
{
  _pthread_descr tmp;
  _pthread_descr this;
  int ret=0;

  __THREAD_INIT();

  this=__thread_self();

  /* put in wait-chain */
  __NO_ASYNC_CANCEL_BEGIN;
  __pthread_lock(&(cond->lock));
  this->waiting=1;
  if (cond->wait_chain) {
    for(tmp=cond->wait_chain;tmp->waitnext;tmp=tmp->waitnext);
    tmp->waitnext=this;
  } else cond->wait_chain=this;
  __pthread_unlock(&(cond->lock));
  __NO_ASYNC_CANCEL_STOP;

  /* Aeh yeah / wait till cond-signal OR timeout */
  pthread_mutex_unlock(mutex);
  while (this->waiting) {
    struct timeval tv;
    gettimeofday(&tv,0);
    if ((abstime->tv_sec <= tv.tv_sec) && (abstime->tv_nsec <= (tv.tv_usec*1000))) {
      ret = 1;
      break;
    }
    __thread_wait_some_time();
    if (this->canceled) break;	/* a cancel */
  }
  pthread_mutex_lock(mutex);

  __NO_ASYNC_CANCEL_BEGIN;
  __pthread_lock(&(cond->lock));
  if (this->waiting) {	/* still waiting -> TIMEOUT or SIGNAL */
    _pthread_descr prev;
    /* remove from wait-chain */
    prev=cond->wait_chain;
    if ((prev=cond->wait_chain)==this) {
      cond->wait_chain=this->waitnext;
    } else {
      for (tmp=prev->waitnext;tmp;prev=tmp,tmp=prev->waitnext) {
	if (tmp==this) {
	  prev->waitnext=this->waitnext;
	  break;
	}
      }
    }
    this->waiting=0;
    this->waitnext=0;
  }
  __pthread_unlock(&(cond->lock));
  __NO_ASYNC_CANCEL_END;

  if (ret) return ETIMEDOUT;

  {
    register int err = (*__errno_location());
    if (err==EINTR) return err;
  }
  return 0;
}


