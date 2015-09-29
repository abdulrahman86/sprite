/* 
 * sigMach.c --
 *
 *	DECstation signals-related routines.
 *
 * Copyright 1992 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that this copyright
 * notice appears in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user5/kupfer/spriteserver/src/sprited/sig/ds3100.md/RCS/sigMach.c,v 1.2 92/03/12 17:50:01 kupfer Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include <sprite.h>
#include <mach_error.h>
#include <mach/exception.h>
#include <mach/mips/exception.h>

#include <proc.h>
#include <sig.h>
#include <sigInt.h>
#include <sigMach.h>
#include <user/sigMach.h>
#include <sigTypes.h>
#include <vm.h>


/*
 *----------------------------------------------------------------------
 *
 * SigMach_SetSignalState --
 *
 *	Set up the machine-dependent state before a forced call to a signal 
 *	handler.
 *	
 * Results:
 *	Returns a success/failure code.
 *
 * Side effects:
 * 	Saves the process's current state on the signal stack.  Munges the 
 * 	process's PC to call the handler, and sets up the stack for the 
 * 	handler.
 *
 *----------------------------------------------------------------------
 */

ReturnStatus
SigMach_SetSignalState(procPtr, sigStackPtr, pc)
    Proc_LockedPCB *procPtr;	/* the process that's taking the signal */
    Sig_Stack *sigStackPtr;	/* partial info to put on the thread's stack */
    Address pc;			/* start address of the signal handler */
{
    kern_return_t kernStatus;
    SigMach_Context *machContextPtr = &sigStackPtr->contextPtr->machContext;
    struct mips_thread_state newRegs; /* registers for calling handler */
    Address topOfStack;		/* user stack pointer */
    ReturnStatus status;
    mach_msg_type_number_t numWords; /* words of thread state read */

    /* 
     * Get a copy of the current state and write it out to the stack.
     */

    numWords = MIPS_THREAD_STATE_COUNT;
    kernStatus = thread_get_state(procPtr->pcb.thread, MIPS_THREAD_STATE,
				  (thread_state_t)&machContextPtr->regs,
				  &numWords);
    if (numWords != MIPS_THREAD_STATE_COUNT) {
	printf("%s: expected %d words of state, got %d.\n",
	       "SigMach_SetSignalState", MIPS_THREAD_STATE_COUNT,
	       numWords);
	return FAILURE;
    }
    numWords = MIPS_FLOAT_STATE_COUNT;
    kernStatus = thread_get_state(procPtr->pcb.thread, MIPS_FLOAT_STATE,
				  (thread_state_t)&machContextPtr->fpRegs,
				  &numWords);
    if (numWords != MIPS_FLOAT_STATE_COUNT) {
	printf("%s: expected %d words of fp state, got %d.\n",
	       "SigMach_SetSignalState", MIPS_FLOAT_STATE_COUNT,
	       numWords);
	return FAILURE;
    }

    bcopy(&machContextPtr->regs, &newRegs, sizeof(newRegs));
    if (sigDebug) {
	printf("set up handler: old stack ptr = 0x%x\n", newRegs.r29);
    }

    /* 
     * Make sure the stack pointer is word-aligned.
     */
    topOfStack = (Address)(machContextPtr->regs.r29 & ~(sizeof(int) - 1));
    topOfStack -= sizeof(Sig_Context);
    if (sigDebug) {
	printf("set up handler: context copied to 0x%x\n",
	       topOfStack);
    }
    status = Vm_CopyOut(sizeof(Sig_Context), (Address)sigStackPtr->contextPtr,
			topOfStack);
    if (status != SUCCESS) {
	printf("%s: can't save state for pid %x: %s\n",
	       "SigMach_SetSignalState", procPtr->pcb.processID,
	       Stat_GetMsg(status));
	return FAILURE;
    }

    /* 
     * Allocate a frame for the fictitious caller of the sigtramp routine 
     * and set up the sigtramp arguments.  The first four go into registers,
     * the fifth goes on the stack.
     */
    
    newRegs.r4 = (int)pc;	/* a0; address of signal handler */
    newRegs.r5 = sigStackPtr->sigNum; /* a1 */
    newRegs.r6 = sigStackPtr->sigCode; /* a2 */
    newRegs.r7 = (int)topOfStack; /* a3; user Sig_Context ptr */
    topOfStack -= 5 * sizeof(int);
    status = Vm_CopyOut(sizeof(int), (Address)&sigStackPtr->sigAddr,
			topOfStack + 4*sizeof(int));
    if (status != SUCCESS) {
	printf("%s: can't set up args for trampoline routine for pid %x: %s\n",
	       "SigMach_SetSignalState", procPtr->pcb.processID,
	       Stat_GetMsg(status));
	return FAILURE;
    }
    if (sigDebug) {
	printf("set up handler: handler at 0x%x.\n", pc);
    }

    /* 
     * Set up the thread's registers so that it next invokes the trampoline 
     * routine.
     */
    
    if (procPtr->pcb.sigTrampProc == USER_NIL) {
	printf("%s: bogus trampoline address for pid %x.\n",
	       "SigMach_SetSignalState", procPtr->pcb.processID);
	return FAILURE;
    }
    newRegs.r29 = (int)topOfStack;
    newRegs.pc = (int)(procPtr->pcb.sigTrampProc);
    if (sigDebug) {
	printf("set up handler: sigtramp at 0x%x, sp at 0x%x\n",
	       newRegs.pc, newRegs.r29);
    }
    kernStatus = thread_set_state(procPtr->pcb.thread, MIPS_THREAD_STATE,
				  (thread_state_t)&newRegs,
				  MIPS_THREAD_STATE_COUNT);
    if (kernStatus != KERN_SUCCESS) {
	printf("%s: can't set state for pid %x: %s.\n",
	       "SigMach_SetSignalState", procPtr->pcb.processID,
	       mach_error_string(kernStatus));
	return FAILURE;
    }

    return SUCCESS;
}


