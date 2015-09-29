/* 
 * sysSysCall.c --
 *
 *	"System call" support.  Sprite "system call" requests, exception
 *	messages, and VM requests all go through here.
 *	
 *	The basic plan is as follows.  The main server loop reads in a
 *	message.  It looks at the destination port to determine how to
 *	process the message.  Sprite requests and exceptions are treated
 *	the same: a user thread is created to handle the request.  For VM
 *	requests, the message is put on a queue for the appropriate VM
 *	segment, and a server thread is started to handle the request.  In
 *	all cases, the thread calls the Mach routine that interprets the
 *	message and calls the Sprite service routine.  The Mach routine
 *	sets up the reply message, which the thread sends.  The thread
 *	frees up the request and reply messages and exits.  (Actually, for
 *	VM requests the thread only exits after all requests in the
 *	segment's queue have been handled.)
 *
 * Copyright 1991 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that this copyright
 * notice appears in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user5/kupfer/spriteserver/src/sprited/sys/RCS/sysSysCall.c,v 1.16 92/07/17 16:35:35 kupfer Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include <sprite.h>
#include <ckalloc.h>
#include <list.h>
#include <mach.h>
#include <mach_error.h>
#include <stdio.h>
#include <stdlib.h>

#include <main.h>
#include <proc.h>
#include <status.h>
#include <sync.h>
#include <sys.h>
#include <sysInt.h>
#include <sysCallNums.h>
#include <timer.h>
#include <utils.h>
#include <vm.h>

/* 
 * Debugging and instrumentation information: 
 * - the number of requests that are currently being processed (exceptions 
 *   and system calls)
 * - number of requests for each call number
 * - if profiling is turned on, the total time spent doing each call. 
 * Plus a lock to protect it all.
 */

static int sysRequestsInProgress = 0;

Sys_CallCount sys_CallCounts[SYS_NUM_CALLS];
Boolean sys_CallProfiling = FALSE;

static Sync_Lock requestCountLock =
    Sync_LockInitStatic("Sys:requestCountLock");

static Boolean sysRequestDebug = FALSE; /* flag to enable various debug 
					 * printf's */ 

/* 
 * Lock to protect the list of free Sys message buffers.
 */
static List_Links freeBufferListHdr;
#define freeBufferList	(&freeBufferListHdr)

static Sync_Lock freeListLock = Sync_LockInitStatic("Sys:freeListLock");
#define LOCKPTR (&freeListLock)

/* 
 * Buffer instrumentation.  Keep track of how many buffers have been 
 * allocated.  Complain if we start allocating excessive numbers of 
 * them.  These variables are all protected by the monitor lock.  
 * BUFCNT_COMPLAINT_THROTTLE is to keep us from complaining too often.
 */

#define BUFCNT_COMPLAINT_THROTTLE	50 
				/* number of alloc's between complaints */

static int numBuffersAlloc = 0;	/* number of buffers that exist */
static int maxBuffersAlloc;	/* soft limit for allocating buffers */
static int allocsSinceComplaint = BUFCNT_COMPLAINT_THROTTLE; 
				/* number of allocations since the 
				 * last complaint, initialized so that
				 * we complain the very first time we
				 * go over the limit */

/* 
 * For each MIG module, there is a top-level routine that handles 
 * argument marshalling and unmarshalling and calling the correct 
 * server routine.  The return value tells whether the request message 
 * is actually for a procedure in the given module.
 */

typedef boolean_t (*MigDemuxProc) _ARGS_((mach_msg_header_t *requestPtr,
					  mach_msg_header_t *replyPtr));

/* 
 * This structure defines the arguments that are passed to a new "user 
 * thread" (i.e., a thread used to satisfy requests from user 
 * processes). 
 */

typedef struct {
    Proc_ControlBlock *procPtr;	/* user process being serviced */
    MigDemuxProc serverProc;	/* per-module routine to process the request */
    char *serverName;		/* printable name for serverProc */
    Sys_MsgBuffer *requestPtr;	/* buffer with the request message */
    Sys_MsgBuffer *replyPtr;	/* buffer with the reply message */
} UserThreadInfo;


/* 
 * There is a single port set through which all requests come.  The 
 * ports that belong to the set are system call request ports, 
 * exception ports, and VM segment (memory object) requests.  Note 
 * that all of these ports are named to correspond to data structures, 
 * for fast lookup when a request comes in.
 */
