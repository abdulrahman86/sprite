	.data
	.text
LL0:
|#PROC# 07

	LF12	=	0
	LS12	=	0
	LFF12	=	0
	LSS12	=	0
	LP12	=	8
	.data
	.text
	.globl	___subdf3
___subdf3:
|#PROLOGUE# 0

	link	a6,#0
|#PROLOGUE# 1

	movl	a6@(8),d0
	movl	a6@(12),d1
	lea	a6@(16),a0
	jsr	Fsubd
	unlk	a6
	rts
