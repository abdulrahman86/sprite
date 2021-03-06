/* 
 * vmTotals.c --
 *
 *	Routines to total up counts and times from a Vm_Stat struct.
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
static char rcsid[] = "$Header: /user5/kupfer/spriteserver/src/cmds/vmstat/RCS/vmTotals.c,v 1.1 92/07/13 14:07:15 kupfer Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include <spriteTime.h>
#include <vmStat.h>
#include <sprited/vmTypes.h>


/*
 *----------------------------------------------------------------------
 *
 * TotalPageCounts --
 *
 *	Total up the per segment type page counts.
 *
 * Results:
 *	Fills in the totals.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

void
TotalPageCounts(statsPtr, pagesReadPtr, partialPagesReadPtr, pagesZeroedPtr,
		pagesWrittenPtr, pagesCleanedPtr)
    Vm_Stat *statsPtr;		/* holds the counts to sum up */
    int *pagesReadPtr;		/* OUT: sum of pagesRead counts */
    int *partialPagesReadPtr;	/* OUT: sum of partialPagesRead counts */
    int *pagesZeroedPtr;	/* OUT: sum of pagesZeroed counts */
    int *pagesWrittenPtr;	/* OUT: sum of pagesWritten counts */
    int *pagesCleanedPtr;	/* OUT: sum of pagesCleaned counts */
{
    int i;
    int pagesRead = 0;
    int partialPagesRead = 0;
    int pagesZeroed = 0;
    int pagesWritten = 0;
    int pagesCleaned = 0;

    for (i = 0; i < VM_NUM_SEGMENT_TYPES; i++) {
	pagesRead += statsPtr->pagesRead[i];
	partialPagesRead += statsPtr->partialPagesRead[i];
	pagesZeroed += statsPtr->pagesZeroed[i];
	pagesWritten += statsPtr->pagesWritten[i];
	pagesCleaned += statsPtr->pagesCleaned[i];
    }

    *pagesReadPtr = pagesRead;
    *partialPagesReadPtr = partialPagesRead;
    *pagesZeroedPtr = pagesZeroed;
    *pagesWrittenPtr = pagesWritten;
    *pagesCleanedPtr = pagesCleaned;
}


/*
 *----------------------------------------------------------------------
 *
 * TotalPageTimes --
 *
 *	Total up the per-segment-type page operation times.
 *
 * Results:
 *	Fills in the totals.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

void
TotalPageTimes(statsPtr, readTimePtr, readCopyTimePtr, writeTimePtr)
    Vm_Stat *statsPtr;		/* holds the per-segment-type times */
    Time *readTimePtr;		/* OUT: total of read times */
    Time *readCopyTimePtr;	/* OUT: total of read-copy times */
    Time *writeTimePtr;		/* OUT: total of write times */
{
    int i;
    Time readTime;
    Time readCopyTime;
    Time writeTime;

    readTime = time_ZeroSeconds;
    readCopyTime = time_ZeroSeconds;
    writeTime = time_ZeroSeconds;
    for (i = 0; i < VM_NUM_SEGMENT_TYPES; i++) {
	Time_Add(statsPtr->readTime[i], readTime, &readTime);
	Time_Add(statsPtr->readCopyTime[i], readCopyTime, &readCopyTime);
	Time_Add(statsPtr->writeTime[i], writeTime, &writeTime);
    }

    *readTimePtr = readTime;
    *readCopyTimePtr = readCopyTime;
    *writeTimePtr = writeTime;
}
