/* 
 * sigStubs.c --
 *
 *	MIG stubs for signals routines.
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
static char rcsid[] = "$Header: /user5/kupfer/spriteserver/src/sprited/sig/RCS/sigStubs.c,v 1.5 92/07/16 18:07:00 kupfer Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include <sprite.h>
#include <mach.h>

#include <sig.h>
#include <sigInt.h>
#include <spriteSrvServer.h>
#include <vm.h>


/*
 *----------------------------------------------------------------------
 *
 * Sig_PauseStub --
 *
 *	The stub for the Sig_Pause Sprite call.
 *
 * Results:
 *	Returns KERN_SUCCESS.  Fills in the Sprite status code from 
 *	Sig_Pause.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

kern_return_t
Sig_PauseStub(serverPort, holdMask, statusPtr, sigPendingPtr)
    mach_port_t serverPort;	/* request port */
    int holdMask;
    ReturnStatus *statusPtr;	/* OUT: Sprite status code */
    boolean_t *sigPendingPtr;	/* OUT: is there a signal pending */
{
#ifdef lint
    serverPort = serverPort;
#endif

    *statusPtr = Sig_Pause(holdMask);
    *sigPendingPtr = Sig_Pending(Proc_GetCurrentProc());
    return KERN_SUCCESS;
}


/*
 *----------------------------------------------------------------------
 *
 * Sig_SendStub --
 *
 *	MIG stub for the Sig_Send Sprite call.
 *
 * Results:
 *	Returns KERN_SUCCESS.  Fills in the Sprite status code from 
 *	Sig_Send and the "pending signals" flag.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

kern_return_t
Sig_SendStub(serverPort, sigNum, id, isFamily, statusPtr, sigPendingPtr)
    mach_port_t serverPort;	/* request port */
    int sigNum;
    Proc_PID id;
    Boolean isFamily;
    ReturnStatus *statusPtr;	/* OUT: Sprite status code */
    boolean_t *sigPendingPtr;	/* OUT: is there a signal pending */
{
#ifdef lint
    serverPort = serverPort;
#endif

    *statusPtr = Sig_Send(sigNum, SIG_NO_CODE, id, isFamily, (Address)0);
    *sigPendingPtr = Sig_Pending(Proc_GetCurrentProc());
    return KERN_SUCCESS;
}


/*
 *----------------------------------------------------------------------
 *
 * Sig_SetActionStub --
 *
 *	MIG stub for the Sig_SetAction Sprite call.
 *
 * Results:
 *	Returns KERN_SUCCESS.  Fills in the Sprite status code from
 *	Sig_SetAction and the "pending signals" flag.  Fills in the old
 *	action information for the signal.
 *
 * Side effects:
 *	Registers the address of the process's trampoline code.
 *
 *----------------------------------------------------------------------
 */

kern_return_t
Sig_SetActionStub(serverPort, sigNum, newAction, sigTrampProc, statusPtr,
		  oldActionPtr, sigPendingPtr)
    mach_port_t serverPort;	/* request port */
    int sigNum;
    Sig_Action newAction;
    vm_address_t sigTrampProc;	/* address of trampoline code */
    ReturnStatus *statusPtr;	/* OUT */
    Sig_Action *oldActionPtr;	/* OUT */
    boolean_t *sigPendingPtr;	/* OUT: is there a signal pending */
{
    Proc_ControlBlock *procPtr;

#ifdef lint
    serverPort = serverPort;
#endif

    *statusPtr = Sig_SetAction(sigNum, &newAction, oldActionPtr);

    procPtr = Proc_GetCurrentProc();
    Proc_Lock(procPtr);
    procPtr->sigTrampProc = (Address)sigTrampProc;
    Proc_Unlock(Proc_AssertLocked(procPtr));

    *sigPendingPtr = Sig_Pending(procPtr);
    return KERN_SUCCESS;
}


/*
 *----------------------------------------------------------------------
 *
 * Sig_SetHoldMaskStub --
 *
 *	MIG stub for the Sig_SetHoldMask Sprite call.
 *
 * Results:
 *	Returns KERN_SUCCESS.  Fills in the old signal mask and "pending 
 *	signals" flag.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

kern_return_t 
Sig_SetHoldMaskStub(serverPort, newMask, oldMaskPtr, sigPendingPtr)
    mach_port_t serverPort;	/* request port */
    int newMask;
    int *oldMaskPtr;		/* OUT */
    boolean_t *sigPendingPtr;	/* OUT: is there a signal pending */
{
#ifdef lint
    serverPort = serverPort;
#endif

    (void)Sig_SetHoldMask(newMask, oldMaskPtr);
    *sigPendingPtr = Sig_Pending(Proc_GetCurrentProc());
    return KERN_SUCCESS;
}


