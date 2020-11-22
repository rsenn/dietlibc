#include "thread_internal.h"

int pthread_mutexattr_setpshared(pthread_mutexattr_t *a, int pshared)
{
	if (pshared > 1U) return EINVAL;
	a->__mutexkind &= ~128U;
	a->__mutexkind |= pshared<<7;
	return 0;
}
