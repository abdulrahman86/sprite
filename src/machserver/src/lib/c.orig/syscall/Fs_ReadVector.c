/* 
 * Fs_ReadVector.c --
 *
 *	Source code for the Fs_ReadVector library procedure.
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
static char rcsid[] = "$Header: Fs_ReadVector.c,v 1.5 88/07/29 17:08:24 ouster Exp $ SPRITE (Berkeley)";
#endif not lint

#include <sprite.h>
#include <fs.h>
#include <status.h>
#include <stdlib.h>

/*
 *----------------------------------------------------------------------
 *
 * Fs_ReadVector --
 *
 *      The "normal" Fs_ReadVector routine for user code.  Read from the file
 *      indicated by the stream ID into the buffers described in vectorArray.
 *	The vectorArray indicates how much data to read, and amtReadPtr 
 *	is an output parameter that indicates how much data were read.  
 *	A length of zero means end-of-file.
 *
 *	Restarting from a signal is automatically handled by Fs_Read.
 *
 * Results:
 *	Result from Fs_Read.
 *
 * Side effects:
 *	See Fs_Read.
 *
 *----------------------------------------------------------------------
 */

ReturnStatus
Fs_ReadVector(streamID, numVectors, vectorArray, amtReadPtr)
    int		streamID;	/* The user's index into its open file list. */
    int		numVectors;	/* The # of vectors in userVectorArray. */
    Fs_IOVector	vectorArray[];	/* The vectors defining where and how much to
				 * read. */
    int		*amtReadPtr; 	/* The amount of bytes actually read. */
{
    register int 	i;
    register Fs_IOVector *vectorPtr;
    register int	bufSize;
    Address		buffer;
    Address		ptr;
    ReturnStatus	status;

    /*
     * Calculate the total number of bytes to be read.
     */
    bufSize = 0;
    for (i = 0, vectorPtr = vectorArray; i < numVectors; i++, vectorPtr++) {
	if (vectorPtr->bufSize < 0) {
	    return SYS_INVALID_ARG;
	}
	bufSize += vectorPtr->bufSize;
    }

    buffer = (Address) malloc((unsigned) bufSize);
    status = Fs_Read(streamID, bufSize, buffer, amtReadPtr);

    if (status == SUCCESS) {
	register int copyAmount;

	bufSize = *amtReadPtr;

	/*
	 * Copy the data to the individual buffers specified in the vectorArray.
	 */
	ptr = buffer;
	for (i = 0, vectorPtr = vectorArray;
		(i < numVectors) && (bufSize > 0); 
		i++, vectorPtr++) {

	    if (bufSize < vectorPtr->bufSize) {
		copyAmount = bufSize;
		vectorPtr->bufSize = bufSize;
	    } else {
		copyAmount = vectorPtr->bufSize;
	    }
	    bcopy(ptr, vectorPtr->buffer, copyAmount);
	    ptr += copyAmount;
	    bufSize -= copyAmount;
	}
    }
    free((char *) buffer);
    return(status);
}
