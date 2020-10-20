#ifndef _SCHED_H
#define _SCHED_H 1

#include <time.h>
#include <sys/types.h>

__BEGIN_DECLS

/*
 * Scheduling policies
 */
#define SCHED_OTHER		0
#define SCHED_FIFO		1
#define SCHED_RR		2

/*
 * This is an additional bit set when we want to
 * yield the CPU for one re-schedule..
 */
#define SCHED_YIELD		0x10

struct sched_param {
  int sched_priority;
};

/* END OF COPY form kernel-header */

int __sched_setparam(pid_t pid, const struct sched_param* p) __THROW;
int sched_setparam(pid_t pid, const struct sched_param* p) __THROW;

int __sched_getparam(pid_t pid, struct sched_param* p) __THROW;
int sched_getparam(pid_t pid, struct sched_param* p) __THROW;

int __sched_getscheduler(pid_t pid) __THROW;
int sched_getscheduler(pid_t pid) __THROW;

int __sched_setscheduler(pid_t pid, int policy, const struct sched_param* p) __THROW;
int sched_setscheduler(pid_t pid, int policy, const struct sched_param* p) __THROW;

int __sched_yield(void) __THROW;
int sched_yield(void) __THROW;

int __sched_get_priority_max(int policy) __THROW;
int sched_get_priority_max(int policy) __THROW;

int __sched_get_priority_min(int policy) __THROW;
int sched_get_priority_min(int policy) __THROW;

int __sched_rr_get_interval(pid_t pid, struct timespec* tp) __THROW;
int sched_rr_get_interval(pid_t pid, struct timespec* tp) __THROW;

#ifdef _GNU_SOURCE
/*
 * cloning flags:
 */
#define CSIGNAL         0x000000ff      /* signal mask to be sent at exit */
#define CLONE_VM        0x00000100      /* set if VM shared between processes */
#define CLONE_FS        0x00000200      /* set if fs info shared between processes */
#define CLONE_FILES     0x00000400      /* set if open files shared between processes */
#define CLONE_SIGHAND   0x00000800      /* set if signal handlers and blocked signals shared */
#define CLONE_PID       0x00001000      /* set if pid shared */
#define CLONE_PTRACE    0x00002000      /* set if we want to let tracing continue on the child too */
#define CLONE_VFORK     0x00004000      /* set if the parent wants the child to wake it up on mm_release */
#define CLONE_PARENT    0x00008000      /* set if we want to have the same parent as the cloner */
#define CLONE_THREAD    0x00010000      /* Same thread group? */
#define CLONE_NEWNS     0x00020000
#define CLONE_SYSVSEM   0x00040000
#define CLONE_SETTLS    0x00080000
#define CLONE_PARENT_SETTID 0x00100000
#define CLONE_CHILD_CLEARTID 0x00200000
#define CLONE_DETACHED  0x00400000
#define CLONE_UNTRACED  0x00800000
#define CLONE_CHILD_SETTID 0x01000000
#define CLONE_NEWUTS    0x04000000
#define CLONE_NEWIPC    0x08000000
#define CLONE_NEWUSER   0x10000000
#define CLONE_NEWPID    0x20000000
#define CLONE_NEWNET    0x40000000
#define CLONE_IO        0x80000000

#define CLONE_SIGNAL    (CLONE_SIGHAND | CLONE_THREAD)

int clone(int (*fn)(void*),void* stack,int flags,void* arg, ...) __THROW;

int unshare(int flags) __THROW;

long set_tid_address(int* tid) __THROW;

/*
 * Linux CPU affinity.
 * For simplicity it supports up to 32 CPUs for 32 bits systems and up to
 * 64 for 64 bits systems. Add the other CPU_SET(3) macros when needed.
 */

typedef	unsigned long cpu_set_t;

static inline void CPU_ZERO(cpu_set_t *set) {*set = 0;}
static inline void CPU_SET(int cpu, cpu_set_t *set) {*set |= (1UL << cpu);}
static inline void CPU_CLR(int cpu, cpu_set_t *set) {*set &= ~(1UL << cpu);}
static inline int CPU_ISSET(int cpu, cpu_set_t *set) {
	return !!(*set & (1UL << cpu));
}