mach_port_t sys_RequestPort;


/* 
 * Temporary declarations, until the Mach header files get it right.
 */

extern mach_msg_return_t mach_msg _ARGS_((mach_msg_header_t *msg,
			mach_msg_option_t option, mach_msg_size_t send_size,
			mach_msg_size_t rcv_size, mach_port_t rcv_name,
			mach_msg_timeout_t timeout, mach_port_t notify));
extern void mach_msg_destroy _ARGS_((mach_msg_header_t *msg));
extern boolean_t spriteSrv_server _ARGS_((mach_msg_header_t *requestPtr,
			mach_msg_header_t *replyPtr));
extern boolean_t exc_server _ARGS_((mach_msg_header_t *requestPtr,
			mach_msg_header_t *replyPtr));


/* Forward references */

static void DemuxRequest _ARGS_((Sys_MsgBuffer *requestPtr,
			Sys_MsgBuffer *replyPtr));
static Boolean DoReturn1 _ARGS_((Sys_MsgBuffer *requestPtr,
			Sys_MsgBuffer *replyPtr));
static Boolean DoReturn2 _ARGS_((Proc_ControlBlock *procPtr,
			Sys_MsgBuffer *requestPtr, Sys_MsgBuffer *replyPtr));
static Boolean HealthyProc _ARGS_((Proc_ControlBlock *procPtr));
static void InitPorts _ARGS_((void));
static Sys_MsgBuffer *SysGetMsgBuffer _ARGS_((void));
static void SysFreeMsgBuffer _ARGS_((Sys_MsgBuffer *bufPtr));
static void SysNewServiceThread _ARGS_((Proc_ControlBlock *procPtr,
			MigDemuxProc serverProc, char *serverName,
			Sys_MsgBuffer *requestPtr, Sys_MsgBuffer *replyPtr));
static void SysSetUpReply _ARGS_((Sys_MsgBuffer *requestPtr,
			Sys_MsgBuffer *replyPtr, kern_return_t retCode));
static any_t UserThreadInit _ARGS_((any_t args));


/*
 *----------------------------------------------------------------------
 *
 * SysInitSysCall --
 *
 *	Initialization for getting Sprite requests.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Initializes the list of message buffers and related variables. 
 *	Creates the port set for getting requests.
 *
 *----------------------------------------------------------------------
 */
    
void
SysInitSysCall()
{
    List_Init(freeBufferList);
    InitPorts();

    /* 
     * Allow 2 buffers for system calls and exceptions per active process.
     * Allow vm_MaxPendingRequests pending requests per VM segment, at 2
     * buffers per request.  Allow for simultaneous operations on 2
     * segments per active process.  Figure on at most, oh, 5 active
     * processes.  Of course, this is all untuned.
     */
    maxBuffersAlloc = 5 * (2 + (2 * (2 * vm_MaxPendingRequests)));
}


/*
 *----------------------------------------------------------------------
 *
 * Sys_ServerLoop --
 *
 *	Code to handle Sprite "system call" and exception requests.  
 *	Never returns.
 *	
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
    
void
Sys_ServerLoop()
{
    kern_return_t kernStatus;
    Sys_MsgBuffer *requestPtr;	/* buffer for reading request message */
    Sys_MsgBuffer *replyPtr;	/* buffer for reply message */

    for (;;) {
	requestPtr = SysGetMsgBuffer();
	replyPtr = SysGetMsgBuffer();
	/* 
	 * A word on synchronization:  if a process is destroyed 
	 * right after we get a message from it, we want to drop the 
	 * message on the floor, rather than possibly associate the 
	 * message with a new process that happens to have the same 
	 * PCB address.  So we only reap dead processes (reclaim PCB
	 * entries) after getting the procPtr for the message port and
	 * deciding what to do with the message.
	 * 
	 * If the process is in the middle of being destroyed, either 
	 * PROC_NO_MORE_REQUESTS or PROC_BEING_SERVED will get set, and the 
	 * thread that locked the pcb last will have to modify its behavior 
	 * accordingly (see Proc_Kill and HealthyProc).
	 * 
	 * XXX Putting the reaper code in the main server loop seems like a 
	 * potential performance lose.  Message sequence numbers could be 
	 * used to verify that the message goes with the current PCB, 
	 * though one could still imagine a worst-case scenario where a 
	 * process gets killed before any of its requests are processed.  
	 * Another alternative is to at least put the VM server loop in a 
	 * second thread.
	 */
	if (sysRequestDebug) {
	    printf("ServerLoop: waiting for request...");
	}
	kernStatus = mach_msg(&requestPtr->bufPtr->Head, MACH_RCV_MSG, 0,
			      SYS_MAX_REQUEST_SIZE, sys_RequestPort,
			      MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	if (sysRequestDebug) {
	    printf("got one\n");
	}
	if (kernStatus != MACH_MSG_SUCCESS) {
	    printf("Sys_ServerLoop: receive failed: %s\n",
		   mach_error_string(kernStatus));
	} else {
	    /* 
	     * If this is the "return1" request, answer it now.  No 
	     * forking, no call overhead.  Otherwise, go through the longer 
	     * call path.
	     */
	    if (!DoReturn1(requestPtr, replyPtr)) {
		DemuxRequest(requestPtr, replyPtr);
	    }
	}
	
	Proc_Reaper();
    }
}


