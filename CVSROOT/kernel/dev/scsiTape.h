/*
 * scsiTape.h --
 *
 *	External definitions for the tape drives on the SCSI I/O bus.
 *	There are several variants of tape drives and each has an
 *	associated header file that defines drive-specific sense
 *	data and other info.
 *
 * Copyright 1986 Regents of the University of California
 * All rights reserved.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header$ SPRITE (Berkeley)
 */

#ifndef _SCSITAPE
#define _SCSITAPE

#include "scsi.h"
#include "scsiDevice.h"
#include "dev/tape.h"

/*
 * State info for an SCSI tape drive.  
 */
#define	SCSI_TAPE_DEFAULT_BLOCKSIZE	512
/*
 * State info for an SCSI tape drive.  This is used to map from a device
 * unit number back to the SCSI controller for the drive.
 */
typedef struct ScsiTape {
    ScsiDevice	*devPtr;        /* SCSI Device we have attached. */
    int state;			/* State bits used to determine if it's open,
				 * it it really exists, etc. */
    char *name;			/* Type name of tape device. */
    int blockSize;		/* Native block size of the drive. */
    void (*releaseProc)();	/* Procedure to customize device close. */
    ReturnStatus (*tapeIOProc)(); /* Procedure to customize tape IO. */
    ReturnStatus (*specialCmdProc)();	/* Procedure to handle specail 
					 * tape cmds. */
    ReturnStatus (*errorProc)(); /* Procedure to handle sense data */
} ScsiTape;



/*
 * Tape drive state:
 *	SCSI_TAPE_CLOSED	The device file corresponding to the drive
 *		is not open.  This inhibits read/writes, and allows opens.
 *	SCSI_TAPE_OPEN		The device file is open.
 *	SCSI_TAPE_AT_EOF	The file mark was hit on the last read.
 *	SCSI_TAPE_WRITTEN	The last op on the tape was a write.  This is
 *				used to write an end-of-tape mark before
 *				closing or rewinding.
 */
#define SCSI_TAPE_CLOSED	0x0
#define SCSI_TAPE_OPEN		0x1
#define SCSI_TAPE_AT_EOF	0x2
#define SCSI_TAPE_WRITTEN	0x4

/*
 * SCSI_TAPE_DEFAULT_BLOCKSIZE the default block size for SCSI Tapes.
 */
#define	SCSI_TAPE_DEFAULT_BLOCKSIZE	512

extern int devNumSCSITapeTypes;
extern ReturnStatus ((*devSCSITapeAttachProcs[])());
/*
 * Forward Declarations.
 */
ReturnStatus DevSCSITapeError();
ReturnStatus DevSCSITapeSpecialCmd();
ReturnStatus DevSCSITapeVariableIO();
ReturnStatus DevSCSITapeFixedBlockIO();

#endif _SCSITAPE