/*
 *----------------------------------------------------------------------
 *
 * Sig_ReturnStub --
 *
 *	Clean up after a user signal handler: restore the signal mask, and 
 *	resume whatever the process was doing before it took the signal.
 *
 * Results:
 *	Returns KERN_SUCCESS.
 *
 * Side effects:
 *	Kills the process if it passed in a bogus pointer.
 *
 *----------------------------------------------------------------------
 */

kern_return_t
Sig_ReturnStub(serverPort, userSigContext)
    mach_port_t serverPort;
    vm_address_t userSigContext; /* context from call to handler */
{
    Sig_Context sigContext;	/* local copy of context */
    ReturnStatus status;
    Proc_ControlBlock *procPtr;

#ifdef lint
    serverPort = serverPort;
#endif

    procPtr = Proc_GetCurrentProc();
    Proc_Lock(procPtr);
    if (sigDebug) {
	printf("sigreturn: context at 0x%x\n", userSigContext);
    }
    status = Vm_CopyIn(sizeof(Sig_Context), (Address)userSigContext,
		       (Address)&sigContext);
    if (status != SUCCESS) {
	printf("%s: pid %x passed in bogus signal context address (0x%x)\n.",
	       "Sig_ReturnStub", procPtr->processID, userSigContext);
	(void)Sig_SendProc(Proc_AssertLocked(procPtr), SIG_KILL, FALSE,
			   PROC_BAD_STACK, (Address)0);
	goto done;
    }

    SigUpdateHoldMask(Proc_AssertLocked(procPtr), sigContext.oldHoldMask);

    /* 
     * Abort the thread's current call and force it to resume execution 
     * where it left off.
     */
    Sig_RestoreAfterSignal(Proc_AssertLocked(procPtr), &sigContext);

 done:
    Proc_Unlock(Proc_AssertLocked(procPtr));
    return KERN_SUCCESS;
}


/*
 *----------------------------------------------------------------------
 *
 * Sig_GetSignalStub --
 *
 *	Get the information for a signal to be handled.
 *
 * Results:
 *	Returns KERN_SUCCESS.  Fills in the Sprite status code.  If 
 *	successful, fills in the address of the signal handler and fills in
 *	the arguments to pass to the handler.  If there is in fact no
 *	need to call a signal handler, a null address is returned for the 
 *	handler.
 *
 * Side effects:
 *	Removes the signal from the pending signals mask and puts it in the 
 *	hold mask.
 *
 *----------------------------------------------------------------------
 */

kern_return_t
Sig_GetSignalStub(serverPort, statusPtr, handlerProcPtr, sigNumPtr, sigCodePtr,
		  userSigContext, sigAddrPtr)
    mach_port_t serverPort;
    ReturnStatus *statusPtr;	/* OUT: Sprite status code */
    vm_address_t *handlerProcPtr; /* OUT: address of signal handler */
    int *sigNumPtr;		/* OUT: Sprite signal number */
    int *sigCodePtr;		/* OUT: Sprite signal subcode */
    vm_address_t userSigContext; /* user address to fill in */
    vm_address_t *sigAddrPtr;	/* OUT: address of the fault */
{
    Proc_ControlBlock *procPtr;
    ReturnStatus status = SUCCESS;
    Boolean suspended = FALSE;	/* was the process's thread suspended? */
    Sig_Stack sigStack;		/* info for the signal handler */
    Sig_Context sigContext;	/* info to restore after the handler returns */

#ifdef lint
    serverPort = serverPort;
#endif

    sigStack.contextPtr = &sigContext;

    procPtr = Proc_GetCurrentProc();
    Proc_Lock(procPtr);

    if (!Sig_Handle(Proc_AssertLocked(procPtr), TRUE, &suspended,
		    &sigStack, (Address *)handlerProcPtr)) {
	*handlerProcPtr = USER_NIL;
	goto done;
    }

    *sigNumPtr = sigStack.sigNum;
    *sigCodePtr = sigStack.sigCode;
    *sigAddrPtr = sigStack.sigAddr;
    status = Vm_CopyOut(sizeof(Sig_Context), (Address)&sigContext,
			(Address)userSigContext);

 done:
    if (suspended) {
	Proc_MakeReady(Proc_AssertLocked(procPtr));
    }
    Proc_Unlock(Proc_AssertLocked(procPtr));
    *statusPtr = status;
    return KERN_SUCCESS;
}
