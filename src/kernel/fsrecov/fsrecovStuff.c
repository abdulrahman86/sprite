/* 
 * fsrecovStuff.c --
 *
 *	Fast recovery handling for the file system.
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
static char rcsid[] = "$Header: /cdrom/src/kernel/Cvsroot/kernel/fsrecov/fsrecovStuff.c,v 1.2 92/08/11 16:31:21 mgbaker Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include <sprite.h>
#include <fs.h>
#include <fslcl.h>
#include <fsio.h>
#include <fsrecov.h>
#include <fsdm.h>
#include <recov.h>
#include <fsutil.h>
#include <stdio.h>
#include <fspdev.h>
#include <fsioDevice.h>
#include <fsioPipe.h>
#include <strings.h>

#define	FSRECOV_OBJECT_TYPE	0

DirEntryMap	fsrecov_DirLogObjNums[FSRECOV_NUM_DIR_LOG_ENTRIES];
Boolean		finishedInit = FALSE;
Fsrecov_DirLog	fsrecov_DirLog;

#define	FSRECOV_SAFE_SECONDS	60	/* After 35 seconds, a write should
					 * be permanent.  This is safer. */
int	fsrecov_MaxNumObjs = 0;
static	Hash_Table	fsrecovHashTable;

Boolean	fsrecov_AlreadyInit = FALSE;	/* Recov box existed on reboot. */
Boolean	fsrecov_FromBox = TRUE;		/* Do the recovery from what we
					 * have in the recov box, even for
					 * old clients that are doing
					 * reopens. */
int	fsrecov_DebugLevel = 1;		/* 0 == off.  2 == everything. */

/*
 * Stuff for doing testing and timing.
 */
Time    fsrecov_StartRebuild;
Time    fsrecov_ReturnedObjs;
Time    fsrecov_BuiltHashTable;
Time    fsrecov_BuiltIOHandles;
Time    fsrecov_EndRebuild;
int     fsrecov_RebuildNumObjs;

typedef	struct ClientObject {
    int			objectNumber;
    Fsio_UseCounts	use;
} ClientObject;

char	*LogOpString _ARGS_((int op));

Boolean	logProcessDebug = TRUE;

#define	CHECK		/* Check against stored objects. */


/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_InitState --
 *
 *	Initialize recov state in recov box for file system.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Recov box gets initialized if it wasn't already.  Hash table of
 *	mappings from fileID to object number gets created but not set up.
 *
 *----------------------------------------------------------------------
 */
void
Fsrecov_InitState()
{
    ReturnStatus		status;
    int				objectType;
    int				numDirLogsPerHandle;
    int				numMarksObjsPerHandle;
#define	NUM_MARKS_OBJS		100
#define	SIZE_MARKS_OBJS		500

    /*
     * Initialize the recovery module.  Do before Rpc and after Vm_Init.
     *
     * If this is a fastRestart, then the recov box will already have
     * been initialized with our fs object type.
     */
    fsrecov_AlreadyInit = Recov_InitRecovBox();
    fsrecov_MaxNumObjs = Recov_MaxNumObjects(sizeof (Fsrecov_HandleState),
	    fsrecov_AlreadyInit);
    /* Allow some room for other objects to use the box. */
    if (sizeof (Fsrecov_DirLogEntry) > sizeof (Fsrecov_HandleState)) {
	numDirLogsPerHandle = (sizeof (Fsrecov_DirLogEntry) /
		sizeof (Fsrecov_HandleState)) + 1;
    } else {
	numDirLogsPerHandle = 1;
    }
    if (SIZE_MARKS_OBJS > sizeof (Fsrecov_HandleState)) {
	numMarksObjsPerHandle = (SIZE_MARKS_OBJS /
		sizeof (Fsrecov_HandleState)) + 1;
    } else {
	numMarksObjsPerHandle = 1;
    }
    if (fsrecov_MaxNumObjs >
	    (100 + (FSRECOV_NUM_DIR_LOG_ENTRIES * numDirLogsPerHandle) +
		(NUM_MARKS_OBJS * numMarksObjsPerHandle))) {
	fsrecov_MaxNumObjs -= 100 +
		(FSRECOV_NUM_DIR_LOG_ENTRIES * numDirLogsPerHandle) +
		(NUM_MARKS_OBJS * numMarksObjsPerHandle);
    }
    Hash_Init(&fsrecovHashTable, 8, sizeof (Fs_FileID) / sizeof (int));
    if (!fsrecov_AlreadyInit) {
	status = Recov_InitType(sizeof (Fsrecov_HandleState),
	    fsrecov_MaxNumObjs, 0, &objectType, Fsrecov_Checksum);
	if (status != SUCCESS) {
	    panic("Fs_InitRecovState: couldn't initialize object type.");
	}
	if (objectType != FSRECOV_OBJECT_TYPE) {
	    panic("Fs_InitRecovState: somebody else got here first.");
	}
    }
    return;
}
    

/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_SetupHandles --
 *
 *	Use the handle state from the recov box to set up stuff.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Hash table of mappings from fileID to object number gets set up
 *	(and filled in if this is a fast restart).
 *
 *----------------------------------------------------------------------
 */