/*
 *----------------------------------------------------------------------
 *
 * DemuxRequest --
 *
 *	Pass a request to the correct module and deal with the 
 *	results. 
 *
 * Results:
 *	None.
 *
 * Side effects:
 * 	Invokes a MIG RPC stub and sends the reply message.  
 *	Frees the request and reply buffers.
 *
 *----------------------------------------------------------------------
 */

static void
DemuxRequest(requestPtr, replyPtr)
    Sys_MsgBuffer *requestPtr;	/* request message buffer */
    Sys_MsgBuffer *replyPtr;	/* reply message buffer */
{
    Vm_Segment *segPtr;
    Proc_ControlBlock *procPtr;
    mach_msg_header_t *msgPtr; /* pointer to actual Mach message (request) */

    msgPtr = &requestPtr->bufPtr->Head;

    /* 
     * Look at the port the message was sent to and pass the message 
     * to the appropriate module.
     */
    
    if ((procPtr = Proc_ExceptionToPCB(msgPtr->msgh_local_port))
	  != NULL) {
	if (sysRequestDebug) {
	    printf("--exception request--\n");
	}
	SysNewServiceThread(procPtr, exc_server, "exception server",
			    requestPtr, replyPtr);
    } else if ((procPtr = Proc_SyscallToPCB(msgPtr->msgh_local_port))
	       != NULL) {
	if (sysRequestDebug) {
	    printf("--syscall request (%d for %x)--\n",
		   msgPtr->msgh_id, procPtr->processID);
	}
	SysNewServiceThread(procPtr, spriteSrv_server, "syscall server",
			    requestPtr, replyPtr);
    } else if ((segPtr = Vm_PortToSegment(msgPtr->msgh_local_port))
	       != NULL) {
	if (sysRequestDebug) {
	    printf("--VM request--\n");
	}
	Vm_EnqueueRequest(segPtr, requestPtr, replyPtr);
	Vm_SegmentRelease(segPtr);
    } else {
	printf("DemuxRequest: unclaimed request port.\n");
	SysSetUpReply(requestPtr, replyPtr, MIG_BAD_ARGUMENTS);
	Sys_ReplyAndFree(requestPtr, replyPtr);
    }
}


/*
 *----------------------------------------------------------------------
 *
 * SysNewServiceThread --
 *
 *	Start up a thread to service a system call or exception.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Creates a user thread, which passes the MIG message to the given
 *	module, sends the reply, and frees the given message buffers.
 *
 *----------------------------------------------------------------------
 */