static inline int CPU_COUNT(cpu_set_t *set)
{
	int c;
	unsigned long v;

	/* Peter Wegner/Derrick Lehmer/Brian Kernighan's method */
	for (c = 0, v = *set; v; c++)
		v &= v - 1; // clear the least significant bit set
	return c;
}

static inline void CPU_AND(cpu_set_t *dest, cpu_set_t *s1, cpu_set_t *s2){
	*dest = *s1 & *s2;
}

static inline void CPU_OR(cpu_set_t *dest, cpu_set_t *s1, cpu_set_t *s2){
	*dest = *s1 | *s2;
}

static inline void CPU_XOR(cpu_set_t *dest, cpu_set_t *s1, cpu_set_t *s2){
	*dest = *s1 ^ *s2;
}

static inline int  CPU_EQUAL(cpu_set_t *s1, cpu_set_t *s2){
	return *s1 == *s2;
}

int sched_setaffinity(pid_t pid, size_t size, cpu_set_t *mask) __THROW;
int sched_getaffinity(pid_t pid, size_t size, cpu_set_t *mask) __THROW;
#define pthread_setaffinity_np sched_setaffinity
#define pthread_getaffinity_np sched_getaffinity

int setns(int fd,int nstype) __THROW;

#endif

#ifdef _GNU_SOURCE

#define __CPU_op_S(i, size, set, op) ( (i)/8U >= (size) ? 0 : \
	(((unsigned long *)(set))[(i)/8/sizeof(long)] op (1UL<<((i)%(8*sizeof(long))))) )

#define CPU_SET_S(i, size, set) __CPU_op_S(i, size, set, |=)
#define CPU_CLR_S(i, size, set) __CPU_op_S(i, size, set, &=~)
#define CPU_ISSET_S(i, size, set) __CPU_op_S(i, size, set, &)

#define __CPU_op_func_S(func, op) \
static __inline void __CPU_##func##_S(size_t __size, cpu_set_t *__dest, \
	const cpu_set_t *__src1, const cpu_set_t *__src2) \
{ \
	size_t __i; \
	for (__i=0; __i<__size/sizeof(long); __i++) \
		((unsigned long *)__dest)[__i] = ((unsigned long *)__src1)[__i] \
			op ((unsigned long *)__src2)[__i] ; \
}

__CPU_op_func_S(AND, &)
__CPU_op_func_S(OR, |)
__CPU_op_func_S(XOR, ^)

#define CPU_AND_S(a,b,c,d) __CPU_AND_S(a,b,c,d)
#define CPU_OR_S(a,b,c,d) __CPU_OR_S(a,b,c,d)
#define CPU_XOR_S(a,b,c,d) __CPU_XOR_S(a,b,c,d)

#define CPU_COUNT_S(size,set) __sched_cpucount(size,set)
#define CPU_ZERO_S(size,set) memset(set,0,size)
#define CPU_EQUAL_S(size,set1,set2) (!memcmp(set1,set2,size))

#define CPU_ALLOC_SIZE(n) (sizeof(long) * ( (n)/(8*sizeof(long)) \
	+ ((n)%(8*sizeof(long)) + 8*sizeof(long)-1)/(8*sizeof(long)) ) )
#define CPU_ALLOC(n) ((cpu_set_t *)calloc(1,CPU_ALLOC_SIZE(n)))
#define CPU_FREE(set) free(set)

#define CPU_SETSIZE 128

#define CPU_SET(i, set) CPU_SET_S(i,sizeof(cpu_set_t),set)
#define CPU_CLR(i, set) CPU_CLR_S(i,sizeof(cpu_set_t),set)
#define CPU_ISSET(i, set) CPU_ISSET_S(i,sizeof(cpu_set_t),set)
#define CPU_AND(d,s1,s2) CPU_AND_S(sizeof(cpu_set_t),d,s1,s2)
#define CPU_OR(d,s1,s2) CPU_OR_S(sizeof(cpu_set_t),d,s1,s2)
#define CPU_XOR(d,s1,s2) CPU_XOR_S(sizeof(cpu_set_t),d,s1,s2)
#define CPU_COUNT(set) CPU_COUNT_S(sizeof(cpu_set_t),set)
#define CPU_ZERO(set) CPU_ZERO_S(sizeof(cpu_set_t),set)
#define CPU_EQUAL(s1,s2) CPU_EQUAL_S(sizeof(cpu_set_t),s1,s2)

#endif

__END_DECLS

#endif
