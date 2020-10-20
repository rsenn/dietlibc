#include "PIC.h"

.text

.global __write1
.type	__write1,@function

.global __write2
.type	__write2,@function

					# regparm=1, daher Stringadresse in %rax
__write2:
	clc				# CY = 0
	.byte	0xB2			# zusammen mit nächstem Byte: mov dl,0xF9
__write1:
	stc				# CY = 1
	sbbq	%rcx,%rcx		# __write2: rcx=0, __write1: rcx=-1
	incq	%rcx
	incq	%rcx			# __write2: rcx=2, __write1: rcx=1
	xorq	%rdx,%rdx
	decq	%rdx
.Lnext:	incq	%rdx
	cmpb	%ch,(%rdx,%rax)		# ch=0, da bei beiden Filedescriptoren Bits 15:8 0 sind
	jnz	.Lnext			# Stringlänge in rdx, ohne rax zerstört zu haben
	PIC_SAVE		# non-PIC: empty line
	PIC_INIT		# non-PIC: empty line
	pushq	%rdx
	pushq	%rax
	pushq	%rcx
#ifdef __DYN_LIB
	call	write@PLT
#else
	call	write			# alles ruf uf dn Stack und ab damit
#endif
	addq	$12,%rsp		# und das leidvolle Putzen
	PIC_RESTORE		# non-PIC: empty line
	ret