static void
SysNewServiceThread(procPtr, serverProc, serverName, requestPtr, replyPtr)
    Proc_ControlBlock *procPtr;	/* the user process we're handling */
    MigDemuxProc serverProc;	/* the fcn. to unmarshall the message */
    char *serverName;		/* name for serverProc */
    Sys_MsgBuffer *requestPtr;	/* message buffer holding the request */
    Sys_MsgBuffer *replyPtr;	/* message buffer to hold the reply */
{
    cthread_t newThreadPtr;
    UserThreadInfo *infoPtr;	/* arguments passed to new thread */

    Proc_Lock(procPtr);

    /* 
     * Sanity check.
     */
    if (!(procPtr->genFlags & PROC_USER)) {
	panic("SysNewServiceThread: not a user process.\n");
    }

    /* 
     * If the process shouldn't be submitting requests or is about to die,
     * drop the request on the floor (don't even send a reply).
     */
    if (!HealthyProc(procPtr)) {
	Proc_Unlock(Proc_AssertLocked(procPtr));
	mach_msg_destroy(&requestPtr->bufPtr->Head);
	mach_msg_destroy(&replyPtr->bufPtr->Head);
	replyPtr->bufPtr->RetCode = MIG_NO_REPLY;
	Sys_ReplyAndFree(requestPtr, replyPtr);
	return;
    }

    /* 
     * The reason for making this check is that in native Sprite code
     * a process can only do one system call at a time (pretend that
     * trap handling is a system call), and there may be code in other
     * modules that depends on this sequentiality.  XXX Should we also
     * verify that the Mach thread for the user process is suspended?
     */
    if (procPtr->genFlags & PROC_BEING_SERVED) {
	printf("SysNewServiceThread: pid %x is already being serviced.\n",
	       procPtr->processID);
	Proc_Unlock(Proc_AssertLocked(procPtr));
	SysSetUpReply(requestPtr, replyPtr, MIG_REMOTE_ERROR);
	Sys_ReplyAndFree(requestPtr, replyPtr);
	return;
    }

    /* 
     * Mark the process so that if someone wants to kill it 
     * asynchronously, the process's user thread will be able to back 
     * out gracefully.
     */
    procPtr->genFlags |= PROC_BEING_SERVED;
    Proc_Unlock(Proc_AssertLocked(procPtr));

    infoPtr = (UserThreadInfo *)ckalloc(sizeof(UserThreadInfo));
    if (infoPtr == NULL) {
	panic("SysNewServiceThread: out of memory.\n");
    }
    infoPtr->procPtr = procPtr;
    infoPtr->serverProc = serverProc;
    infoPtr->serverName = serverName;
    infoPtr->requestPtr = requestPtr;
    infoPtr->replyPtr = replyPtr;

    newThreadPtr = cthread_fork(UserThreadInit, (any_t)infoPtr);
    if (newThreadPtr == NO_CTHREAD) {
	/* "can't happen" */
	panic("SysNewServiceThread: no more threads.\n");
    }

    cthread_detach(newThreadPtr);
}


/*
 *----------------------------------------------------------------------
 *
 * HealthyProc --
 *
 *	Determine if a process is able to take an exception or make a 
 *	Sprite request.
 *
 * Results:
 *	Returns TRUE if the process is judged healthy.  Returns FALSE 
 *	if the process is sick and its request should be ignored.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static Boolean
HealthyProc(procPtr)
    Proc_ControlBlock *procPtr;	/* the process to judge */
{
    Boolean okay = TRUE;

    if (procPtr->state != PROC_READY) {
	printf("SysNewServiceThread: pid %x is %s: orphan request.\n",
	       procPtr->processID, Proc_StateName(procPtr->state));
	okay = FALSE;
    }
    if (procPtr->genFlags & PROC_NO_MORE_REQUESTS) {
	printf("SysNewServiceThread: pid %x: orphan request\n",
	       procPtr->processID);
	okay = FALSE;
    }
#if 0
    /* 
     * Unfortunately, this check causes more trouble than it's worth, 
     * particularly because sys_ShuttingDown doesn't turn on until we're
     * killing the server processes.  The Right Thing is probably to change
     * sys_ShuttingDown from a Boolean to an int (giving it different 
     * values for each shutdown stage), but it doesn't seem worth making
     * that change right now.
     */
    if (!okay && !sys_ShuttingDown && main_DebugFlag) {
	panic("SysNewServiceThread: process state error.\n");
    }
#endif

    return okay;
}


/*
 *----------------------------------------------------------------------
 *
 * UserThreadInit --
 *
 *	Initialization for a user thread.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Sets the name and context for the thread.  Does all the work 
 *	promised by SysNewServiceThread.  Frees the initialization 
 *	info for the thread.  Updates the Sprite system call stats.
 *
 *----------------------------------------------------------------------
 */