void
Fsrecov_SetupHandles()
{
    ReturnStatus		status;
    Fsrecov_HandleState		*obuffer;
    int				osizeNeeded;
    Recov_ObjectID		*ibuffer;
    int				isizeNeeded;
    Hash_Entry			*entryPtr;
    int				i;
    Fs_FileID			fileID;
    Recov_ObjectID		objID;
    Fsrecov_HandleState		*recovInfoPtr;

    if (!fsrecov_AlreadyInit) {
	return;
    }
    Timer_GetTimeOfDay(&fsrecov_StartRebuild, (int *) NIL, (Boolean *) NIL);
    /*
     * Set-up handle table from what's in box.
     */
    osizeNeeded = sizeof (Fsrecov_HandleState) * fsrecov_MaxNumObjs;
    obuffer = (Fsrecov_HandleState *) malloc(osizeNeeded);
    if (obuffer == (Fsrecov_HandleState *) NIL) {
	panic("Fsrecov_SetupHandles: not enough object buffer space.");
    }
    isizeNeeded = sizeof (Recov_ObjectID) * fsrecov_MaxNumObjs;
    ibuffer = (Recov_ObjectID *) malloc(isizeNeeded);
    if (ibuffer == (Recov_ObjectID *) NIL) {
	free((char *) obuffer);
	panic("Fsrecov_SetupHandles: not enough object buffer space.");
    }
    status = Recov_ReturnObjects(FSRECOV_OBJECT_TYPE, &osizeNeeded,
	    (char *) obuffer, &isizeNeeded, (char *) ibuffer, (int *) NIL,
	    (char *) NIL);
    if (status != SUCCESS) {
	free((char *) obuffer);
	free((char *) ibuffer);
	panic("Fsrecov_SetupHandles: couldn't get old object list out of box.");
    }
    fsrecov_RebuildNumObjs = osizeNeeded / sizeof (Fsrecov_HandleState);
    Timer_GetTimeOfDay(&fsrecov_ReturnedObjs, (int *) NIL, (Boolean *) NIL);
    /*
     * Put mapping from fileID (with clientID as first field) to object
     * number into hash table.
     */
    for (i = 0; i < fsrecov_RebuildNumObjs; i++) {
	/* This is a fileID, but with the first field being the clientID. */
	fileID = obuffer[i].fileID;
	objID = ibuffer[i];
	/* Put objectID/fileID pairing in hash table. */
	entryPtr = Hash_Find(&fsrecovHashTable, (Address) &fileID);
	if (entryPtr->value != (Address) NIL) {
	    free((char *) obuffer);
	    free((char *) ibuffer);
	    panic("Fsrecov_SetupHandles: file ID already in table for client.");
	}
	entryPtr->value = (Address) malloc(sizeof (ClientObject));
	((ClientObject *) entryPtr->value)->objectNumber =
		ibuffer[i].objectNumber;
	((ClientObject *) entryPtr->value)->use = obuffer[i].use;
    }

    /*
     * If we're not really to use the stuff from the recov box to recover
     * our state, then just return here before we do that.
     */
    if (!fsrecov_FromBox) {
	return;
    }

    /*
     * Now that we have the hash table so that we can find the objects
     * for any file ID, go through and set up the client lists and initialize
     * the io handles.
     */
    Timer_GetTimeOfDay(&fsrecov_BuiltHashTable, (int *) NIL, (Boolean *) NIL);
    for (i = 0; i < fsrecov_RebuildNumObjs; i++) {
	fileID = obuffer[i].fileID;
	recovInfoPtr = &obuffer[i];

	switch(fileID.type) {
	case FSIO_LCL_FILE_STREAM:
	case FSIO_RMT_FILE_STREAM: {
	    (void) Fsio_FileSetupHandle(recovInfoPtr);
	    break;
	}
	case FSIO_LCL_DEVICE_STREAM:
	case FSIO_RMT_DEVICE_STREAM: {
	    (void) Fsio_DeviceSetupHandle(recovInfoPtr);
	    break;
	}
	case FSIO_LCL_PIPE_STREAM:
	case FSIO_RMT_PIPE_STREAM: {
	    (void) Fsio_PipeSetupHandle(recovInfoPtr);
	    break;
	}
	case FSIO_CONTROL_STREAM:
	case FSIO_PFS_CONTROL_STREAM: {
	    (void) Fspdev_ControlSetupHandle(recovInfoPtr);
	    break;
	}
	default:
	    break;
	}
    }
    Timer_GetTimeOfDay(&fsrecov_BuiltIOHandles, (int *) NIL, (Boolean *) NIL);
    /*
     * Finally, reopen all the streams.
     */
    for (i = 0; i < fsrecov_RebuildNumObjs; i++) {
	fileID = obuffer[i].fileID;
	if (fileID.type == FSIO_STREAM) {
	    Fs_HandleHeader	*ioHandlePtr;
	    int			clientID;
	    Fs_HandleHeader	handle;
	    Fs_FileID		otherID;
	    Fs_Stream		*streamPtr;

	    objID = ibuffer[i];
	    recovInfoPtr = &obuffer[i];
	    otherID = recovInfoPtr->otherID;

	    /* Fix ID's to include correct serverID. */
	    clientID = fileID.serverID;
	    otherID.serverID = rpc_SpriteID;
	    fileID.serverID = rpc_SpriteID;
	    handle.fileID = fileID;
	    /*
	     * The client verify that's done in the real stream reopen
	     * does a verify on the remote handle type, since it hasn't
	     * converted the type yet.  Then it turns around and converts
	     * the type to be local.  Here, it just ends up re-localizing
	     * a type that's already local.
	     */
	    ioHandlePtr = (*fsio_StreamOpTable[
		    Fsio_MapLclToRmtType(otherID.type)].clientVerify)
		    (&otherID, clientID, (int *) NIL);
	    if (ioHandlePtr == (Fs_HandleHeader *) NIL) {
		panic("Fsrecov_SetupHandles: no handle found for stream.\n");
		/*
		 * The info field of the object data for a stream includes
		 * its flags to pass to the delete routine.
		 */
		if (Fsrecov_DeleteHandle(&handle, clientID, recovInfoPtr->info)
			!= SUCCESS) {
		    printf(
			"Fsrecov_SetupHandles: couldn't delete bad stream.\n");
		}
		continue;
	    }
	    streamPtr = Fsutil_HandleFetchType(Fs_Stream, &fileID);
	    /* This should usually return NIL. */
	    if (streamPtr != (Fs_Stream *) NIL) {
		/* Verify we have the stream hooked to the correct ioHandle. */
		if (streamPtr->ioHandlePtr != ioHandlePtr) {
		    printf(
		    "Fsrecov_SetupHandles: bad handle found for stream.\n");
		    if (Fsrecov_DeleteHandle(&handle, clientID,
			    recovInfoPtr->info) != SUCCESS) {
			printf(
			"Fsrecov_SetupHandles: couldn't delete bad stream.\n");
		    }
		    Fsutil_HandleRelease(streamPtr, TRUE);
		    Fsutil_HandleRelease(ioHandlePtr, TRUE);
		    continue;
		} else {
		    Fsutil_HandleRelease(streamPtr, TRUE);
		}
		/* Check sharing stuff done in Fsio_StreamReopen? */
	    }
	    /* streamPtr->flags set from recovInfoPtr->info parameter. */
	    streamPtr = Fsio_StreamAddClient(&fileID, clientID, ioHandlePtr,
		    recovInfoPtr->info, ioHandlePtr->name, (Boolean *) NIL,
		    (Boolean *) NIL);
	    streamPtr->offset = recovInfoPtr->clientData;
	    Fsutil_HandleRelease(streamPtr, TRUE);
	    Fsutil_HandleRelease(ioHandlePtr, TRUE);
	}
    }
    Timer_GetTimeOfDay(&fsrecov_EndRebuild, (int *) NIL, (Boolean *) NIL);

    free((char *) obuffer);
    free((char *) ibuffer);

    return;
}

/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_AddHandle --
 *
 *	Add a handle to the recov state in recov box for file system.
 *
 * Results:
 *	SUCCESS or FAILURE.
 *
 * Side effects:
 *	
 *	Add a handle to the recov box and its fileID/objectID mapping
 *	to the hash table.
 *
 *----------------------------------------------------------------------
 */
