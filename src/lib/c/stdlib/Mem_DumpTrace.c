/* 
 * Mem_DumpTrace.c --
 *
 *	Source code for the "Mem_DumpTrace" library procedure.  See memInt.h
 *	for overall information about how the allocator works.
 *
 * Copyright 1988 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /sprite/src/lib/c/stdlib/RCS/Mem_DumpTrace.c,v 1.1 88/05/20 15:49:20 ouster Exp $ SPRITE (Berkeley)";
#endif not lint

#define MEM_TRACE 1
#include "memInt.h"

/*
 *----------------------------------------------------------------------
 *
 * Mem_DumpTrace --
 *
 *	Dump the allocation trace records for the given size block.  If
 *	the size is specified as -1 then all trace records for all size
 *	blocks are dumped.
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
Mem_DumpTrace(blockSize)
    int	blockSize;
{
    register MemTraceElement	*trPtr;
    int				i, j;

    for (i = 0, trPtr = memTraceArray; i < memNumSizesToTrace; i++, trPtr++) {
	if ((trPtr->traceInfo.size != blockSize) && (blockSize != -1)) {
	    continue;
	}
	if (!(trPtr->traceInfo.flags & MEM_STORE_TRACE) ||
	    (trPtr->traceInfo.flags & MEM_TRACE_NOT_INIT)) {
	    continue;
	}

	(*memPrintProc)(memPrintData, "Trace for size = %d:\n", 
				      trPtr->traceInfo.size);
	(*memPrintProc)(memPrintData, "Caller-PC      Num-blocks  \n");
	for (j = 0; j < MAX_CALLERS_TO_TRACE; j++) {
	    if (trPtr->allocInfo[j].numBlocks == 0) {
		break;
	    }
	    (*memPrintProc)(memPrintData, "%8x         %6d\n", 
					  trPtr->allocInfo[j].callerPC,
					  trPtr->allocInfo[j].numBlocks);
	}
	if  (blockSize != -1) {
	    break;
	}
    }
}