static any_t
UserThreadInit(arg)
    any_t arg;			/* arguments for the new thread */
{
    UserThreadInfo *infoPtr = (UserThreadInfo *)arg;
    char threadName[100];
    boolean_t msgAccepted = TRUE; /* was the message digestable by the 
				   * module */
    Proc_ControlBlock *procPtr;	/* the user process being serviced */
    kern_return_t kernStatus;
    Boolean doCall;		/* try to process the request? */
    Time startTime, endTime;	/* start/end times for processing the req. */
    Time elapsedTime;		/* total time for processing the request */
    int sysCallNumber = -1;	/* which system call, if any */
    Sys_CallCount *callPtr;	/* pointer into syscall counters array */

    elapsedTime = time_ZeroSeconds;

    /* 
     * If debugging is turned on, wire the current C Thread to a 
     * kernel thread, so that gdb will be able to find it.
     */
    if (main_DebugFlag) {
	cthread_wire();
    }

    procPtr = infoPtr->procPtr;
    sprintf(threadName, "user pid %x", procPtr->processID);
    cthread_set_name(cthread_self(), threadName);
    Proc_SetCurrentProc(procPtr);

    /* 
     * Instrumentation.
     */
    if (DoReturn2(procPtr, infoPtr->requestPtr, infoPtr->replyPtr)) {
	cthread_set_name(cthread_self(), (char *)0);
	cthread_exit(0);
	return 0;		/* lint */
    }

    /* 
     * Update the number of outstanding system requests.  This has to wait
     * until the process for the thread is recorded, in case Sync_GetLock
     * has to sleep.
     */
    Sync_GetLock(&requestCountLock);
    sysRequestsInProgress++;
    Sync_Unlock(&requestCountLock);

    /* 
     * If the process is suspended (which presumably happened after it had
     * posted the current request), sit here until it gets resumed.  
     */
    
    Proc_Lock(procPtr);
    while (procPtr->state == PROC_SUSPENDED) {
	Proc_UnlockAndSwitch(Proc_AssertLocked(procPtr), PROC_SUSPENDED);
	Proc_Lock(procPtr);
    }
    doCall = (procPtr->genFlags & PROC_NO_MORE_REQUESTS) == 0;
    Proc_Unlock(Proc_AssertLocked(procPtr));

    procPtr->copyInTime = time_ZeroSeconds;
    procPtr->copyOutTime = time_ZeroSeconds;

    /* 
     * If the process is still alive, record the system call number and
     * call the given routine to process the message.  If it's dead, mark
     * the reply buffer so that no reply is sent.
     */

    if (doCall) {
	if (infoPtr->serverProc == spriteSrv_server) {
	    sysCallNumber = (infoPtr->requestPtr->bufPtr->Head.msgh_id -
			     SYS_CALL_BASE);
	    if (sys_CallProfiling) {
		Timer_GetTimeOfDay(&startTime, NULL, NULL);
	    } else {
		startTime = time_ZeroSeconds;
	    }
	}
	msgAccepted = (*infoPtr->serverProc)(
				&infoPtr->requestPtr->bufPtr->Head,
				&infoPtr->replyPtr->bufPtr->Head);
	if (sys_CallProfiling && !Time_EQ(startTime, time_ZeroSeconds)) {
	    Timer_GetTimeOfDay(&endTime, NULL, NULL);
	    Time_Subtract(endTime, startTime, &elapsedTime);
	}
	if (!msgAccepted) {
	    printf("UserThreadInit: %s didn't take the message.\n",
		   infoPtr->serverName);
	}
    } else {
	infoPtr->replyPtr->bufPtr->RetCode = MIG_NO_REPLY;
    }

    /* 
     * Lock the process before sending the reply, so that we can atomically 
     * clear the "being served" flag.
     */
    Proc_Lock(procPtr);
    Sys_ReplyAndFree(infoPtr->requestPtr, infoPtr->replyPtr);
    ckfree(infoPtr);

    /* 
     * Instrumentation: decrement the count of in-progress requests and
     * update the instrumentation information.  Make sure that profiling
     * was turned on for the entire call.  This should be done before the
     * process is unlocked, in case it makes a new request while we are
     * waiting for requestCountLock.
     */
    Sync_GetLock(&requestCountLock);
    sysRequestsInProgress--;
    if (sysRequestDebug) {
	printf("user request done, %d still in progress\n",
	       sysRequestsInProgress);
    }
    if (sysCallNumber != -1) {
	if (sysCallNumber < 0 || sysCallNumber >= SYS_NUM_CALLS) {
	    panic("UserThreadInit: bogus system call number.\n");
	}
	callPtr = sys_CallCounts + sysCallNumber;
	callPtr->numCalls++;
	Time_Add(callPtr->timeSpent, elapsedTime, &callPtr->timeSpent);
	Time_Add(procPtr->copyInTime, callPtr->copyInTime,
		 &callPtr->copyInTime);
	Time_Add(procPtr->copyOutTime, callPtr->copyOutTime,
		 &callPtr->copyOutTime);
    }
    Sync_Unlock(&requestCountLock);

    /* 
     * Final pcb bookkeeping.
     */
    
    if (procPtr->genFlags & PROC_NO_MORE_REQUESTS) {
	Proc_Unlock(Proc_AssertLocked(procPtr));
	Proc_Exit(1);
    }

    procPtr->genFlags &= ~PROC_BEING_SERVED;
    if (procPtr->genFlags & PROC_NEEDS_WAKEUP) {
	procPtr->genFlags &= ~PROC_NEEDS_WAKEUP;
	kernStatus = thread_resume(procPtr->thread);
	if (kernStatus != KERN_SUCCESS) {
	    printf("UserThreadInit: couldn't resume pid %x: %s\n",
		   procPtr->processID, mach_error_string(kernStatus));
	}
    }

    Proc_Unlock(Proc_AssertLocked(procPtr));

    cthread_set_name(cthread_self(), (char *)0);
    cthread_exit(0);
    return 0;			/* lint */
}


