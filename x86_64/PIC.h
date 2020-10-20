#ifndef __X86_64_PIC_H__
#define __X86_64_PIC_H__
#ifdef __DYN_LIB

#undef __x86_64

#if 1
/* don't trash the athlon return stack */
#if (__GNUC__ >= 3)	// FIXME: how do I check for the binutils version ?
.section .gnu.linkonce.t.__x86_64.get_pc_thunk.bx,"ax",@progbits
.global __x86_64.get_pc_thunk.bx
.hidden __x86_64.get_pc_thunk.bx
.type   __x86_64.get_pc_thunk.bx,@function
__x86_64.get_pc_thunk.bx:
	movq	(%rsp), %rbx
	ret
.previous

.macro PIC_INIT
	call	__x86_64.get_pc_thunk.bx
	addq	$_GLOBAL_OFFSET_TABLE_, %rbx
.endm
#else
.text
.Lgetpic:
	mov	(%rsp),%rbx
	ret

.macro PIC_INIT
	call	.Lgetpic
	addl	$_GLOBAL_OFFSET_TABLE_, %rbx
.endm
#endif
#else
/* standard code for PIC init */
.macro PIC_INIT
	call	0f
0:	popl	%rbx
	addl	$_GLOBAL_OFFSET_TABLE_+[.-0b], %rbx
.endm
#endif

#define PIC_SAVE	pushq	%rbx
#define PIC_RESTORE	popq	%rbx


/* get memory variable (var) in register (reg) */
.macro GET_VAR var reg
	movq	\var@GOT(%rbx), \reg
	movq	(\reg), \reg
.endm

/* put register (reg) into memory variable (var) TRASH register (tmp) */
.macro PUT_VAR reg var tmp
	movq	\var@GOT(%rbx), \tmp
	movq	\reg, (\tmp)
.endm

/* get memory variable (var) in register (reg)
 * IF no more PIC-code is needed */
.macro GET_1VAR var reg
	PIC_SAVE
	PIC_INIT
	GET_VAR \var, \reg
	PIC_RESTORE
.endm

/* put register (reg) into memory variable (var)
 * IF no more PIC-code is needed */
.macro PUT_1VAR reg var
	PIC_SAVE
	PIC_INIT
	PUT_VAR \reg, \var, %rbx
	PIC_RESTORE
.endm

#else

#define PIC_SAVE
#define PIC_RESTORE
#define PIC_INIT

.macro GET_VAR var reg
	movq	\var, \reg
.endm

.macro PUT_VAR reg var tmp
	movq	\reg, \var
.endm

.macro GET_1VAR var reg
	GET_VAR \var, \reg
.endm

.macro PUT_1VAR reg var
	PUT_VAR \reg, \var, none
.endm

#endif
#endif
