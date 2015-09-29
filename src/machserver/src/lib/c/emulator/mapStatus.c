/* 
 * mapStatus.c --
 *
 *	Code to map between Mach and Sprite status codes.
 *	XXX It might make more sense to give Sprite module numbers to each 
 *	Mach system and then use the Mach facilities as much as possible.
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
static char rcsid[] = "$Header: /user5/kupfer/spriteserver/src/lib/c/emulator/RCS/mapStatus.c,v 1.2 92/04/29 21:34:22 kupfer Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include <sprite.h>
#include <mach.h>
#include <mach/error.h>
#include <mach/mig_errors.h>
#include <mach/message.h>
#include <status.h>

/* 
 * Mappings from Mach kernel errors to Sprite errors.
 */

static ReturnStatus kernTable[] = {
	SUCCESS,		/* KERN_SUCCESS */
	SYS_ARG_NOACCESS,	/* KERN_INVALID_ADDRESS */
	SYS_ARG_NOACCESS,	/* KERN_PROTECTION_FAILURE */
	VM_SEG_TOO_LARGE,	/* KERN_NO_SPACE */
	SYS_INVALID_ARG,	/* KERN_INVALID_ARGUMENT */
	FAILURE,		/* KERN_FAILURE */
	MACH_RESOURCE_SHORTAGE,	/* KERN_RESOURCE_SHORTAGE */
	MACH_OTHER_ERROR,	/* KERN_NOT_RECEIVER */
	GEN_EACCES,		/* KERN_NO_ACCESS */
	VM_SEGMENT_DESTROYED,	/* KERN_MEMORY_FAILURE */
	MACH_VM_ERROR,		/* KERN_MEMORY_ERROR */
	MACH_OTHER_ERROR,	/* KERN_ALREADY_IN_SET (obsolete) */
	MACH_NOT_IN_SET,	/* KERN_NOT_IN_SET */
	MACH_NAME_EXISTS,	/* KERN_NAME_EXISTS */
	MACH_OTHER_ERROR,	/* KERN_ABORTED */
	MACH_INVALID_NAME,	/* KERN_INVALID_NAME */
	MACH_DEAD_TASK,		/* KERN_INVALID_TASK */
	MACH_INVALID_RIGHT,	/* KERN_INVALID_RIGHT */
	GEN_INVALID_ARG,	/* KERN_INVALID_VALUE */
	MACH_UREFS_OVERFLOW,	/* KERN_UREFS_OVERFLOW */
	MACH_IPC_ERROR,		/* KERN_INVALID_CAPABILITY */
	MACH_RIGHT_EXISTS,	/* KERN_RIGHT_EXISTS */
	MACH_IPC_ERROR,		/* KERN_INVALID_HOST */
	MACH_VM_ERROR,		/* KERN_MEMORY_PRESENT */
};
static kern_return_t minKernStatus = 0;
static kern_return_t maxKernStatus =
    sizeof(kernTable)/sizeof(ReturnStatus) - 1;


/*
 *----------------------------------------------------------------------
 *
 * Utils_MapMachStatus --
 *
 *	Map a Mach error status to a Sprite error status.
 *	XXX This code really needs to be made more robust, so that the Mach 
 *	guys can make changes without causing us a lot of grief.
 *
 * Results:
 *	Returns the equivalent Sprite code.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

ReturnStatus
Utils_MapMachStatus(kernStatus)
    kern_return_t kernStatus;	/* code to map */
{
    ReturnStatus result = MACH_OTHER_ERROR;

    if (kernStatus >= minKernStatus && kernStatus <= maxKernStatus) {
	result = kernTable[kernStatus];
#ifdef MACH_IPC_COMPAT
    } else if (kernStatus >= RCV_ERRORS_START &&
	       kernStatus <= RCV_ERRORS_END) {
	result = MACH_RCV_ERROR;
    } else if (kernStatus >= SEND_ERRORS_START &&
	       kernStatus <= SEND_ERRORS_END) {
	result = MACH_SEND_ERROR;
#endif
    } else if ((kernStatus & system_emask) == err_mach_ipc) {
	switch (err_get_sub(kernStatus)) {
	case 0:			/* see <mach/message.h> */
	    result = MACH_SEND_ERROR;
	    break;
	case 1:			/* see <mach/message.h> */
	    result = MACH_RCV_ERROR;
	    break;
	default:
	    panic("Utils_MapMachStatus: unexpected IPC error code: 0x%x\n",
		  kernStatus);
	    break;
	}
    } else if (kernStatus >= MIG_TYPE_ERROR &&
	       kernStatus <= MIG_DESTROY_REQUEST) {
	result = MACH_MIG_ERROR;
    }

    return result;
}