/*
 *----------------------------------------------------------------------
 *
 * SigMach_ExcToSig --
 *
 *	DECstation-specific code to map a Mach exception to a Sprite 
 *	signal.
 *
 * Results:
 *	Fills in the Sprite signal number, subcode, and address.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

void
SigMach_ExcToSig(exceptionType, exceptionCode, exceptionSubcode, sigNumPtr,
		 codePtr, sigAddrPtr)
    int exceptionType;
    int exceptionCode;
    int exceptionSubcode;
    int *sigNumPtr;		/* OUT: signal number */
    int *codePtr;		/* OUT: signal subcode */
    Address *sigAddrPtr;	/* OUT: the guilty address, if any */
{
    int sigNum = 0;
    int code = SIG_NO_CODE;

    /* 
     * Look for machine-specific exception codes.  If we don't find one 
     * that matches, use the machine-independent mapping.
     */

    switch (exceptionType) {
    case EXC_BAD_INSTRUCTION:
	sigNum = SIG_ILL_INST;
	switch (exceptionCode) {
	case EXC_MIPS_RESOPND:
	    code = SIG_BAD_TRAP;
	    break;
	case EXC_MIPS_PRIVINST:
	    code = SIG_PRIV_INST;
	    break;
	case EXC_MIPS_RESADDR:
	    sigNum = SIG_ADDR_FAULT;
	    code = SIG_ACCESS_VIOL;
	    break;
	}
	break;
    case EXC_ARITHMETIC:
	sigNum = SIG_ARITH_FAULT;
	switch (exceptionCode) {
	case EXC_MIPS_FLT_UNIMP:
#if 0
	    /* 
	     * XXX If you look at the hardware exception code that causes 
	     * this Mach exception code, compatibility with Sprite calls 
	     * for the following signal number and code.  However, this 
	     * means that 0.0/0.0 generates SIGILL instead of SIGFPE (at 
	     * least until the floating point support in Mach is fixed), 
	     * which is pretty silly.
	     */
	    sigNum = SIG_ILL_INST;
	    code = SIG_FP_EXCEPTION;
#endif
	    break;
	case EXC_MIPS_FLT_OVERFLOW:
	    code = SIG_OVERFLOW;
	    break;
	case EXC_MIPS_FLT_DIVIDE0:
	    code = SIG_ZERO_DIV;
	    break;
	}
	break;
    case EXC_BREAKPOINT:
	switch (exceptionCode) {
	case EXC_MIPS_BPT:
	    sigNum = SIG_BREAKPOINT;
	    break;
	case EXC_MIPS_TRACE:
	    sigNum = SIG_TRACE_TRAP;
	    break;
	}
	break;
    case EXC_SOFTWARE:
	switch (exceptionCode) {
	case EXC_MIPS_SOFT_SEGV:
	    sigNum = SIG_ADDR_FAULT;
	    break;
	case EXC_MIPS_SOFT_CPU:
	    /* 
	     * We expect the hardware to generate the corresponding
	     * exception only if a process is trying to do a floating point
	     * operation and the floating point state hasn't been set up
	     * yet.  The kernel should have taken care of this silently.
	     */
	    sigNum = SIG_ARITH_FAULT;
	    printf("SigMach_ExcToSig: coprocessor unusable.\n");
	    break;
	}
	break;
    }

    if (sigNum != 0) {
	*sigNumPtr = sigNum;
	*codePtr = code;
	*sigAddrPtr = (Address)exceptionSubcode;
    } else {
	Sig_ExcToSig(exceptionType, exceptionCode, exceptionSubcode, sigNumPtr,
		     codePtr, sigAddrPtr);
    }
}


/*
 *----------------------------------------------------------------------
 *
 * SigMach_RestoreState --
 *
 *	Restore registers that were saved before calling the signal 
 *	handler.
 *
 * Results:
 *	Returns success/failure.
 *
 * Side effects:
 *	Changes the registers for the thread.
 *
 *----------------------------------------------------------------------
 */

ReturnStatus
SigMach_RestoreState(procPtr, sigContextPtr)
    Proc_LockedPCB *procPtr;	/* the process that gets changed */
    Sig_Context *sigContextPtr;	/* holds the register values to use */
{
    kern_return_t kernStatus;
    SigMach_Context *machContextPtr;

    machContextPtr = &sigContextPtr->machContext;
    kernStatus = thread_set_state(procPtr->pcb.thread, MIPS_THREAD_STATE,
				  (thread_state_t)&machContextPtr->regs,
				  MIPS_THREAD_STATE_COUNT);
    if (kernStatus != KERN_SUCCESS) {
	printf("%s: can't restore registers for pid %x: %s\n",
	       "SigMach_RestoreState", procPtr->pcb.processID,
	       mach_error_string(kernStatus));
	return FAILURE;
    }
    kernStatus = thread_set_state(procPtr->pcb.thread, MIPS_FLOAT_STATE,
				  (thread_state_t)&machContextPtr->fpRegs,
				  MIPS_FLOAT_STATE_COUNT);
    if (kernStatus != KERN_SUCCESS) {
	printf("%s: can't restore fp registers for pid %x: %s\n",
	       "SigMach_RestoreState", procPtr->pcb.processID,
	       mach_error_string(kernStatus));
	return FAILURE;
    }

    return SUCCESS;
}