/*
 *----------------------------------------------------------------------
 *
 * Sys_ReplyAndFree --
 *
 *	Send a reply message for a MIG RPC and free the message buffers.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Assuming there were no errors, a reply message is sent.
 *	All the associated buffers and ports are freed up.
 *
 *----------------------------------------------------------------------
 */

void
Sys_ReplyAndFree(requestPtr, replyPtr)
    Sys_MsgBuffer *requestPtr;
    Sys_MsgBuffer *replyPtr;
{
    kern_return_t kernStatus;

    /* 
     * Look at the return buffer and decide whether to send the reply 
     * message.  Unlike mach_msg_server, we take responsibility for freeing 
     * up resources even if no reply is to be sent.
     */
    
    if (replyPtr->bufPtr->RetCode != KERN_SUCCESS) {
	/* 
	 * Null out the reply port right in the request message, so that
	 * mach_msg_destroy doesn't destroy it when it destroys the request
	 * message.  This is so we can send an error message.
	 */
	requestPtr->bufPtr->Head.msgh_remote_port = MACH_PORT_NULL;
	mach_msg_destroy(&requestPtr->bufPtr->Head);
    }

    if (replyPtr->bufPtr->Head.msgh_remote_port == MACH_PORT_NULL ||
	replyPtr->bufPtr->RetCode == MIG_NO_REPLY) {
	/* 
	 * There's no reply port or we're not supposed to send a reply, so 
	 * free up resources held by the reply.
	 */
	mach_msg_destroy(&replyPtr->bufPtr->Head);
	goto freeBuffers;
    }

    /* 
     * Send the reply.
     */
	
    kernStatus = mach_msg(&replyPtr->bufPtr->Head, MACH_SEND_MSG,
			  replyPtr->bufPtr->Head.msgh_size, 0, MACH_PORT_NULL,
			  MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    switch (kernStatus) {
    case MACH_MSG_SUCCESS:
	break;
    case MACH_SEND_INVALID_DEST:
	/* the reply can't be delivered, so destroy it */
	mach_msg_destroy(&replyPtr->bufPtr->Head);
	break;
    case MACH_RCV_TOO_LARGE:
	/* the kernel destroyed the request */
	break;
    default:
	/* "shouldn't happen" */
	printf("Sys_ReplyAndFree: couldn't send reply: %s\n", 
	       mach_error_string(kernStatus));
	break;
    }

    /* 
     * Free the request and reply buffers.
     */

 freeBuffers:
    SysFreeMsgBuffer(requestPtr);
    SysFreeMsgBuffer(replyPtr);
}


/*
 *----------------------------------------------------------------------
 *
 * SysGetMsgBuffer --
 *
 *	Get an unused message buffer.
 *
 * Results:
 *	Returns a pointer to a message buffer.
 *
 * Side effects:
 *	Either removes the buffer from the free list or creates a new 
 *	buffer.
 *
 *----------------------------------------------------------------------
 */

static ENTRY Sys_MsgBuffer *
SysGetMsgBuffer()
{
    Sys_MsgBuffer *msgPtr = NULL;
    kern_return_t kernStatus;

    LOCK_MONITOR;

    /* 
     * If the free list is empty, make a new buffer.  If we've gone 
     * over the maximum, complain (but not too often).
     */
    if (!List_IsEmpty(freeBufferList)) {
	msgPtr = (Sys_MsgBuffer *)List_First((List_Links *)freeBufferList);
	List_Remove((List_Links *)msgPtr);
    } else {
	++numBuffersAlloc;
	if (numBuffersAlloc > maxBuffersAlloc) {
	    ++allocsSinceComplaint;
	    if (allocsSinceComplaint > BUFCNT_COMPLAINT_THROTTLE) {
		printf("SysGetMsgBuffer: warning: %d message buffers allocated\n",
		       numBuffersAlloc);
		allocsSinceComplaint = 0;
	    }
	}
	msgPtr = (Sys_MsgBuffer *)ckalloc(sizeof(Sys_MsgBuffer));
	if (msgPtr == NULL) {
	    panic("SysGetMsgBufer: out of memory.\n");
	}
	kernStatus = vm_allocate(mach_task_self(),
				 (vm_address_t *)&msgPtr->bufPtr,
				 SYS_MAX_REQUEST_SIZE, TRUE);
	if (kernStatus != KERN_SUCCESS) {
	    panic("SysGetMsgBuffer: can't allocate buffer: %s\n",
		   mach_error_string(kernStatus));
	}
	List_InitElement((List_Links *)msgPtr);
    }
    UNLOCK_MONITOR;

    return msgPtr;
}


/*
 *----------------------------------------------------------------------
 *
 * SysFreeMsgBuffer --
 *
 *	Returns a message buffer to the free list.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If there aren't too many buffers, the buffer is put at the end 
 *	of the free list.  Otherwise the memory for the buffer is 
 *	freed and the count of buffers is decremented.
 *
 *----------------------------------------------------------------------
 */

static ENTRY void
SysFreeMsgBuffer(msgPtr)
    Sys_MsgBuffer *msgPtr;	/* the buffer to free */
{
    LOCK_MONITOR;
    /* 
     * If we're over the limit on buffers, just free the memory.  
     * Otherwise put the buffer on the free list.
     */
    if (numBuffersAlloc > maxBuffersAlloc) {
	(void)vm_deallocate(mach_task_self(), (vm_address_t)msgPtr->bufPtr,
			    SYS_MAX_REQUEST_SIZE);
	ckfree(msgPtr);
	--numBuffersAlloc;
    } else {
	List_Insert((List_Links *)msgPtr,
		    LIST_ATREAR((List_Links *)freeBufferList));
    }
    UNLOCK_MONITOR;
}


/*
 *----------------------------------------------------------------------
 *
 * InitPorts --
 *
 *	Set up Mach port set for getting requests.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Creates the port set that all requests come through. 
 *
 *----------------------------------------------------------------------
 */

static void
InitPorts()
{
    kern_return_t kernStatus;

    kernStatus = mach_port_allocate(mach_task_self(),
				    MACH_PORT_RIGHT_PORT_SET,
				    &sys_RequestPort);
    if (kernStatus != KERN_SUCCESS) {
	printf("Couldn't allocate request port set: %s\n",
	       mach_error_string(kernStatus));
	exit(1);
    }
}


/*
 *----------------------------------------------------------------------
 *
 * SysSetUpReply --
 *
 *	Set up a reply buffer for a system request.  The reply buffer is 
 *	normally set up by the MIG-generated stub, but for some error cases 
 *	we never let the stub see the message.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Using information from the request buffer, fills in enough of the
 *	reply buffer that the reply can be sent.
 *
 *----------------------------------------------------------------------
 */

static void
SysSetUpReply(requestPtr, replyPtr, retCode)
    Sys_MsgBuffer *requestPtr;	/* request buffer */
    Sys_MsgBuffer *replyPtr;	/* reply buffer */
    kern_return_t retCode;	/* return code to put in the reply */
{
    mach_msg_header_t *inPtr = &requestPtr->bufPtr->Head;
    mig_reply_header_t *outPtr = replyPtr->bufPtr;

    outPtr->Head.msgh_bits = MACH_MSGH_BITS(MACH_MSGH_BITS_REMOTE(
							inPtr->msgh_bits), 0);
    outPtr->Head.msgh_size = sizeof(*outPtr);
    outPtr->Head.msgh_remote_port = inPtr->msgh_remote_port;
    outPtr->Head.msgh_local_port = MACH_PORT_NULL;
    outPtr->Head.msgh_kind = inPtr->msgh_kind;
    outPtr->Head.msgh_id = inPtr->msgh_id + 100;
    
    outPtr->RetCodeType.msgt_name = MACH_MSG_TYPE_INTEGER_32;
    outPtr->RetCodeType.msgt_size = 32;
    outPtr->RetCodeType.msgt_number = 1;
    outPtr->RetCodeType.msgt_inline = TRUE;
    outPtr->RetCodeType.msgt_longform = FALSE;
    outPtr->RetCodeType.msgt_deallocate = FALSE;
    outPtr->RetCodeType.msgt_unused = 0;

    outPtr->RetCode = retCode;
}


/*
 *----------------------------------------------------------------------
 *
 * SysBufferStats --
 *
 *	Print numbers about current syscall buffer utilization.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

void
SysBufferStats()
{
    printf("System call buffers allocated: %d\n", numBuffersAlloc);

}


/*
 *----------------------------------------------------------------------
 *
 * DoReturn1 --
 *
 *	If the request is a Test_Return1, process it.
 *
 * Results:
 *	Returns TRUE if the request was Test_Return1, FALSE otherwise.
 *
 * Side effects:
 *	If the request was Test_Return1, the request and reply buffers are 
 *	freed.
 *
 *----------------------------------------------------------------------
 */

static Boolean
DoReturn1(requestPtr, replyPtr)
    Sys_MsgBuffer *requestPtr;	/* request message buffer */
    Sys_MsgBuffer *replyPtr;	/* reply message buffer */
{
    int callNumber;		/* MIG call number */

    callNumber = requestPtr->bufPtr->Head.msgh_id;
    if (callNumber != SYS_CALL_BASE + TEST_RETURN1_STUB) {
	return FALSE;
    } else {
	if (!spriteSrv_server(&requestPtr->bufPtr->Head,
			      &replyPtr->bufPtr->Head)) {
	    printf("DoReturn1: message not accepted.\n");
	}
	Sys_ReplyAndFree(requestPtr, replyPtr);
	return TRUE;
    }
}


/*
 *----------------------------------------------------------------------
 *
 * DoReturn2 --
 *
 *	If the request is a Test_Return2, process it.
 *
 * Results:
 *	Returns TRUE if the request was Test_Return2, FALSE otherwise.
 *
 * Side effects:
 *	If the request was Test_Return2, the request and reply buffers are 
 *	freed, and the PCB's "being served" flag is cleared.
 *
 *----------------------------------------------------------------------
 */

static Boolean
DoReturn2(procPtr, requestPtr, replyPtr)
    Proc_ControlBlock *procPtr;	/* the process making the request */
    Sys_MsgBuffer *requestPtr;	/* request message buffer */
    Sys_MsgBuffer *replyPtr;	/* reply message buffer */
{
    int callNumber;		/* MIG call number */

    callNumber = requestPtr->bufPtr->Head.msgh_id;
    if (callNumber != SYS_CALL_BASE + TEST_RETURN2_STUB) {
	return FALSE;
    } else {
	if (!spriteSrv_server(&requestPtr->bufPtr->Head,
			      &replyPtr->bufPtr->Head)) {
	    printf("DoReturn2: message not accepted.\n");
	}
	Proc_Lock(procPtr);
	Sys_ReplyAndFree(requestPtr, replyPtr);
	procPtr->genFlags &= ~PROC_BEING_SERVED;
	Proc_Unlock(Proc_AssertLocked(procPtr));
	return TRUE;
    }
}
