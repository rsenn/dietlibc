#include <pthread.h>
#include "thread_internal.h"

int pthread_mutexattr_settype(pthread_mutexattr_t *a, int type)
{
	if ((unsigned)type > 2) return EINVAL;
	a->__mutexkind = (a->__mutexkind & ~3) | type;
	return 0;
}