ReturnStatus
Fsrecov_AddHandle(hdrPtr, otherIDPtr, clientID, useFlags, clientData, addRef)
    Fs_HandleHeader		*hdrPtr;
    Fs_FileID			*otherIDPtr;
    int				clientID;
    unsigned int		useFlags;
    int				clientData;
    Boolean			addRef;
{
    int				status = SUCCESS;
    Recov_ObjectID		objectID;
    Fsrecov_HandleState		objectData;
    Hash_Entry			*entryPtr;

    /* Add objectID/fileID mapping to hash table. */
    objectData.fileID = hdrPtr->fileID;
    objectData.fileID.serverID = clientID;
    if (fsrecov_DebugLevel >= 3) {
	printf("Fs_Add: looking for %d.%d.%d.%d\n", objectData.fileID.type,
	objectData.fileID.serverID, objectData.fileID.major,
	objectData.fileID.minor);
    }
    entryPtr = Hash_Find(&fsrecovHashTable, (Address) &objectData.fileID);
    if (entryPtr->value != (Address) NIL) {
	if (fsrecov_DebugLevel >= 3) {
	    printf("Found.\n");
	}
	/* Already a handle for this client/file pair. */
	objectID.typeID = FSRECOV_OBJECT_TYPE;
	objectID.objectNumber =
		((ClientObject *) entryPtr->value)->objectNumber;
	objectData.use =
		((ClientObject *) entryPtr->value)->use;
#ifdef CHECK
	status = Recov_ReturnObject((ClientData) &objectData, objectID, FALSE);
	if (status != SUCCESS) {
	    panic("Fsrecov_AddHandle: couldn't find existing object in box.");
	}
#endif CHECK
	/* Increment ref counts for this client's use of file. */
	if (addRef) {
	    objectData.use.ref++;
	    if (hdrPtr->fileID.type != FSIO_CONTROL_STREAM &&
		    hdrPtr->fileID.type != FSIO_PFS_CONTROL_STREAM) {
		if (useFlags & FS_WRITE) {
		    objectData.use.write++;
		}
		if (useFlags & FS_EXECUTE) {
		    objectData.use.exec++;
		}
	    }
	    ((ClientObject *) entryPtr->value)->use = objectData.use;
	}
	objectData.otherID.type = -1;
	objectData.otherID.serverID = -1;
	objectData.otherID.major = -1;
	objectData.otherID.minor = -1;
	/* Check other state. */
	if (hdrPtr->fileID.type == FSIO_LCL_FILE_STREAM) {
	    Fsio_FileIOHandle		*handlePtr;
	    
	    handlePtr = (Fsio_FileIOHandle *) hdrPtr;
#ifdef CHECK
	    /* Info is file version. */
	    if (objectData.info != handlePtr->descPtr->version) {
		if (fsrecov_DebugLevel >= 2) {
		    printf(
		    "Fsrecov_AddHandle: changing version number of object.\n");
		}
		objectData.info = handlePtr->descPtr->version;
	    }
#else CHECK
	    objectData.info = handlePtr->descPtr->version;
#endif CHECK

#ifdef CHECK
	    /* ClientData is whether file is cacheable or not. */
	    if (objectData.clientData != clientData) {
		printf(
		    "Fsrecov_AddHandle: changing cachable state of object.\n");
		objectData.clientData = clientData;
	    }
#else CHECK
	    objectData.clientData = clientData;
#endif CHECK
	} else if (hdrPtr->fileID.type == FSIO_CONTROL_STREAM ||
		hdrPtr->fileID.type == FSIO_PFS_CONTROL_STREAM) {
	    /* Info is the serverID == hostID of machine running the server. */
	    objectData.info = clientID;
	    /* ClientData is the "seed." */
	    objectData.clientData = clientData;
	    /* The useFlags may indicate NIL serverID. */
	    if (useFlags == NIL) {
		objectData.info = NIL;
	    }
	} else if (hdrPtr->fileID.type == FSIO_STREAM) {
	    if (otherIDPtr != (Fs_FileID *) NIL) {
		objectData.otherID = *otherIDPtr;
	    }
	    /* ClientData is stream offset. */
	    objectData.clientData = clientData;
	    /* Info is useFlags since there's other fields that count there. */
	    objectData.info = useFlags;
	}

	/* Update object in box. */
	status = Recov_UpdateObject((ClientData ) &objectData, objectID);
	if (status != SUCCESS) {
	    panic("Fsrecov_AddHandle: couldn't update object in box.");
	}
    } else {
	if (fsrecov_DebugLevel >= 3) {
	    printf("NOT found.\n");
	}
	/* Put the file into the box for the first time for this client. */
	if (addRef) {
	    objectData.use.ref = 1;
	} else {
	    objectData.use.ref = 0;
	}
	objectData.use.write = 0;
	objectData.use.exec = 0;
	if (addRef) {
	    if (hdrPtr->fileID.type != FSIO_CONTROL_STREAM &&
		    hdrPtr->fileID.type != FSIO_PFS_CONTROL_STREAM) {
		if (useFlags & FS_WRITE) {
		    objectData.use.write = 1;
		}
		if (useFlags & FS_EXECUTE) {
		    objectData.use.exec = 1;
		}
	    }
	}
	objectData.otherID.type = -1;
	objectData.otherID.serverID = -1;
	objectData.otherID.major = -1;
	objectData.otherID.minor = -1;
	if (hdrPtr->fileID.type == FSIO_LCL_FILE_STREAM) {
	    Fsio_FileIOHandle		*handlePtr;
	    
	    handlePtr = (Fsio_FileIOHandle *) hdrPtr;
	    /* Info is file version. */
	    objectData.info = handlePtr->descPtr->version;
	    /* ClientData is whether file is cacheable or not. */
	    objectData.clientData = clientData;
	} else if (hdrPtr->fileID.type == FSIO_CONTROL_STREAM ||
		hdrPtr->fileID.type == FSIO_PFS_CONTROL_STREAM) {
	    /* Info is the serverID == hostID of machine running the server. */
	    objectData.info = clientID;
	    /* ClientData is the "seed." */
	    objectData.clientData = clientData;
	    /* The useFlags may indicate NIL serverID. */
	    if (useFlags == NIL) {
		objectData.info = NIL;
	    }
	} else if (hdrPtr->fileID.type == FSIO_STREAM) {
	    if (otherIDPtr != (Fs_FileID *) NIL) {
		objectData.otherID = *otherIDPtr;
	    }
	    /* ClientData is offset of stream. */
	    objectData.clientData = clientData;
	    /* Info is useFlags since there's other fields that count there. */
	    objectData.info = useFlags;
	} else {
	    objectData.clientData = 0;
	    objectData.info = 0;
	}
	status = Recov_InsertObject(FSRECOV_OBJECT_TYPE,
		(ClientData) &objectData, -1, &objectID);
	if (status != SUCCESS) {
	    printf("Fsrecov_AddHandle: object insert failed.\n");
	}
	entryPtr->value = (Address) malloc(sizeof (ClientObject));
	((ClientObject *) entryPtr->value)->objectNumber =
		objectID.objectNumber;
	((ClientObject *) entryPtr->value)->use = objectData.use;
    }

    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_DeleteHandle --
 *
 *	Delete a handle in the recov state in recov box for file system.
 *
 * Results:
 *	SUCCESS or FAILURE.
 *
 * Side effects:
 *	
 *	Delete a file handle in the recov box and its fileID/objectID mapping
 *	to the hash table.
 *
 *----------------------------------------------------------------------
 */
ReturnStatus
Fsrecov_DeleteHandle(hdrPtr, clientID, flags)
    Fs_HandleHeader		*hdrPtr;
    int				clientID;
    unsigned int		flags;
{
    int				status;
    Recov_ObjectID		objectID;
    Fsrecov_HandleState		objectData;
    Hash_Entry			*entryPtr;

    /*
     * If we decide ever to make stuff recacheable after an offender closes
     * a file, I'd have to pass in that info and deal with it here.
     */

    /* Get objectID/fileID mapping to hash table. */
    objectData.fileID = hdrPtr->fileID;
    objectData.fileID.serverID = clientID;
    if (fsrecov_DebugLevel >= 3) {
	printf("Fs_Delete: looking for %d.%d.%d.%d\n", objectData.fileID.type,
	objectData.fileID.serverID, objectData.fileID.major,
	objectData.fileID.minor);
    }
    entryPtr = Hash_Find(&fsrecovHashTable, (Address) &objectData.fileID);
    if (entryPtr->value == (Address) NIL) {
	/* No handle for this client/file pair! */
	panic("Fsrecov_DeleteHandle: couldn't find object in hash table.");
    }
    if (fsrecov_DebugLevel >= 3) {
	printf("Found.\n");
    }
    objectID.typeID = FSRECOV_OBJECT_TYPE;
    objectID.objectNumber = ((ClientObject *) entryPtr->value)->objectNumber;

    objectData.otherID.type = -1;
    objectData.otherID.serverID = -1;
    objectData.otherID.major = -1;
    objectData.otherID.minor = -1;
#ifdef CHECK
    status = Recov_ReturnObject((ClientData) &objectData, objectID, FALSE);
    if (status != SUCCESS) {
	panic("Fsrecov_DeleteHandle: couldn't return object.");
    }
#else CHECK
    objectData.use = ((ClientObject *) entryPtr->value)->use;
#endif CHECK

    objectData.use.ref--;
    if (flags & FS_EXECUTE) {
	objectData.use.exec--;
    }
    if (flags & FS_WRITE) {
	objectData.use.write--;
    }
    if (fsrecov_DebugLevel >= 3) {
	printf("Ref is %d\n", objectData.use.ref);
    }

    if (fsrecov_DebugLevel >= 3) {
	if (objectData.use.ref < 0) {
	    printf("Ref count is %d\n", objectData.use.ref);
	}
    }

    /*
     * If the reference count has gone to 0 and there are no dirty
     * blocks in the client's cache to be written back, then we can
     * delete the last reference to this handle in the recovery box.
     * This is true also if the file is not cacheable, since there'll be
     * no dirty blocks in that case.
     * Otherwise, we can only decrement the reference counts.
     * This means it's possible for a handle to be in the recovery box
     * with a 0 reference count if it still has dirty blocks to write back.
     */
    if (objectData.use.ref <= 0 &&
	    ((hdrPtr->fileID.type != FSIO_LCL_FILE_STREAM &&
	    hdrPtr->fileID.type != FSIO_RMT_FILE_STREAM) ||
	    (flags & FS_LAST_DIRTY_BLOCK) || (objectData.clientData == 0))) {
	/* Delete the object. */
	if (fsrecov_DebugLevel >= 3) {
	    printf("Deleting object.\n");
	    if (flags & FS_LAST_DIRTY_BLOCK) {
		printf("Was last dirty block.\n");
	    }
	}
	status = Recov_DeleteObject(objectID);
	if (status != SUCCESS) {
	    panic("Fsrecov_DeleteHandle: Couldn't delete existing object.");
	}
	/* Also delete from hash table. */
	free(entryPtr->value);
	Hash_Delete(&fsrecovHashTable, entryPtr);
    } else {
#ifndef CHECK
	if (hdrPtr->fileID.type == FSIO_STREAM) {
	    Fs_Stream	*streamPtr;

	    streamPtr = (Fs_Stream *) hdrPtr;
	    if (streamPtr->ioHandlePtr == (Fs_HandleHeader *) NIL) {
		panic("Fsrecov_DeleteHandle: NIL ioHandlePtr.");
	    }
	    objectData.otherID = streamPtr->ioHandlePtr->hdr;

	    /* ClientData is stream offset. */
	    objectData.clientData = streamPtr->offset;
	    /* Info is useFlags. */
	    objectData.info = streamPtr->flags;
	}
	if (hdrPtr->fileID.type == FSIO_CONTROL_STREAM ||
		hdrPtr->fileID.type == FSIO_PFS_CONTROL_STREAM) {
	    /* Make this easy for now, if slower for pseudo devices. */
	    status = Recov_ReturnObject((ClientData) &objectData, objectID,
		    FALSE);
	    if (status != SUCCESS) {
		panic("Fsrecov_DeleteHandle: couldn't return pseudo device.");
	    }
	    /* Info is the serverID == hostID of machine running the server. */
	    /* ClientData is the "seed." */
	    /* useFlags may indicate NIL serverID. */
	}
	if (hdrPtr->fileID.type == FSIO_LCL_FILE_STREAM) {
	    Fsio_FileIOHandle	*handlePtr;

	    handlePtr = (Fsio_FileIOHandle *) hdrPtr;
	    /* ClientData is whether file is cacheable or not. */
	    fileStatePtr->cacheable
	    /* Info is file version. */
	    objectData.info = handlePtr->descPtr->version;
	}
#endif CHECK
	/* Update the object with new ref counts. */
	status = Recov_UpdateObject((ClientData ) &objectData, objectID);
	if (status != SUCCESS) {
	    panic("Fsrecov_DeleteHandle: couldn't update object in box.");
	}
	((ClientObject *) entryPtr->value)->use = objectData.use;
    }
	    
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_GetHandle --
 *
 *	Return a handle from the recov state in recov box for file system.
 *
 * Results:
 *	SUCCESS or FAILURE.
 *
 * Side effects:
 *	
 *	Return an object filled in with the recovery box entry.
 *
 *----------------------------------------------------------------------
 */
ReturnStatus
Fsrecov_GetHandle(fileID, clientID, objectPtr, checksum)
    Fs_FileID		fileID;
    int			clientID;
    Fsrecov_HandleState	*objectPtr;
    Boolean		checksum;
{
    Hash_Entry			*entryPtr;
    Recov_ObjectID		objectID;
    ReturnStatus		status = SUCCESS;
    
    /* Get objectID/fileID mapping to hash table. */
    fileID.serverID = clientID;
    if (fsrecov_DebugLevel >= 3) {
	printf("Fs_Get: looking for %d.%d.%d.%d\n",
		fileID.type, fileID.serverID,
		fileID.major, fileID.minor);
    }
    entryPtr = Hash_Find(&fsrecovHashTable, (Address) &fileID);
    if (entryPtr->value == (Address) NIL) {
	/* No handle for this client/file pair! */
	return FAILURE;
    }
    objectID.typeID = FSRECOV_OBJECT_TYPE;
    objectID.objectNumber = ((ClientObject *) entryPtr->value)->objectNumber;
    status = Recov_ReturnObject((ClientData) objectPtr, objectID, checksum);

    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_UpdateHandle --
 *
 *	Update a handle in the recov state in recov box for file system.
 *
 * Results:
 *	SUCCESS or FAILURE.
 *
 * Side effects:
 *	
 *	Updates the information for a handle in the recov box.
 *
 *----------------------------------------------------------------------
 */
ReturnStatus
Fsrecov_UpdateHandle(fileID, clientID, objectPtr)
    Fs_FileID		fileID;
    int			clientID;
    Fsrecov_HandleState	*objectPtr;
{
    Hash_Entry			*entryPtr;
    Recov_ObjectID		objectID;
    ReturnStatus		status;
    
    /* Get objectID/fileID mapping to hash table. */
    fileID.serverID = clientID;
    if (fsrecov_DebugLevel >= 3) {
	printf("Fs_Get: looking for %d.%d.%d.%d\n", fileID.type,
		fileID.serverID, fileID.major, fileID.minor);
    }
    entryPtr = Hash_Find(&fsrecovHashTable, (Address) &fileID);
    if (entryPtr->value == (Address) NIL) {
	/* No handle for this client/file pair! */
	panic("Fsrecov_UpdateHandle: couldn't find object in hash table.");
    }
    objectID.typeID = FSRECOV_OBJECT_TYPE;
    objectID.objectNumber = ((ClientObject *) entryPtr->value)->objectNumber;
    status = Recov_UpdateObject((ClientData) objectPtr, objectID);
    /* We'll need to do better than this! */
    if (status != SUCCESS) {
	panic("Fsrecov_UpdateHandle: couldn't get object.");
    }
    return SUCCESS;
}


/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_ThisType --
 *
 *	Is this the type of handle to put in the recov box?
 *
 * Results:
 *	TRUE or FALSE (yes or no).
 *
 * Side effects:
 *	
 *	None.
 *
 *----------------------------------------------------------------------
 */
Boolean
Fsrecov_ThisType(hdrPtr, clientID)
    Fs_HandleHeader	*hdrPtr;
    int			clientID;
{
    Boolean	saveThis = FALSE;

    switch (hdrPtr->fileID.type) {
    case FSIO_LCL_FILE_STREAM:
    case FSIO_RMT_FILE_STREAM: {
	if (clientID != rpc_SpriteID) {
	    saveThis = TRUE;
	}
	break;
    }
    case FSIO_LCL_DEVICE_STREAM:
    case FSIO_RMT_DEVICE_STREAM: {
	if (clientID != rpc_SpriteID) {
	    saveThis = TRUE;
	}
	break;
    }
    case FSIO_CONTROL_STREAM: {
	/*
	 * Save the control handle if the pdev server is not us: the serverID
	 * will either be NIL or something that's not our rpc_SpriteID.  Why
	 * am I checking the clientID also?  I don't know...  It should
	 * usually be the same as the server ID unless that's NIL.
	 */
	if (clientID != rpc_SpriteID &&
		((Fspdev_ControlIOHandle *) hdrPtr)->serverID != rpc_SpriteID) {
	    saveThis = TRUE;
	}
	break;
    }
    default:
	break;
    }

    return saveThis;
}


/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_Checksum --
 *
 *	Compute the 16-bit one's complement of the 1's complement sum of
 *	of all words in the buffer, which is an Fsrecov_HandleState item.
 *
 *	Note: It is assumed that the length of the buffer is at most
 *	128K bytes long. It also helps if the buffer is word-aligned.
 *
 * Results:
 *	The 1's complement checksum in network byte-order.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
unsigned short
Fsrecov_Checksum(len, bufPtr)
    register int len;		/* The number of bytes to checksum. */
    Address bufPtr;		/* What to checksum. */
{
    register unsigned short *wordPtr = (unsigned short *) bufPtr;
    register unsigned int sum = 0;

    
    /*
     * The basic algorithm 16-bit 1's complement addition is 
     *  1) add the two unsigned 16-bit quantities, 
     *  2) if there was a carry out of the high-order bit, 
     *	   it is added to the sum.
     * To detect a carry out of the high-order bit, the sum is stored
     * in a 32-bit word. As an optimization, we delay step 2 until
     * all the words have been added together. At that point, the
     * upper-half of the sum contains the sum of the carries from the
     * additions. This value is then added to the lower half and if that
     * operation causes a carry, then 1 is added to the sum.
     *
     * The optimization does place a limit on how many bytes can be
     * summed without causing an overflow of the 32-bit sum. In the worst
     * case, a maximum of 64K additions of 16-bit values can be added
     * without overflow.
     * 
     * The summation is done in an unrolled loop. Once we have less than 
     * 32 bytes to sum then it must be done in smaller loops.
     */

    if (len != sizeof (Fsrecov_HandleState)) {
	panic("Fsrecov_Checksum: object isn't an Fsrecov_HandleState.");
    }
    if (sizeof (Fsrecov_HandleState) != 52) {
	panic("Fsrecov_Checksum: handle size has changed from 52 to %d.",
		sizeof (Fsrecov_HandleState));
    }
    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;

    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;

    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;

    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;

    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;
    sum += *wordPtr++;

    sum += *wordPtr++;

    return((~sum & 0xffff));
}


/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_DirOpInit --
 *
 *	Initialize the logging of directory operations.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Stuff gets initialized.
 *
 *----------------------------------------------------------------------
 */
void
Fsrecov_DirOpInit()
{
    int			objectType;
    int			i;
    ReturnStatus	status;
    Fsrecov_DirLogEntry	*entries;
    int			entryArraySize;
    Recov_ObjectID	*objectIDs;
    int			objectIDArraySize;
    int			numEntries = 0;
    int			oldestTime = 0;
    int			newestTime = 0;
    int			newestEntry = 0;
    int			logSeqNum;
    int			compareTime;

    fsrecov_DirLog.nextLogSeqNum = 0;
    fsrecov_DirLog.oldestEntry = -1;

    for (i = 0; i < FSRECOV_NUM_DIR_LOG_ENTRIES; i++) {
	fsrecov_DirLogObjNums[i].objectNum = -1;
    }

    if (!fsrecov_AlreadyInit) {
	/* Set up the recovery box type number. */
	status = Recov_InitType(sizeof (Fsrecov_DirLogEntry),
	    FSRECOV_NUM_DIR_LOG_ENTRIES, 0, &objectType,
		(unsigned short (*)()) 1);
	if (status != SUCCESS) {
	    panic("Fsrecov_DirOpInit: initialization of recovery type failed.");
	}
	if (objectType != FSRECOV_DIR_LOG_TYPE) {
	    panic("Fsrecov_DirOpInit: got wrong dir log recovery type.");
	}

	return;
    }

    numEntries = Recov_NumObjects(FSRECOV_DIR_LOG_TYPE);
    if (numEntries <= 0) {
	return;
    }
    entryArraySize = numEntries * sizeof (Fsrecov_DirLogEntry);
    objectIDArraySize = numEntries * sizeof (Recov_ObjectID);
    entries = (Fsrecov_DirLogEntry *) malloc(entryArraySize);
    objectIDs = (Recov_ObjectID *) malloc(objectIDArraySize);

    status = Recov_ReturnObjects(FSRECOV_DIR_LOG_TYPE, &entryArraySize,
	    (char *) entries, &objectIDArraySize, (char *) objectIDs,
	    (int *) NIL, (char *) NIL);
    if (status != SUCCESS) {
	panic("Fsrecov_DirOpInit: couldn't get returned log entries.");
    }
    if ((entryArraySize / sizeof (Fsrecov_DirLogEntry)) != numEntries) {
	panic("Fsrecov_DirOpInit: something is wrong with # of log entries.");
    }

    for (i = 0; i < numEntries; i++) {
	logSeqNum = entries[i].logSeqNum;
	compareTime = entries[i].startTime;
	fsrecov_DirLogObjNums[logSeqNum].objectNum = objectIDs[i].objectNumber;
	/* Mark first with old startTime just for sorting purposes. */
	fsrecov_DirLogObjNums[logSeqNum].timeStamp = compareTime;
	if (fsrecov_DirLog.oldestEntry == -1) {
	    fsrecov_DirLog.oldestEntry = logSeqNum;
	    oldestTime = compareTime;
	}
	if ((compareTime < oldestTime) ||
		(compareTime == oldestTime &&
		logSeqNum < fsrecov_DirLog.oldestEntry)) {
	    fsrecov_DirLog.oldestEntry = logSeqNum;
	    oldestTime = compareTime;
	}
	if ((compareTime > newestTime) ||
		(compareTime == newestTime && logSeqNum > newestEntry)) {
	    newestEntry = logSeqNum;
	    newestTime = compareTime;
	}
    }
    /*
     * Now check if the oldest time and newest time were unique and whether
     * they wrapped around the end of the log.  'Cause if they did, we've
     * picked the wrong ones.
     */
    if (fsrecov_DirLog.oldestEntry == 0) {
	for (i = FSRECOV_NUM_DIR_LOG_ENTRIES - 1; i >= 0; i--) {
	    if (fsrecov_DirLogObjNums[i].objectNum != -1 && 
		    fsrecov_DirLogObjNums[i].timeStamp == oldestTime) {
		fsrecov_DirLog.oldestEntry = i;
	    } else {
		break;
	    }
	}
    }
    if (newestEntry == FSRECOV_NUM_DIR_LOG_ENTRIES - 1) {
	for (i = 0; i < FSRECOV_NUM_DIR_LOG_ENTRIES; i++) {
	    if (fsrecov_DirLogObjNums[i].objectNum != -1 && 
		    fsrecov_DirLogObjNums[i].timeStamp == newestTime) {
		newestEntry = i;
	    } else {
		break;
	    }
	}
    }

    fsrecov_DirLog.nextLogSeqNum = newestEntry + 1;
    if (fsrecov_DirLog.nextLogSeqNum >= FSRECOV_NUM_DIR_LOG_ENTRIES) {
	fsrecov_DirLog.nextLogSeqNum = 0;
	if (fsrecov_DirLogObjNums[0].objectNum != -1) {
	    panic("Fsrecov_DirOpInit: log already filled on startup.");
	}
    }

    free((char *) entries);
    free((char *) objectIDs);

    /*
     * Now set time to current time, so we have a chance for these to
     * be written out before being removed from the log.
     */
    for (i = 0; i < FSRECOV_NUM_DIR_LOG_ENTRIES; i++) {
	if (fsrecov_DirLogObjNums[i].objectNum != -1) {
	    fsrecov_DirLogObjNums[i].timeStamp = Fsutil_TimeInSeconds();
	}
    }
    return;
}


/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_DirOpStart --
 *
 *	Note the start of a directory operation.
 *
 * Results:
 *	Client data identifying the log entry.
 *
 * Side effects:
 *	Log entry made in recovery box.
 *
 *----------------------------------------------------------------------
 */
ClientData
Fsrecov_DirOpStart(opFlags, dirHandlePtr, dirOffset, name, nameLen, fileNumber,
	type, fileDescPtr)
    int         opFlags;        /* Operation code and flags. See fsdm.h for
                                 * definitions. */
    Fsio_FileIOHandle *dirHandlePtr;    /* Handle of directory being operated
                                         * on. */
    int         dirOffset;      /* Byte offset into directory of the directory
                                 * entry containing operation. -1 if offset
                                 * is not known. */
    char        *name;          /* Name of object being operated on. */
    int         nameLen;        /* Length in characters of name. */
    int         fileNumber;     /* File number of object being operated on.*/
    int         type;           /* Type of the object being operated on. */
    Fsdm_FileDescriptor *fileDescPtr; /* FileDescriptor object being operated on
                                       * before operation starts. */
{
    Recov_ObjectID		objectID;
    register Fsdm_Domain        *domainPtr;
    Fsrecov_DirLogEntry		dirLogEntry;
    int				dirFileNumber = dirHandlePtr->hdr.fileID.minor;
    ReturnStatus		status;

    /*
     * Don't start adding entries until we've processed the ones already
     * in the box.  This will happen before we start accepting RPC's, so
     * we won't lose any directory op's that clients depend on.
     */
    if (!finishedInit) {
	return (ClientData) NIL;
    }

    domainPtr = Fsdm_DomainFetch(dirHandlePtr->hdr.fileID.major, FALSE);
    if (domainPtr == (Fsdm_Domain *)NIL) {
        return (ClientData) NIL;
    }

    opFlags |= FSRECOV_LOG_START_ENTRY;


    dirLogEntry.logSeqNum = fsrecov_DirLog.nextLogSeqNum; 
    fsrecov_DirLog.nextLogSeqNum++;
    /* Circular buffer. */
    if (fsrecov_DirLog.nextLogSeqNum >= FSRECOV_NUM_DIR_LOG_ENTRIES) {
	fsrecov_DirLog.nextLogSeqNum = 0;
    }
    /*
     * Log has wrapped on itself, flush out descriptor operations --
     * need call-backs that won't hurt work in progres.  We don't need to
     * wait for this here, necessarily, but next logging must wait till
     * this has finished.
     */
    if (fsrecov_DirLogObjNums[fsrecov_DirLog.nextLogSeqNum].objectNum != -1) {
	panic("Fsrecov_DirOpStart: Log has wrapped on itself.");
    }

    /* Fill in the rest of the entry. */
    dirLogEntry.opFlags = opFlags;
    dirLogEntry.dirFileNumber = dirFileNumber;
    dirLogEntry.dirOffset = dirOffset;
    dirLogEntry.linkCount = (fileDescPtr == (Fsdm_FileDescriptor *) NIL) ? 0 :
	    fileDescPtr->numLinks;
    dirLogEntry.dirEntry.fileNumber = fileNumber;
    dirLogEntry.dirEntry.recordLength = Fslcl_DirRecLength(nameLen);
    dirLogEntry.dirEntry.nameLength = nameLen;
    dirLogEntry.startTime = Fsutil_TimeInSeconds();
    bcopy(name, dirLogEntry.dirEntry.fileName, nameLen);

    status = Recov_InsertObject(FSRECOV_DIR_LOG_TYPE,
	    (ClientData) &dirLogEntry, -1, &objectID);
    if (status != SUCCESS) {
	panic("Fsrecov_DirOpStart: Couldn't insert new entry in log.");
    }

    fsrecov_DirLogObjNums[dirLogEntry.logSeqNum].objectNum =
	    objectID.objectNumber;
    if (fsrecov_DirLog.oldestEntry < 0) {
	fsrecov_DirLog.oldestEntry = dirLogEntry.logSeqNum;
    }

    Fsdm_DomainRelease(dirHandlePtr->hdr.fileID.major);

    return (ClientData) dirLogEntry.logSeqNum;
}


/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_DirOpEnd --
 *
 *	Note the end of a directory operation and its status.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Log entry made in recovery box.
 *
 *----------------------------------------------------------------------
 */
void
Fsrecov_DirOpEnd(opFlags, dirHandlePtr, dirOffset, name, nameLen, fileNumber,
                type, fileDescPtr, clientData, opStatus)
    int         opFlags;        /* Operation code and flags. See fsdm.h for
                                 * definitions. */
    Fsio_FileIOHandle *dirHandlePtr;    /* Handle of directory being operated
                                         * on. */
    int         dirOffset;      /* Byte offset into directory of the directory
                                 * entry containing operation. -1 if offset
                                 * is not known. */
    char        *name;          /* Name of object being operated on. */
    int         nameLen;        /* Length in characters of name. */
    int         fileNumber;     /* File number of objecting being operated on.*/
    int         type;           /* Type of the object being operated on. */
    Fsdm_FileDescriptor *fileDescPtr; /* FileDescriptor object being operated on
                                       * before operation starts. */
    ClientData  clientData;     /* ClientData as returned by DirOpStart. */
    ReturnStatus opStatus;	/* Return status of the operation, SUCCESS if
                                 * operation succeeded. FAILURE otherwise. */
{
    register	Fsdm_Domain	*domainPtr;
    Recov_ObjectID		objectID;
    ReturnStatus		status;
    Fsrecov_DirLogEntry		dirLogEntry;

    /*
     * Don't start adding entries until we've processed the ones already
     * in the box.  This will happen before we start accepting RPC's, so
     * we won't lose any directory op's that clients depend on.
     */
    if (!finishedInit) {
	return;
    }

    domainPtr = Fsdm_DomainFetch(dirHandlePtr->hdr.fileID.major, FALSE);
    if (domainPtr == (Fsdm_Domain *)NIL) {
        return;
    }
    opFlags |= FSRECOV_LOG_END_ENTRY;

    objectID.typeID = FSRECOV_DIR_LOG_TYPE;
    objectID.objectNumber = fsrecov_DirLogObjNums[(int) clientData].objectNum;
    if (objectID.objectNumber < 0) {
	panic("Fsrecov_DirOpEnd: entry not found.");
    }
    if (opStatus != SUCCESS) {
	/* Remove the log entry, since the operation failed. */
	status = Recov_DeleteObject(objectID);
	if (status != SUCCESS) {
	    panic("Fsrecov_DirOpEnd: Couldn't delete entry.");
	}
	return;
    }
    status = Recov_ReturnObject((ClientData) &dirLogEntry, objectID, FALSE);
    if (status != SUCCESS) {
	panic("Fsrecov_DirOpEnd: Couldn't get entry.");
    }
    dirLogEntry.opFlags = opFlags;
    status = Recov_UpdateObject((ClientData) &dirLogEntry, objectID);
    if (status != SUCCESS) {
	panic("Fsrecov_DirOpEnd: Couldn't update entry.");
    }

    /*
     * FSRECOV_SAFE_SECONDS after this time stamp we can remove the entry
     * from log.
     */
    fsrecov_DirLogObjNums[(int) clientData].timeStamp = Fsutil_TimeInSeconds();
    Fsdm_DomainRelease(dirHandlePtr->hdr.fileID.major);

    return;
}


/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_UpdateLog --
 *
 *	Remove entries in the directory operation log that are old enough
 *	to have been written safely.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Log entries removed from recovery box.
 *
 *----------------------------------------------------------------------
 */
void
Fsrecov_UpdateLog(timeStamp)
    int		timeStamp;	/* If not NIL, use this as comparison time. */
{
    int		currentTime;
    int		seqNum;
    int		numEntries;

    if (!finishedInit) {
	return;
    }

    if (timeStamp == (int) NIL) {
	currentTime = Fsutil_TimeInSeconds();
    } else{
	currentTime = timeStamp;
    }

    seqNum = fsrecov_DirLog.oldestEntry;
    if (seqNum < 0) {
	return;
    }
    while (seqNum != fsrecov_DirLog.nextLogSeqNum &&
	    Recov_NumObjects(FSRECOV_DIR_LOG_TYPE) > 0) {
        if (fsrecov_DirLogObjNums[seqNum].timeStamp >
		currentTime - FSRECOV_SAFE_SECONDS) {
	    break;
	}
	Fsrecov_DirOpRemove((ClientData) seqNum);
	seqNum++;
	if (seqNum >= FSRECOV_NUM_DIR_LOG_ENTRIES) {
	    seqNum = 0;
	}
    }
    numEntries = Recov_NumObjects(FSRECOV_DIR_LOG_TYPE);
    if (numEntries <= 0) {
	fsrecov_DirLog.oldestEntry = -1;
    } else {
	fsrecov_DirLog.oldestEntry = seqNum;
    }
    if (numEntries > 0 && fsrecov_DirLog.oldestEntry ==
	    fsrecov_DirLog.nextLogSeqNum) {
	panic("Fsrecov_UpdateLog: something is wrong with the log.\n");
    }

    return;
}


/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_DirOpRemove --
 *
 *	Remove an entry in the directory operation log.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Log entry removed from recovery box.
 *
 *----------------------------------------------------------------------
 */
void
Fsrecov_DirOpRemove(clientData)
    ClientData		clientData;	/* Sequence number in log. */
{
    ReturnStatus	status;
    Recov_ObjectID	objectID;

    if (!finishedInit) {
	return;
    }

    objectID.typeID = FSRECOV_DIR_LOG_TYPE;
    objectID.objectNumber = fsrecov_DirLogObjNums[(int) clientData].objectNum;
    if (objectID.objectNumber < 0) {
	printf("Fsrecov_DirOpRemove: object already deleted.\n");
	return;
    }

    status = Recov_DeleteObject(objectID);
    if (status != SUCCESS) {
	panic("Fsrecov_DirOpRemove: couldn't delete log entry.");
    }
    fsrecov_DirLogObjNums[(int) clientData].objectNum = -1;

    return;
}

typedef	struct	FileIdent {
    int	major;
    int	minor;
} FileIdent;
typedef	struct	LogEntry {
    List_Links	links;
    Fsrecov_DirLogEntry	dirLogEntry;
} LogEntry;


/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_ProcessLog --
 *
 *	Process the log entries, making sure the disk state reflects them.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Disk state may be modified.
 *
 *----------------------------------------------------------------------
 */
void
Fsrecov_ProcessLog()
{
    FileIdent			fileIdent;
    LogEntry			*logEntryPtr;
    Fsrecov_DirLogEntry		dirLogEntry;
    Fsrecov_DirLogEntry		*dirLogEntryPtr;
    Hash_Table			descTable;
    Hash_Entry			*entryPtr = (Hash_Entry *) NIL;
    int				seqNum;
    LogEntry			*itemPtr;
    List_Links			*savePtr;
    List_Links			*otherItemPtr;
    Hash_Entry			*otherEntryPtr;
    ReturnStatus		status;
    Recov_ObjectID		objID;
    Fsdm_Domain			*domainPtr;

    seqNum = fsrecov_DirLog.oldestEntry;
    if (seqNum < 0) {
	finishedInit = TRUE;
	return;
    }
    Hash_Init(&descTable, 8, sizeof (FileIdent) / sizeof (int));

    /*
     * Try to put together a picture of what each mentioned directory
     * should contain in the way of names and desriptors.
     */

    objID.typeID = FSRECOV_DIR_LOG_TYPE;
    while (seqNum != fsrecov_DirLog.nextLogSeqNum) {
	/* Process the entry. */
	objID.objectNumber = fsrecov_DirLogObjNums[seqNum].objectNum;
	status = Recov_ReturnObject((ClientData) &dirLogEntry, objID, TRUE);
	if (status != SUCCESS) {
	    panic("Fsrecov_ProcessLog: couldn't get entry.");
	}
	fileIdent.major = 0;		/* XXX Fix this. XXX */
	fileIdent.minor = dirLogEntry.dirFileNumber;
	entryPtr = Hash_Find(&descTable, (Address) &fileIdent);
	logEntryPtr = (LogEntry *) malloc(sizeof (LogEntry));
	List_InitElement((List_Links *) logEntryPtr);
	bcopy(&dirLogEntry, &(logEntryPtr->dirLogEntry),
		sizeof (dirLogEntry));
	if (entryPtr->value == (Address) NIL) {
	    entryPtr->value = (Address) malloc(sizeof (List_Links));
	    List_Init((List_Links *) (entryPtr->value));
	    List_Insert((List_Links *) logEntryPtr,
		    LIST_ATREAR((List_Links *) entryPtr->value));
	    /*
	     * Do I just want to remove the entry?  I can't go through
	     * fast reboot anyway if it croaks here.
	     */
	    /* Reset time to now, so op will get written out before removed. */
	    fsrecov_DirLogObjNums[seqNum].timeStamp = Fsutil_TimeInSeconds();
	    seqNum++;
	    if (seqNum >= FSRECOV_NUM_DIR_LOG_ENTRIES) {
		seqNum = 0;
	    }
	    continue;
	}


	/* Entries already exist for this parent directory. */

	LIST_FORALL((List_Links *) entryPtr->value, (List_Links *)itemPtr) {
	    if ((itemPtr->dirLogEntry.dirEntry.nameLength ==
		    dirLogEntry.dirEntry.nameLength) &&
		    strncmp(itemPtr->dirLogEntry.dirEntry.fileName,
		    dirLogEntry.dirEntry.fileName,
		    dirLogEntry.dirEntry.nameLength) == 0) {
		int	op;
		/* Same filename. */

		switch (dirLogEntry.opFlags & FSDM_LOG_OP_MASK) {
		case FSDM_LOG_CREATE :
		    op = (itemPtr->dirLogEntry.opFlags & FSDM_LOG_OP_MASK);
		    if (op != FSDM_LOG_RENAME_DELETE && op != FSDM_LOG_UNLINK) {
			panic("Fsrecov_ProcessLog: incompatible log entry.");
		    }
		    break;
		case FSDM_LOG_LINK :
		    break;
		case FSDM_LOG_RENAME_DELETE :
		    break;
		case FSDM_LOG_RENAME_LINK :
		    break;
		case FSDM_LOG_UNLINK :	/* Fall through. */
		case FSDM_LOG_RENAME_UNLINK :
		    /* XXX What to do if it's still open? */
		    /* What if it's a dir? */
		    if (!(dirLogEntry.opFlags & FSDM_LOG_IS_DIRECTORY)) {
			break;
		    }
		    if (dirLogEntry.linkCount > 2) {
			break;
		    }
		    /*
		     * It can be removed - find anything it's a
		     * parent of and remove that from list.
		     */
		    fileIdent.major = 0;	/* XXX Fix this. XXX */
		    fileIdent.minor = dirLogEntry.dirEntry.fileNumber;
		    otherEntryPtr = Hash_LookOnly(&descTable,
			    (Address) &fileIdent);
		    if (otherEntryPtr->value == (Address) NIL) {
			break;
		    }
		    /* Remove stuff. */
		    while (!List_IsEmpty((List_Links *) otherEntryPtr->value)) {
			otherItemPtr = List_First((List_Links *)
				otherEntryPtr->value);
			List_Remove(otherItemPtr);
			free((char *) otherItemPtr);
		    }
		    free(otherEntryPtr->value);
		    Hash_Delete(&descTable, otherEntryPtr);
		    break;
		default:
		    panic("Fsrecov_ProcessLog: unknown operation.");
		    break;
		}
		savePtr = (List_Links *) itemPtr;
		itemPtr = (LogEntry *) List_Prev((List_Links *) itemPtr);
		List_Remove(savePtr);
	    }
	}
	List_Insert((List_Links *) logEntryPtr,
		LIST_ATREAR((List_Links *) entryPtr->value));
	/*
	 * Do I just want to remove the entry?  I can't go through
	 * fast reboot anyway if it croaks here.
	 */
	/* Reset time to now, so op will get written out before removed. */
        fsrecov_DirLogObjNums[seqNum].timeStamp = Fsutil_TimeInSeconds();
	seqNum++;
	if (seqNum >= FSRECOV_NUM_DIR_LOG_ENTRIES) {
	    seqNum = 0;
	}
    }

    domainPtr = Fsdm_DomainFetch(0 /* XXX Fix this. XXX */, FALSE /* TRUE? */);

    /* Now check disk to see that it matches state we've accumulated. */

    if (logProcessDebug) {	/* Just print what the state is. */
	Hash_Search		hashSearch;
	Hash_Entry		*entryPtr;
	Fsio_FileIOHandle	*parentHandlePtr;
	Fs_FileID		fileID;

	Hash_StartSearch(&hashSearch);
	printf("Fsrecov_ProcessLog: Printing state:\n");

	for (entryPtr = Hash_Next(&descTable, &hashSearch);
		entryPtr != (Hash_Entry *) NIL;
		entryPtr = Hash_Next(&descTable, &hashSearch)) {
	    char	buf[256];

	    fileID.type = 1;
	    fileID.serverID = rpc_SpriteID;
	    fileID.major = 0;	/* XXX Fix this! XXX */
	    fileID.minor = entryPtr->key.words[1];
	    parentHandlePtr = (Fsio_FileIOHandle *) Fsutil_HandleFetch(&fileID);
	    if (parentHandlePtr == (Fsio_FileIOHandle *) NIL) {
		status = Fsio_LocalFileHandleInit(&fileID,
			dirLogEntry.dirEntry.fileName,
			(Fsdm_FileDescriptor *) NIL, FALSE, &parentHandlePtr);
		if (status != SUCCESS) {
		    panic("Fsrecov_ProcessLog: couldn't get handle for dir.");
		}
	    }
	    LIST_FORALL((List_Links *) entryPtr->value,
		    (List_Links *) logEntryPtr) {
		dirLogEntryPtr = &(logEntryPtr->dirLogEntry);
		strncpy(buf, dirLogEntryPtr->dirEntry.fileName,
			dirLogEntryPtr->dirEntry.nameLength);
		buf[dirLogEntryPtr->dirEntry.nameLength] = '\0';
	    }
	    Fslcl_CheckDirLog(parentHandlePtr, (List_Links *) entryPtr->value);
	    Fsutil_HandleUnlock(parentHandlePtr);
	}
	    
	finishedInit = TRUE;
	return;
    }

    finishedInit = TRUE;
    return;
}


/*
 *----------------------------------------------------------------------
 *
 * LogOpString --
 *
 *	Return a string representing the operation logged.
 *
 * Results:
 *	The string.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
char *
LogOpString(op)
    int		op;
{
    static	char	returnString[256];

    switch (op & FSDM_LOG_OP_MASK) {
    case FSDM_LOG_CREATE :
	strcpy(returnString, "Create ");
	break;
    case FSDM_LOG_UNLINK :
	strcpy(returnString, "Unlink ");
	break;
    case FSDM_LOG_LINK :
	strcpy(returnString, "Link ");
	break;
    case FSDM_LOG_RENAME_DELETE :
	strcpy(returnString, "Rename delete ");
	break;
    case FSDM_LOG_RENAME_LINK :
	strcpy(returnString, "Rename link ");
	break;
    case FSDM_LOG_RENAME_UNLINK :
	strcpy(returnString, "Rename unlink ");
	break;
    default: 
	strcpy(returnString, "Unknown ");
	break;
    }
    
    if (op & FSRECOV_LOG_START_ENTRY) {
	strcat(returnString, "START ");
    }
    if (op & FSRECOV_LOG_END_ENTRY) {
	strcat(returnString, "END ");
    }
    if (op & FSDM_LOG_STILL_OPEN) {
	strcat(returnString, "OPEN ");
    }
    if (op & FSDM_LOG_IS_DIRECTORY) {
	strcat(returnString, "DIRECTORY ");
    }

    return returnString;
}

/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_GetComponent --
 *
 *	Return file name and its length from a dir log entry.
 *
 * Results:
 *	Name and length in parameters.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
void
Fsrecov_GetComponent(itemPtr, componentPtr, lengthPtr)
    List_Links	*itemPtr;
    char	**componentPtr;
    int		*lengthPtr;
{
    LogEntry	*logEntryPtr;

    logEntryPtr = (LogEntry *) itemPtr;
    *componentPtr = logEntryPtr->dirLogEntry.dirEntry.fileName;
    *lengthPtr = logEntryPtr->dirLogEntry.dirEntry.nameLength;

    return;
}

/*
 *----------------------------------------------------------------------
 *
 * Fsrecov_TestCmd --
 *
 *	Test timings of fsrecov and recov stuff.  This is called directly
 *	by Sys_StatsStub and therefore has the same interface.
 *
 * Results:
 *	Failure if test fails.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
ReturnStatus
Fsrecov_TestCmd(option, argPtr)
    int		option;
    Address	argPtr;
{
    ReturnStatus	status = SUCCESS;

    switch (option) {
    case RECOV_PRINT_REBOOT_TIMES: {
	Time	answer;
	int	sum;
	
	printf("Total objects recovered: %d\n", fsrecov_RebuildNumObjs);
	if (fsrecov_RebuildNumObjs <= 0) {
	    break;
	}
	Time_Subtract(fsrecov_EndRebuild, fsrecov_StartRebuild,
		&answer);
	printf("Time to do rebuild: %d seconds, %d microseconds\n",
		answer.seconds, answer.microseconds);
	sum = (answer.seconds * 1000000) + answer.microseconds;
	sum /= fsrecov_RebuildNumObjs;
	printf("Rebuild time per obj: %d microseconds \n", sum);
	Time_Subtract(fsrecov_ReturnedObjs, fsrecov_StartRebuild,
		&answer);
	printf("Time to return objs: %d seconds, %d microseconds\n",
		answer.seconds, answer.microseconds);
	Time_Subtract(fsrecov_BuiltHashTable, fsrecov_ReturnedObjs,
		&answer);
	printf("Time to build hashtable: %d seconds, %d microseconds\n",
		answer.seconds, answer.microseconds);
	Time_Subtract(fsrecov_BuiltIOHandles, fsrecov_BuiltHashTable,
		&answer);
	printf("Time to do IOHandles: %d seconds, %d microseconds\n",
		answer.seconds, answer.microseconds);
	Time_Subtract(fsrecov_EndRebuild, fsrecov_BuiltIOHandles,
		&answer);
	printf("Time to do streams: %d seconds, %d microseconds\n",
		answer.seconds, answer.microseconds);

	break;
    }
    case RECOV_TEST_ADD_DELETE: {
	Fsio_FileIOHandle	handle;
	Fs_HandleHeader		*hdrPtr = (Fs_HandleHeader *) &handle;
	struct Fsdm_FileDescriptor desc;
	int		q;
	Time 		time1;
	Time 		time2;
	Time 		answer;
	Recov_ObjectID	objID;
	int		numHandles;
	int		iterations;
	int		sum;
	Recov_ObjectID	*objArrayPtr;
	int		i;

	if (argPtr == (Address) NIL || argPtr == (Address) 0) {
	    numHandles = 1;
	    iterations = 1000;
	} else {
	    numHandles = ((Recov_Timings *) argPtr)->numHandles;
	    iterations = ((Recov_Timings *) argPtr)->iterations;
	}
	hdrPtr->fileID.type = FSIO_LCL_FILE_STREAM;
	hdrPtr->fileID.serverID = 53;
	hdrPtr->fileID.major = 99;
	hdrPtr->fileID.minor = 99;
	handle.descPtr = &desc;

	Timer_GetTimeOfDay(&time1, (int *) NIL, (Boolean *) NIL);

	for (q = 0; q < iterations; q++) {
	    for (i = 0; i < numHandles; i++) {
		hdrPtr->fileID.minor = 99 + i;
		if (Fsrecov_AddHandle(hdrPtr, (Fs_FileID *) NIL,
			99, 0, 1, TRUE) != SUCCESS) {
		    printf("BAD FS_ADD_HANDLE.\n");
		    status = FAILURE;
		    break;
		}
	    }
	    if (status != SUCCESS) {
		break;
	    }
	    for (i = 0; i < numHandles; i++) {
		hdrPtr->fileID.minor = 99 + i;
		if (Fsrecov_DeleteHandle(hdrPtr, 99,
			FS_LAST_DIRTY_BLOCK) != SUCCESS) {
		    printf("BAD FS_DELETE_HANDLE.\n");
		    status = FAILURE;
		    break;
		}
	    }
	    if (status != SUCCESS) {
		break;
	    }
	}
	if (status != SUCCESS) {
	    break;
	}
	Timer_GetTimeOfDay(&time2, (int *) NIL, (Boolean *) NIL);
	Time_Subtract(time2, time1, &answer);
	sum = (answer.seconds * 1000000) + answer.microseconds;
	sum /= iterations;
	sum /= numHandles;
	printf(
	"FS ANSWER, %d iterations, %d handles: %d seconds, %d usecs\n",
		iterations, numHandles, answer.seconds,
		answer.microseconds);
	printf("FS ANSWER per iteration per handle: %d microseconds\n",
		sum);
	

	objArrayPtr = (Recov_ObjectID *)
		malloc(numHandles * sizeof (Recov_ObjectID));
	Timer_GetTimeOfDay(&time1, (int *) NIL, (Boolean *) NIL);
	for (q = 0; q < iterations; q++) {
	    for (i = 0; i < numHandles; i++) {
		hdrPtr->fileID.minor = 99 + i;
		if (Recov_InsertObject(0, (ClientData) hdrPtr,
			-1, &objID) != SUCCESS) {
		    printf("BAD RECOV ADD\n");
		    status = FAILURE;
		    break;
		}
		objArrayPtr[i] = objID;
	    }
	    if (status != SUCCESS) {
		break;
	    }
	    for (i = 0; i < numHandles; i++) {
		hdrPtr->fileID.minor = 99 + i;
		if (Recov_DeleteObject(objArrayPtr[i]) != SUCCESS) {
		    printf("BAD RECOV DELETE\n");
		    status = FAILURE;
		    break;
		}
	    }
	    if (status != SUCCESS) {
		break;
	    }
	}
	if (status != SUCCESS) {
	    break;
	}
	Timer_GetTimeOfDay(&time2, (int *) NIL, (Boolean *) NIL);
	Time_Subtract(time2, time1, &answer);
	printf(
    "RECOV ANSWER, %d iterations, %d handles: %d seconds, %d usecs\n",
	    iterations, numHandles, answer.seconds,
	    answer.microseconds);
	sum = (answer.seconds * 1000000) + answer.microseconds;
	sum /= iterations;
	sum /= numHandles;
	printf(
	    "RECOV_ANSWER per iteration per handle: %d microseconds\n",
	    sum);

	break;
    }
    case RECOV_TEST_ADD: {
	Fsio_FileIOHandle	handle;
	Fs_HandleHeader		*hdrPtr = (Fs_HandleHeader *) &handle;
	struct Fsdm_FileDescriptor desc;
	int		q;
	Time 		time1;
	Time 		time2;
	Time 		answer;
	Recov_ObjectID	objID;
	int		numHandles;
	int		iterations;
	int		sum;
	Recov_ObjectID	*objArrayPtr;
	int		i;

	if (argPtr == (Address) NIL || argPtr == (Address) 0) {
	    numHandles = 1;
	    iterations = 1000;
	} else {
	    numHandles = ((Recov_Timings *) argPtr)->numHandles;
	    iterations = ((Recov_Timings *) argPtr)->iterations;
	}
	hdrPtr->fileID.type = FSIO_LCL_FILE_STREAM;
	hdrPtr->fileID.serverID = 53;
	hdrPtr->fileID.major = 99;
	hdrPtr->fileID.minor = 99;
	handle.descPtr = &desc;


	sum = 0;
	for (q = 0; q < iterations; q++) {
	    Timer_GetTimeOfDay(&time1, (int *) NIL, (Boolean *) NIL);
	    for (i = 0; i < numHandles; i++) {
		hdrPtr->fileID.minor = 99 + i;
		if (Fsrecov_AddHandle(hdrPtr, (Fs_FileID *) NIL,
			99, 0, 1, TRUE) != SUCCESS) {
		    printf("BAD FS_ADD_HANDLE.\n");
		    status = FAILURE;
		    break;
		}
	    }
	    if (status != SUCCESS) {
		break;
	    }
	    Timer_GetTimeOfDay(&time2, (int *) NIL, (Boolean *) NIL);
	    for (i = 0; i < numHandles; i++) {
		hdrPtr->fileID.minor = 99 + i;
		if (Fsrecov_DeleteHandle(hdrPtr, 99,
			FS_LAST_DIRTY_BLOCK) != SUCCESS) {
		    printf("BAD FS_DELETE_HANDLE.\n");
		    status = FAILURE;
		    break;
		}
	    }
	    if (status != SUCCESS) {
		break;
	    }
	    Time_Subtract(time2, time1, &answer);
	    sum += (answer.seconds * 1000000) + answer.microseconds;
	}
	if (status != SUCCESS) {
	    break;
	}
	printf(
	"FS ANSWER, %d iterations, %d handles\n", iterations,
		numHandles);
	sum /= iterations;
	sum /= numHandles;
	printf("FS ANSWER per iteration per handle: %d microseconds\n",
		sum);
	

	objArrayPtr = (Recov_ObjectID *)
		malloc(numHandles * sizeof (Recov_ObjectID));
	sum = 0;
	for (q = 0; q < iterations; q++) {
	    Timer_GetTimeOfDay(&time1, (int *) NIL, (Boolean *) NIL);
	    for (i = 0; i < numHandles; i++) {
		hdrPtr->fileID.minor = 99 + i;
		if (Recov_InsertObject(0, (ClientData) hdrPtr,
			-1, &objID) != SUCCESS) {
		    printf("BAD RECOV ADD\n");
		    status = FAILURE;
		    break;
		}
		objArrayPtr[i] = objID;
	    }
	    if (status != SUCCESS) {
		break;
	    }
	    Timer_GetTimeOfDay(&time2, (int *) NIL, (Boolean *) NIL);
	    for (i = 0; i < numHandles; i++) {
		hdrPtr->fileID.minor = 99 + i;
		if (Recov_DeleteObject(objArrayPtr[i]) != SUCCESS) {
		    printf("BAD RECOV DELETE\n");
		    status = FAILURE;
		    break;
		}
	    }
	    if (status != SUCCESS) {
		break;
	    }
	    Time_Subtract(time2, time1, &answer);
	    sum += (answer.seconds * 1000000) + answer.microseconds;
	}
	if (status != SUCCESS) {
	    break;
	}
	printf( "RECOV ANSWER, %d iterations, %d handles\n", iterations,
		numHandles);
	sum /= iterations;
	sum /= numHandles;
	printf(
	    "RECOV_ANSWER per iteration per handle: %d microseconds\n",
	    sum);

	break;
    }
    default:
	printf("Fsrecov_TestCmd: Invalid argument.\n");
	break;
    }
    return status;
}
#undef CHECK
