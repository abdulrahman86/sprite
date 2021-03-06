/*
 * vmMach.h
 *
 *     	Machine dependent virtual memory data structures and procedure
 *	headers.
 *
 * Copyright (C) 1989 Digital Equipment Corporation.
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies.
 * Digital Equipment Corporation makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * $Header$ SPRITE (DECWRL)
 */

#ifndef _VMMACH
#define _VMMACH

#ifdef KERNEL
#include <vm3maxConst.h>
#else
#include <kernel/vm3maxConst.h>
#endif

/*
 * Machine dependent data for each software segment.
 */
typedef struct VmMach_SegData {
    int	dummy;
} VmMach_SegData;

/*
 * Machine dependent shared memory data
 */
typedef struct VmMach_SharedData {
    int        *allocVector;           /* Allocated block vector. */
    int         allocFirstFree;         /* First free block. */
} VmMach_SharedData;

/*
 * Structure to allow processes to share regions of their VA with the kernel.
 */

typedef struct VmMach_KernSharedInfo {
    List_Links	links;
    unsigned	firstPage;		/* First virtual page in region. */
    unsigned	lastPage;		/* Last virtual page in region. */
    unsigned	firstPhysPage;		/* First physical page, if region
					 * is in the user mapping region. */
    int		flags;			/* See below. */
} VmMach_KernSharedInfo;

/*
 * Flags in a VmMach_KernSharedInfo.
 */

#define VMMACH_KERN_SHARED_UNCACHEABLE 1	/* Is region uncacheable? */

/*
 * Machine dependent data for each process.
 */
typedef struct VmMach_ProcData {
    struct Vm_Segment	*mapSegPtr;	/* Pointer to segment which is mapped
					 * into this processes address
					 * space. */
    unsigned int	mappedPage;	/* Page in the mapped seg where
					 * the mapping begins. */
    int			pid;		/* Which pid is used to map this
					 * process. */
    unsigned int	modPage;	/* A TLB modified fault occured on this
					 * virtual page - set the modify bit
					 * in the TLB entry if we try to
					 * validate this VA. */
    Address		sharedPtr;	/* Shared memory pointer, in case
					 * shared memory is mapped.
					 * This is really Vm_SegProcList*, 
					 * but made address because of
					 * header file problems.*/
    VmMach_SharedData	sharedData;	/* Data for shared memory. */
    List_Links		kernSharedList;	/* List of VA regions shared with
					 * the kernel. */
} VmMach_ProcData;

/*
 * TLB Map.
 */
extern unsigned *vmMach_KernelTLBMap;

/*
 * Machine dependent functions exported to machine dependent modules.
 */

extern Boolean VmMach_MakeDebugAccessible _ARGS_((unsigned addr));
extern ENTRY ReturnStatus VmMach_TLBFault _ARGS_((Address virtAddr));
extern ReturnStatus VmMach_TLBModFault _ARGS_((Address virtAddr));
extern ReturnStatus VmMach_UserMap _ARGS_((int numBytes, Address addr,
	Address physAddr, Boolean cache, Address *newAddrPtr));
extern ENTRY ReturnStatus VmMach_UserUnmap _ARGS_((Address addr));
extern int VmMachCopyEnd _ARGS_((void));

#endif _VMMACH
