	.file	"__waitpid.c"
	.pred.safe_across_calls p1-p5,p16-p63
	.text
	.align 16
	.global waitpid#
	.proc waitpid#
waitpid:
	.prologue 12, 35
	.mmi
	.save ar.pfs, r36
	alloc r36 = ar.pfs, 3, 3, 4, 0
	mov r37 = r1
	.save rp, r35
	mov r35 = b0
	.body
	.mmi
	mov r38 = r32
	mov r39 = r33
	mov r40 = r34
	.mmb
	nop 0
	mov r41 = r0
	nop 0
	;;
	.bbb
	nop 0
	nop 0
	br.call.sptk.many b0 = wait4#
	;;
	.mmi
	nop 0
	mov r1 = r37
	mov b0 = r35
	.mib
	nop 0
	mov ar.pfs = r36
	br.ret.sptk.many b0
	.endp waitpid#
	.ident	"GCC: (GNU) 3.4.0"
