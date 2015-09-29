/* 
 * fsmake.c --
 *
 *	Make a sprite file system on a raw disk.
 *
 * IMPORTANT NOTE ABOUT PORTABILITY:
 *
 *	I use the sprite directory format even when I am reading a UNIX
 *	directory.  I can get a way with this because they are the same.  The
 *	reason why I don't use the UNIX one is that on a PMAX there are some
 *	defines in "/usr/include/sys/dir.h" which screw up the compilation
 *	of this file.  If this runs on a UNIX system that does not have
 *	a compatible directory format then the code in CopyTree must be
 *	modified.
 *
 * Copyright (C) 1989 by Digital Equipment Corporation, Maynard MA
 *
 *			All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the name of Digital not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 *
 * Digitial disclaims all warranties with regard to this software, including
 * all implied warranties of merchantability and fitness.  In no event shall
 * Digital be liable for any special, indirect or consequential damages or
 * any damages whatsoever resulting from loss of use, data or profits,
 * whether in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of this
 * software.
 */

#ifndef lint
static char rcsid[] = "$Header: /sprite/src/admin/fsinstall/RCS/fsmake.c,v 1.14 90/02/17 23:47:50 nelson Exp $ SPRITE (Berkeley)";
#endif

#include "sprite.h"
#include "option.h"
#include "disk.h"
#include <stdio.h>
#include <sys/file.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <assert.h>
#include <sys/time.h>

#ifdef direct
#undef direct
#endif

#if defined(sprite) || defined(sun)
#define S_GFDIR     S_IFDIR
#define S_GFMT      S_IFMT
#define S_GFLNK     S_IFLNK
#define S_GFREG      S_IFREG
#endif

/*
 * Constants settable via the command line.
 */
int kbytesToFileDesc = 4;	/* The ratio of kbytes to
				 * the number of file descriptors */
Boolean printOnly = FALSE;	/* Stop after computing the domain header
				 * and just print it out. No disk writes */
Boolean overlapBlocks = FALSE;	/* Allow filesystem blocks to overlap track
				 * boundaries.  Some disk systems can't deal. */
char *deviceName;		/* Set to "rsd0" or "rxy1", etc. */
char *partName;			/* Set to "a", "b", "c" ... "g" */
char defaultFirstPartName[] = "a";
char *diskType = NULL;		/* Type of disk (e.g. rz23) */
int  spriteID = 1;		/* This machines sprite id. */
char *devFileName = NULL;	/* Name of file that contains devices to 
				 * create. */
char *dirName = NULL;		/* Name of directory that contains files to
				 * copy to the disk. */
Boolean newLabel = FALSE;

Option optionArray[] = {
    {OPT_STRING, "dev", (Address)&deviceName,
	"Required: Name of device, eg \"rsd0\" or \"rxy1\""},
    {OPT_STRING, "part", (Address)&partName,
	"Required: Partition ID: (a, b, c, d, e, f, g)"},
    {OPT_TRUE, "overlap", (Address)&overlapBlocks,
	"Overlap filesystem blocks across track boundaries (FALSE)"},
    {OPT_INT, "ratio", (Address)&kbytesToFileDesc,
	"Ratio of Kbytes to file descriptors (4)"},
    {OPT_TRUE, "test", (Address)&printOnly,
	"Test: print results, don't write disk (FALSE)"},
    {OPT_FALSE, "write", (Address)&printOnly,
	"Write the disk (TRUE)"},
    {OPT_STRING, "type", (Address)&diskType,
	"Type of disk.  Used to look up disk info in /etc/disktab"},
    {OPT_INT, "sid", (Address)&spriteID,
	"Sprite ID of this workstation"},
    {OPT_STRING, "dir", (Address)&dirName,
	"Directory to copy files from"},
    {OPT_STRING, "devFile", (Address)&devFileName,
	"File that contains devices to create"},
    {OPT_TRUE, "newLabel", (Address)&newLabel,
	"Overwrite the current label on the disk if there is one"},
};
int numOptions = sizeof(optionArray) / sizeof(Option);

/*
 * Structure used to peruse Sprite directories.
 */
typedef struct DirIndexInfo {
    Fsdm_FileDescriptor *fdPtr;	     	     /* The file descriptor being
					      * read. */
    int		 blockNum;		     /* Block that is being read, 
					      * written, or allocated. */
    int		 blockAddr;		     /* Address of directory block
					      * to read. */
    int		 dirOffset;		     /* Offset of the directory entry 
					      * that we are currently examining 
					      * in the directory. */
    char	 dirBlock[FS_BLOCK_SIZE];    /* Where directory data is 
					      * stored. */
} DirIndexInfo;

/*
 * Time of day when this program runs.
 */

struct timeval curTime;

/*
 * Forward Declarations.
 */
void		SetSummaryInfo();
void		SetDomainHeader();
void		SetDiskGeometry();
void		SetDomainParts();
void		WriteAllFileDescs();
void		WriteAndInitDataBitmap();
unsigned char	*ReadFileDescBitmap();
void		WriteFileDescBitmap();
unsigned char	*ReadBitmap();
void		WriteBitmap();
char 		*MakeFileDescBitmap();
Disk_Info	*ScanDiskTab();
static Fslcl_DirEntry	*OpenDir();
Fslcl_DirEntry	*NextDirEntry();
static void		CloseDir();
Fslcl_DirEntry	*AddToDirectory();
void		CreateDir();
void		MarkDataBitmap();
void		InitDesc();
void		CopyTree();
void		ReadFileDesc();
void		WriteFileDesc();
void		MakeDevices();
void		WriteRootDirectory();

/*
 * Macro to get a pointer into the bit map for a particular block.
 */
#define BlockToCylinder(domainPtr, blockNum) \
    (blockNum) / (domainPtr)->geometry.blocksPerCylinder

#define GetBitmapPtr(domainPtr, bitmapPtr, blockNum) \
  &((bitmapPtr)[BlockToCylinder(domainPtr, blockNum) * \
  bytesPerCylinder + (blockNum) % (domainPtr)->geometry.blocksPerCylinder / 2])

/*
 * Macros to convert physical block numbers to virtual block numbers. All direct
 * blocks are virtual, indirect blocks are physical.
 */
#define VirtToPhys(blockNum) \
    ((blockNum) + (domainPtr)->dataOffset * FS_FRAGMENTS_PER_BLOCK)

#define PhysToVirt(domainPtr,blockNum) \
    ((blockNum) - (domainPtr)->dataOffset * FS_FRAGMENTS_PER_BLOCK)

/*
 * Macro to mark the file descriptor bit map.
 */
#define MarkFDBitmap(num,bitmapPtr) \
    (bitmapPtr)[(num) >> 3] |= (1 << (7 -((num)  & 7)))

int			freeFDNum;	/* The currently free file descriptor.*/
int			freeBlockNum;	/* The currently free data block. */
Fsdm_FileDescriptor	devFD;		/* The file descriptor for the dev
					     * directory. */
Fsdm_FileDescriptor	*devFDPtr;	/* Pointer to the file descriptor for
					 * the dev directory. */
int			devFDNum;	/* The file number of the dev 
					 * directory. */
int			partFID;	/* The file id of the partition that
					 * we are initializing. */
Fsdm_DomainHeader 		*domainPtr;	/* The domain the we are initializing.*/
int			partition;	/* The partition that we are 
					 * initializing. */
Disk_Info		*diskInfoPtr;	/* Information about the disk that
					 * we are initializing. */
Fsdm_SummaryInfo 		*summaryPtr;	/* Summary information for the domain.*/
unsigned char		*fdBitmapPtr;	/* Pointer to the file descriptor
					 * bitmap. */
unsigned char		*cylBitmapPtr;	/* Pointer to the cylinder bit map. */
int			bytesPerCylinder;/* The number of bytes in
					  * the bitmap for a cylinder.*/



/*
 * Some number of sectors in the root partition must be allocated to the
 * boot program. The default is for the new filesystem to have the same
 * number of boot sectors as the old filesystem. If the disk did not
 * previously have a filesystem, or if the domain header cannot be found,
 * then the following number of boot sectors are allocated. The standard
 * Sun format is for the summary sector to be in sector #17. 16 boot sectors
 * and one disk label fill the first 17 sectors.
 */
int	defaultBootSectors = 16;

int bootSectors = -1;


/*
 *----------------------------------------------------------------------
 *
 * main --
 *
 *	Create the required file names from the command line
 *	arguments.  Then open the first partition on the disk
 *	because it contains the disk label, and open the partition
 *	that is to be formatted.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Calls MakeFilesystem
 *
 *----------------------------------------------------------------------
 */
main(argc, argv)
    int		argc;
    char	*argv[];
{
    char	answer[10];
    char	partitionName[64];
    int		status;
    static      char block[DEV_BYTES_PER_SECTOR];
    int         i;

    gettimeofday(&curTime, NULL);

    (void)Opt_Parse(argc, argv, optionArray, numOptions, 0);

    if (deviceName == (char *)0) {
	fprintf(stderr,"Specify device name with -dev option\n");
	exit(1);
    }
    if (partName == (char *)0) {
	fprintf(stderr,"Specify partition with -part option\n");
	exit(1);
    } 
    if (spriteID == 0) {
	fprintf(stderr, "Specify sprite id with -sid option\n");
	exit(1);
    }

    bootSectors = defaultBootSectors;

    if (!printOnly) {
	printf("The \"-write\" option will cause fsmake to overwrite the current filesystem.\nDo you really want to do this?[y/n] ");
	if (scanf("%10s",answer) != 1) {
	    exit(0);
	}
	if ((*answer != 'y') && (*answer != 'Y')) {
	    exit(0);
	}
    }

    if (partName[0] != 'a' && partName[0] != 'c') {
	fprintf(stderr, "Can only format partitions a or c\n");
	exit(1);
    }

    /*
     * Gen up the name of the first partition on the disk.
     */
    sprintf(partitionName, "/dev/%s%s", deviceName, partName);

    if (printOnly) {
	partFID = open(partitionName, O_RDONLY);
    } else {
	partFID = open(partitionName, O_RDWR);
    }
    if (partFID < 0 ) {
	perror("Can't open first partition");
	exit(1);
    }

    printf("fsmake based on 4K filesystem blocks\n");

    partition = partName[0] - 'a';

    diskInfoPtr = NULL;
    if (!newLabel) {
	/*
	 * See if we can read the copy of the super block at the beginning
	 * of the partition to find out basic disk geometry and where to
	 * write the domain header.  This will only work with disks that
	 * have either a sun label or a sprite label.
	 */
	diskInfoPtr = Disk_ReadDiskInfo(partFID, partition);
    }
    if (diskInfoPtr == NULL && diskType != NULL) {
	/*
	 * See if we can find the information in /etc/disktab.
	 */
	diskInfoPtr = ScanDiskTab();
    }
    if (diskInfoPtr == NULL) {
	fprintf(stderr,"MakeFilesystem: Unable to read super block.\n");
	return(1);
    }

    /*
     * Clear out the old summary sector and domain header. This is especially
     * important if we are going to move them.
     */
    if ((!printOnly) && (diskInfoPtr->summarySector != -1)) {
	status = Disk_SectorWrite(partFID, diskInfoPtr->summarySector,1, block);
	if (status != SUCCESS) {
	    perror("Clear of old summary sector failed"); 
	    return(status);
	}
	for (i = 0; i < diskInfoPtr->numDomainSectors; i++) {
	    status = Disk_SectorWrite(partFID, diskInfoPtr->domainSector+i,
			    1, block);
	    if (status != SUCCESS) {
		perror("Clear of old domain header failed"); 
		return(status);
	    }
	}
    }
    /* 
     * The disk did not previously have a filesystem on it.
     */
    if (diskInfoPtr->summarySector == -1) {
	diskInfoPtr->summarySector = bootSectors + 1;
	diskInfoPtr->domainSector = bootSectors + 2;
    }
    domainPtr = (Fsdm_DomainHeader *)
	malloc((unsigned) diskInfoPtr->numDomainSectors * DEV_BYTES_PER_SECTOR);
    SetDomainHeader();
    bytesPerCylinder = (domainPtr->geometry.blocksPerCylinder + 1) / 2;
    Disk_PrintDomainHeader(domainPtr);

    if (!printOnly) {
	assert(diskInfoPtr->domainSector >= 0);
	status = Disk_SectorWrite(partFID, diskInfoPtr->domainSector,
			    diskInfoPtr->numDomainSectors, (Address)domainPtr);
	if (status != 0) {
	    perror("DomainHeader write failed");
	    return(status);
	}
    }

    summaryPtr = (Fsdm_SummaryInfo *) malloc(DEV_BYTES_PER_SECTOR);
    SetSummaryInfo();
    Disk_PrintSummaryInfo(summaryPtr);
    if (!printOnly) {
	assert(diskInfoPtr->summarySector >= 0);
	status = Disk_SectorWrite(partFID, diskInfoPtr->summarySector, 1,
			    (Address)summaryPtr);
	if (status != 0) {
	    perror("Summary sector write failed");
	    return(status);
	}
    }

    WriteAllFileDescs();
    WriteAndInitDataBitmap();
    WriteRootDirectory();

    /*
     * We now have a good empty file system.  Add any files and devices
     * that need to be added.
     */
    if (dirName != NULL) {
	Fsdm_FileDescriptor	rootDesc;

	fdBitmapPtr = ReadFileDescBitmap();
	freeFDNum = 3;
	cylBitmapPtr = ReadBitmap();
	freeBlockNum = 1;
	ReadFileDesc(FSDM_ROOT_FILE_NUMBER, &rootDesc);
	CopyTree(dirName, FSDM_ROOT_FILE_NUMBER, &rootDesc,
	    FSDM_ROOT_FILE_NUMBER, FALSE, "/");
	WriteFileDesc(FSDM_ROOT_FILE_NUMBER, &rootDesc);
	if (devFileName != NULL) {
	    if (devFDPtr == NULL) {
		fprintf(stderr, "Couldn't find /dev\n");
		exit(1);
	    }
	    MakeDevices();
	}
	if (!printOnly) {
	    assert(diskInfoPtr->summarySector);
	    status = Disk_SectorWrite(partFID, diskInfoPtr->summarySector, 1,
				(Address)summaryPtr);
	    if (status != 0) {
		perror("Summary sector write failed (2)");
		exit(status);
	    }
	    WriteFileDescBitmap(fdBitmapPtr);
	    WriteBitmap(cylBitmapPtr);
	}
    }

    fflush(stderr);
    fflush(stdout);
    (void)close(partFID);
    exit(0);
}


/*
 *----------------------------------------------------------------------
 *
 * SetDomainHeader --
 *
 *	Compute the domain header based on the partition size and
 *	other basic disk parameters.
 *
 * Results:
 *	A return code.
 *
 * Side effects:
 *	Fill in the domain header.
 *
 *----------------------------------------------------------------------
 */
void
SetDomainHeader()
{
    register Fsdm_Geometry *geoPtr;

    domainPtr->magic = FSDM_DOMAIN_MAGIC;
    domainPtr->firstCylinder = diskInfoPtr->firstCylinder;
    domainPtr->numCylinders = diskInfoPtr->numCylinders;
    /*
     * The device.serverID from the disk is used during boot to discover
     * the host"s spriteID if reverse arp couldn't find a host ID.  The
     * unit number of disk indicates what partition of the disk this
     * domain header applies to.  For example, both the "a" and "c" partitions
     * typically start at sector zero, but only one is valid.  During boot
     * time the unit number is used to decide which partition should be
     * attached.
     */
    domainPtr->device.serverID = spriteID;
    domainPtr->device.type = -1;
    domainPtr->device.unit = partition;
    domainPtr->device.data = (ClientData)-1;

    geoPtr = &domainPtr->geometry;
    SetDiskGeometry(&domainPtr->geometry);

    SetDomainParts();
}

/*
 *----------------------------------------------------------------------
 *
 * SetDiskGeometry --
 *
 *	This computes the rotational set arrangment depending on the
 *	disk geometry.  The basic rules for this are that filesystem blocks
 *	are skewed on successive tracks, and that the skewing pattern
 *	repeats in either 2 or 4 tracks.  This is specific to the fact that
 *	filesystem blocks are 4Kbytes.  This means that one disk track
 *	contains N/4 filesystem blocks and that one sector per track
 *	is wasted if there are an odd number of sectors per track.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Fill in the geometry struct.
 *
 *----------------------------------------------------------------------
 */
void
SetDiskGeometry(geoPtr)
    register Fsdm_Geometry	*geoPtr;	/* Fancy geometry information */
{
    int index;			/* Array index */
    int numBlocks;		/* The number of blocks in a rotational set */
    int tracksPerSet;		/* Total number of tracks in a rotational set */
    int numTracks;		/* The number of tracks in the set so far */
    int extraSectors;		/* The number of leftover sectors in a track */
    int offset;			/* The sector offset within a track */
    int startingOffset;		/* The offset of the first block in a track */
    int offsetIncrement;	/* The skew of the starting offset on each
				 * successive track of the rotational set */
    Boolean overlap;		/* TRUE if filesystem blocks overlap tracks */

    geoPtr->numHeads = diskInfoPtr->numHeads;
    geoPtr->sectorsPerTrack = diskInfoPtr->numSectors;

    /*
     * Figure out some basic parameters of the rotational set.  The number
     * of tracks in the set is either 2 or 4.  If 2, then the blocks on
     * successive tracks are skewed by 1/2 a filesystem block.  If 4,
     * blocks are skewed by 1/4 block.  A 4 track rotational set is best
     * becasue there are more rotational positions.  If, however, it
     * causes 2 or 3 wasted tracks at the end, or if blocks naturally
     * overlap by 1/2 block, then only 2 tracks per rotational set are
     * used.
     */
    switch(geoPtr->numHeads % 4) {
	case 0:
	case 1: {
	    extraSectors = geoPtr->sectorsPerTrack % DISK_SECTORS_PER_BLOCK;
	    if (extraSectors < DISK_SECTORS_PER_BLOCK/4) {
		/*
		 * Not enough extra sectors to overlap blocks onto the
		 * next track.  The blocks will fit evenly on a track,
		 * but the blocks on the following tracks will be skewed.
		 */
		tracksPerSet = 4;
		overlap = FALSE;
		offsetIncrement = DISK_SECTORS_PER_BLOCK/4;
	    } else if (extraSectors < DISK_SECTORS_PER_BLOCK/2) {
		/*
		 * Enough to overlap the first 1/4 block onto the next track.
		 */
		tracksPerSet = 4;
		overlap = TRUE;
		offsetIncrement = DISK_SECTORS_PER_BLOCK * 3/4;
	    } else if (extraSectors < DISK_SECTORS_PER_BLOCK * 3/4) {
		/*
		 * Enough to overlap 1/2 block.
		 */
		tracksPerSet = 2;
		overlap = TRUE;
		offsetIncrement = DISK_SECTORS_PER_BLOCK/2;
	    } else {
		/*
		 * Enough to overlap 3/4 block.
		 */
		tracksPerSet = 4;
		overlap = TRUE;
		offsetIncrement = DISK_SECTORS_PER_BLOCK/4;
	    }
	    break;
	}
	case 2:
	case 3: {
	    /*
	     * Instead of wasting 2 or 3 tracks to have a 4 track rotational
	     * set, the rotational set is only 2 tracks long.  Also see if
	     * the blocks naturally overlap by 1/2 block.
	     */
	    tracksPerSet = 2;
	    offsetIncrement = DISK_SECTORS_PER_BLOCK/2;
	    if ((geoPtr->sectorsPerTrack % DISK_SECTORS_PER_BLOCK) <
		      DISK_SECTORS_PER_BLOCK/2) {
		overlap = FALSE;
	    } else {
		overlap = TRUE;
	    }
	}
    }
    if (!overlapBlocks) {
	overlap = FALSE;
	offsetIncrement = 0;
    }
    printf("overlap %s, offsetIncrement %d\n", (overlap ? "TRUE" : "FALSE"),
		      offsetIncrement);
    /*
     * Determine rotational position of the blocks in the rotational set.
     */
    extraSectors = geoPtr->sectorsPerTrack;
    startingOffset = 0;
    offset = startingOffset;
    for (numBlocks = 0, numTracks = 0 ; ; ) {
	if (extraSectors >= DISK_SECTORS_PER_BLOCK) {
	    /*
	     * Ok to fit in another filesystem block on this track.
	     */
	    geoPtr->blockOffset[numBlocks] = offset;
	    numBlocks++;	
	    offset += DISK_SECTORS_PER_BLOCK;
	    extraSectors -= DISK_SECTORS_PER_BLOCK;
	} else {
	    /*
	     * The current block has to take up room on the next track.
	     */
	    numTracks++;
	    if (numTracks < tracksPerSet) {
		/*
		 * Ok to go to the next track.
		 */
		startingOffset += offsetIncrement;
		if (overlap) {
		    /*
		     * If the current block can overlap to the next track,
		     * use the current offset.  Because of the overlap
		     * there are fewer sectors available for blocks on
		     * the next track.
		     */
		    geoPtr->blockOffset[numBlocks] = offset;
		    numBlocks++;
		    extraSectors = geoPtr->sectorsPerTrack - startingOffset;
		}
		offset = startingOffset + numTracks * geoPtr->sectorsPerTrack;
		if (!overlap) {
		    /*
		     * If no overlap the whole next track is available.
		     */
		    extraSectors = geoPtr->sectorsPerTrack;
		}
	    } else {
		/*
		 * Done.
		 */
		for (index = numBlocks; index < FSDM_MAX_ROT_POSITIONS; index++){
		    geoPtr->blockOffset[index] = -1;
		}
		break;
	    }
	}
    }
    geoPtr->blocksPerRotSet = numBlocks;
    geoPtr->tracksPerRotSet = tracksPerSet;
    geoPtr->rotSetsPerCyl = geoPtr->numHeads / tracksPerSet;
    geoPtr->blocksPerCylinder = numBlocks * geoPtr->rotSetsPerCyl;

    /*
     * Now the rotational positions have to be sorted so that rotationally
     * optimal blocks can be found.  The array sortedOffsets is set so
     * that the I'th element has the index into blockOffset which contains
     * the I'th rotational position, eg.
     *	blockOffset	sortedOffsets
     *	    0 (+0)		0
     *	    8 (+0)		2
     *	    4 (+17)		1
     *	   12 (+17)		3
     */

    offsetIncrement = DISK_SECTORS_PER_BLOCK / tracksPerSet;
    for (index = 0 ; index < FSDM_MAX_ROT_POSITIONS ; index++) {
	geoPtr->sortedOffsets[index] = -1;
    }
    for (index = 0 ; index < numBlocks ; index++) {
	offset = geoPtr->blockOffset[index] % geoPtr->sectorsPerTrack;
	geoPtr->sortedOffsets[offset/offsetIncrement] = index;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * SetDomainParts --
 *
 *	Set up the way the domain is divided into 4 areas:  the bitmap
 *	for the file descriptors, the file descriptors, the bitmap for
 *	the data blocks, and the data blocks.
 *
 * Results:
 *	The geometry information is completed.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
void
SetDomainParts()
{
    register Fsdm_Geometry *geoPtr;
    int			numFiles;
    int 		numBlocks;
    int 		offset;	
    int 		numSectors;
    int			numSets;
    int 		bitmapBytes;

    /*
     * Set aside a number of blocks at the begining of the partition for
     * things like the super block, the boot program, and the domain header.
     * It is easiest to do this by reserving one or more rotational sets.
     */
    geoPtr = &domainPtr->geometry;
    numSectors = geoPtr->tracksPerRotSet * geoPtr->sectorsPerTrack;
    for ( numSets = 1; ; numSets++ ) {
	if (numSets * numSectors >
	    diskInfoPtr->domainSector + diskInfoPtr->numDomainSectors) {
	    break;
	}
    }
    printf("Reserving %d blocks for domain header, etc.\n",
		    numSets*geoPtr->blocksPerRotSet);
    /*
     * Determine the number of filesystem blocks available and compute a
     * first guess at the number of file descriptors.  If at the end of
     * the computation things don't fit nicely, then the number of files
     * is changed and the computation is repeated.
     */
    numFiles = 0;
    do {
	numBlocks = geoPtr->blocksPerCylinder * diskInfoPtr->numCylinders -
		    numSets * geoPtr->blocksPerRotSet;
	if (numFiles == 0) {
	    numFiles = numBlocks * DISK_KBYTES_PER_BLOCK / kbytesToFileDesc;
	}
	numFiles		  &= ~(FSDM_FILE_DESC_PER_BLOCK-1);
	offset			  = numSets * geoPtr->blocksPerRotSet;

	domainPtr->fdBitmapOffset = offset;
	bitmapBytes		  = (numFiles - 1) / BITS_PER_BYTE + 1;
	domainPtr->fdBitmapBlocks = (bitmapBytes - 1) / FS_BLOCK_SIZE + 1;
	numBlocks		  -= domainPtr->fdBitmapBlocks;
	offset			  += domainPtr->fdBitmapBlocks;

	domainPtr->fileDescOffset = offset;
	domainPtr->numFileDesc	  = numFiles;
	numBlocks		  -= numFiles / FSDM_FILE_DESC_PER_BLOCK;
	offset			  += numFiles / FSDM_FILE_DESC_PER_BLOCK;
	/*
	 * The data blocks will start on a cylinder.  Try the next
	 * cylinder boundary after the start of the bitmap.
	 */
	domainPtr->bitmapOffset	  = offset;
	domainPtr->dataOffset	  = ((offset-1) / geoPtr->blocksPerCylinder + 1)
				     * geoPtr->blocksPerCylinder;
	domainPtr->dataBlocks	  = domainPtr->numCylinders *
				      geoPtr->blocksPerCylinder -
				      domainPtr->dataOffset;
	bitmapBytes		  = (domainPtr->dataBlocks * DISK_KBYTES_PER_BLOCK -
				       1) / BITS_PER_BYTE + 1;
	domainPtr->bitmapBlocks	  = (bitmapBytes - 1) / FS_BLOCK_SIZE + 1;
	/*
	 * Check the size of the bit map against space available for it
	 * between the end of the file descriptors and the start of the
	 * data blocks.
	 */
	if (domainPtr->dataOffset - domainPtr->bitmapOffset <
	    domainPtr->bitmapBlocks) {
	    int numBlocksNeeded;
	    /*
	     * Need more blocks to hold the bitmap.  Reduce the number
	     * of file descriptors to get the blocks and re-iterate.
	     */
	    numBlocksNeeded = domainPtr->bitmapBlocks -
		(domainPtr->dataOffset - domainPtr->bitmapOffset);
	    numFiles -= numBlocksNeeded * FSDM_FILE_DESC_PER_BLOCK;
	} else if (domainPtr->dataOffset - domainPtr->bitmapOffset >
		    domainPtr->bitmapBlocks) {
	    int extraBlocks;
	    /*
	     * There are extra blocks between the end of the file descriptors
	     * and the start of the bitmap.  Increase the number of
	     * file descriptors and re-iterate.
	     */
	    extraBlocks = domainPtr->dataOffset - domainPtr->bitmapOffset -
		    domainPtr->bitmapBlocks;
	    numFiles += extraBlocks * FSDM_FILE_DESC_PER_BLOCK;
	}
    } while (domainPtr->dataOffset - domainPtr->bitmapOffset !=
		domainPtr->bitmapBlocks);
    domainPtr->dataCylinders	= domainPtr->dataBlocks /
				  geoPtr->blocksPerCylinder ;
}

/*
 *----------------------------------------------------------------------
 *
 * SetSummaryInfo --
 *
 *	Initialize the summary information for the domain.  It is well
 *	known that this occupies one sector.
 *
 * Results:
 *	A return code.
 *
 * Side effects:
 *	Fill in the summary info.
 *
 *----------------------------------------------------------------------
 */
void
SetSummaryInfo()
{

    bzero((Address)summaryPtr, DEV_BYTES_PER_SECTOR);

    strcpy(summaryPtr->domainPrefix, "(new domain)");
    /*
     * 4 blocks are already allocated for the root directory.
     */
    summaryPtr->numFreeKbytes = domainPtr->dataBlocks * (FS_BLOCK_SIZE / 1024)
				- 4;
    /*
     * 3 file descriptors are already used, 0 and 1 are reserved and
     * 2 is for the root.
     */
    summaryPtr->numFreeFileDesc = domainPtr->numFileDesc - 3;
    /*
     * The summary state field is unused.
     */
    summaryPtr->state = 0;
    /*
     * The domain number under which this disk partition is mounted is
     * recorded on disk so servers re-attach disks under the same "name".
     * We set it to the special value so it gets a new number assigned
     * when it is first attached.
     */
    summaryPtr->domainNumber = -1;
    /*
     * The flags field is used to record whether or not the disk has been
     * safely "sync"ed to disk upon shutdown.
     */
    summaryPtr->flags = 0;
    summaryPtr->attachSeconds = 0;
    summaryPtr->detachSeconds = 0;
    summaryPtr->fixCount = 0;
}

/*
 *----------------------------------------------------------------------
 *
 * WriteAllFileDescs --
 *
 *	Write out the file descriptor array to disk.
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
WriteAllFileDescs()
{
    int				status;
    char			*bitmap;
    char			*block;
    register Fsdm_FileDescriptor	*fileDescPtr;
    register int		index;

    bitmap = MakeFileDescBitmap();
    if (!printOnly) {
	status = Disk_BlockWrite(partFID, domainPtr,
				domainPtr->fdBitmapOffset,
				domainPtr->fdBitmapBlocks, (Address)bitmap);
	if (status != 0) {
	    fprintf(stderr, "WriteAllFileDescs: Could write fd bitmap\n");
	    exit(1);
	}
    }
    /*
     * Make the first block of file descriptors.  This contains some
     * canned file descriptors for the root, bad block file, and the
     * lost and found directory.  For (early system) testing an empty file
     * can also be created.
     */
    block = (char *)malloc(FS_BLOCK_SIZE);
    bzero(block, FS_BLOCK_SIZE);
    for (index = 0;
         index < FSDM_FILE_DESC_PER_BLOCK;
	 index++ ) {
	fileDescPtr = (Fsdm_FileDescriptor *)((int)block +
					   index * FSDM_MAX_FILE_DESC_SIZE);
	fileDescPtr->magic = FSDM_FD_MAGIC;
	if (index < FSDM_BAD_BLOCK_FILE_NUMBER) {
	    fileDescPtr->flags = FSDM_FD_RESERVED;
	} else if (index == FSDM_BAD_BLOCK_FILE_NUMBER) {
	    InitDesc(fileDescPtr, FS_FILE, 0, -1, -1, 0, 0, 0700, curTime.tv_sec);
	    fileDescPtr->permissions = 0000;
	    fileDescPtr->numLinks = 0;
	} else if (index == FSDM_ROOT_FILE_NUMBER) {
	    InitDesc(fileDescPtr, FS_DIRECTORY, FS_BLOCK_SIZE, -1, -1, 0,
		    0, 0755, curTime.tv_sec);
	    /*
	     * Place the data in the first file system block.
	     */
	    fileDescPtr->direct[0] = 0;
	} else {
	    fileDescPtr->flags = FSDM_FD_FREE;
	}
    }
    if (!printOnly) {
	/*
	 * Write out the first, specially hand crafted, block of file
	 * descriptors.
	 */
	status = Disk_BlockWrite(partFID, domainPtr, 
				 domainPtr->fileDescOffset,
				 1, (Address)block);
	if (status != 0) {
	    fprintf(stderr, "WriteAllFileDescs: Couldn't write descriptor\n");
	    exit(1);
	}
	/*
	 * Redo the block for the remaining file descriptors
	 */
	bzero(block, FS_BLOCK_SIZE);
	for (index = 0;
	     index < FSDM_FILE_DESC_PER_BLOCK;
	     index++ ) {
	    fileDescPtr = (Fsdm_FileDescriptor *)((int)block + index *
					       FSDM_MAX_FILE_DESC_SIZE);
	    fileDescPtr->magic = FSDM_FD_MAGIC;
	    fileDescPtr->flags = FSDM_FD_FREE;
	}
	/*
	 * Write out the remaining file descriptors.
	 */
	for (index = FSDM_FILE_DESC_PER_BLOCK;
	     index < domainPtr->numFileDesc;
	     index += FSDM_FILE_DESC_PER_BLOCK) {
	    status = Disk_BlockWrite(partFID, domainPtr,
		     domainPtr->fileDescOffset + (index/FSDM_FILE_DESC_PER_BLOCK),
		     1, (Address)block);
	    if (status != 0) {
		fprintf(stderr, 
			"WriteAllFileDescs: Couldn't write descriptor (2)\n");
		exit(1);
	    }
	}
    }
}


/*
 *----------------------------------------------------------------------
 *
 * MakeFileDescBitmap --
 *
 *	Compute out the bitmap for file descriptor array to disk.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
char *
MakeFileDescBitmap()
{
    register char	*bitmap;
    register int	index;

    /*
     * Allocate and initialize the bitmap to all 0"s to mean all free.
     */
    bitmap = (char *)malloc((unsigned) domainPtr->fdBitmapBlocks *
				 FS_BLOCK_SIZE);
    bzero((Address)bitmap, domainPtr->fdBitmapBlocks * FS_BLOCK_SIZE);

    /*
     * Reserve file descriptors 0, 1, and 2.  File number 0 is not used at 
     * all in the filesystem.  File number 1 is for the file with bad blocks.
     * File number 2 (FSDM_ROOT_FILE_NUMBER) is the root directory of the domain.
     *
     * IF THIS CHANGES remember to fix SetSummaryInfo
     */
    bitmap[0] |= 0xe0;

    /*
     * Set the bits in the map at the end that don't correspond to
     * any existing file descriptors.
     */
    index = domainPtr->numFileDesc / BITS_PER_BYTE;
    if (domainPtr->numFileDesc % BITS_PER_BYTE) {
	register int bitIndex;
	/*
	 * Take care the last byte that only has part of its bits set.
	 */
	for (bitIndex = domainPtr->numFileDesc % BITS_PER_BYTE;
	     bitIndex < BITS_PER_BYTE;
	     bitIndex++) {
	    bitmap[index] |= 1 << ((BITS_PER_BYTE - 1) - bitIndex);
	}
	index++;
    }
    for ( ; index < domainPtr->fdBitmapBlocks * FS_BLOCK_SIZE; index++) {
	bitmap[index] = 0xff;
    }

    if (printOnly) {
	Disk_PrintFileDescBitmap(domainPtr, bitmap);
    }
    return(bitmap);
}

/*
 *----------------------------------------------------------------------
 *
 * WriteAndInitDataBitmap --
 *
 *	Write out the bitmap for the data blocks.  This knows that the
 *	first 4K is allocated to the root directory.
 *
 * Results:
 *	A return code from the writes.
 *
 * Side effects:
 *	Write the bitmap.
 *
 *----------------------------------------------------------------------
 */
void
WriteAndInitDataBitmap()
{
    int		status;
    char	*bitmap;
    int		kbytesPerCyl;
    int		bitmapBytesPerCyl;
    int		index;

    bitmap = (char *)malloc((unsigned) domainPtr->bitmapBlocks * FS_BLOCK_SIZE);
    bzero(bitmap, domainPtr->bitmapBlocks * FS_BLOCK_SIZE);
    /*
     * Set the bit corresponding to the 4K used for the root directory.
     *   ________
     *	|0______7|	Bits are numbered like this in a byte.
     *
     * IF THIS CHANGES remember to fix SetSummaryInfo()
     */
    bitmap[0] |= 0xf0;
    /*
     * The bitmap is organized by cylinder.  There are whole number of
     * bytes in the bitmap for each cylinder.  Each bit in the bitmap
     * corresponds to 1 kbyte on the disk.
     */
    kbytesPerCyl = domainPtr->geometry.blocksPerCylinder * DISK_KBYTES_PER_BLOCK;
    bitmapBytesPerCyl = (kbytesPerCyl - 1) / BITS_PER_BYTE + 1;
    if ((kbytesPerCyl % BITS_PER_BYTE) != 0) {
	/*
	 * There are bits in the last byte of the bitmap for each cylinder
	 * that don't have kbytes behind them.  Set those bits here so
	 * the blocks don't get allocated.
	 */
	register int extraBits;
	register int mask;

	extraBits = kbytesPerCyl % BITS_PER_BYTE;
	/*
	 * Set up a mask that has the right part filled with 1"s.
	 */
	mask = 0x0;
	for ( ; extraBits < BITS_PER_BYTE ; extraBits++) {
	    mask |= 1 << ((BITS_PER_BYTE - 1) - extraBits);
	}
	for (index = 0;
	     index < domainPtr->dataBlocks * DISK_KBYTES_PER_BLOCK / BITS_PER_BYTE;
	     index += bitmapBytesPerCyl) {
	    bitmap[index + bitmapBytesPerCyl - 1] |= mask;
	}
    }
    /*
     * Set the bits in the bitmap that correspond to non-existent cylinders;
     * the bitmap is allocated a whole number of blocks on the disk
     * so there are bytes at its end that don't have blocks behind them.
     */
 
    for (index = domainPtr->dataCylinders * bitmapBytesPerCyl;
	 index < domainPtr->bitmapBlocks * FS_BLOCK_SIZE;
	 index++) {
	bitmap[index] = 0xff;
    }
    if (printOnly) {
	Disk_PrintDataBlockBitmap(domainPtr, bitmap);
    } else {
	status = Disk_BlockWrite(partFID, domainPtr, 
				 domainPtr->bitmapOffset,
				 domainPtr->bitmapBlocks, (Address)bitmap);
	if (status != 0) {
	    fprintf(stderr, "WriteAndInitDataBitmap: Couldn't write bitmap\n");
	    exit(status);
	}
    }
}


/*
 *----------------------------------------------------------------------
 *
 * WriteRootDirectory --
 *
 *	Write the data blocks of the root directory.
 *
 * Results:
 *	A return code from the writes.
 *
 * Side effects:
 *	Write the root directory"s data block.
 *
 *----------------------------------------------------------------------
 */
void
WriteRootDirectory()
{
    int		status;
    char	*block;
    Fslcl_DirEntry	*dirEntryPtr;
    int		offset;
    int		i;

    block = (char *)malloc(FS_BLOCK_SIZE);
    CreateDir(block, FSDM_ROOT_FILE_NUMBER, FSDM_ROOT_FILE_NUMBER);

    if (printOnly) {
	printf("Root Directory\n");
	offset = 0;
	dirEntryPtr = (Fslcl_DirEntry *)block;
	Disk_PrintDirEntry(dirEntryPtr);
	offset += dirEntryPtr->recordLength;
	dirEntryPtr = (Fslcl_DirEntry *)((int)block + offset);
	Disk_PrintDirEntry(dirEntryPtr);
    } else {
	/*
	 * This write trounces the data beyond the stuff allocated to
	 * the root directory.  Currently this is ok and is done because
	 * BlockWrite writes whole numbers of filesystem blocks.
	 */
	status = Disk_BlockWrite(partFID, domainPtr,
				 domainPtr->dataOffset, 1, block);
	if (status != 0) {
	    fprintf(stderr, "WriteRootDirectory: Couldn't write directory\n");
	    exit(status);
	}
    }
}

/*
 * The 8 partitions, a through h.
 */
#define	A_PART	0
#define B_PART	1
#define C_PART	2
#define D_PART	3
#define E_PART	4
#define F_PART	5
#define G_PART	6
#define H_PART	7

#define	BUF_SIZE	100

char		buf[BUF_SIZE];
char		fullBuf[BUF_SIZE];


/*
 *----------------------------------------------------------------------
 *
 * ScanDiskTab --
 *
 *	Initialize the disk info struct by looking up this disk type in
 *	the disk table.
 *
 * Results:
 *	A pointer to a disk info struct.
 *
 * Side effects:
 *	Disk info struct malloc'd and initialized.  The disk header will be
 *	written if is successfully set up.
 *
 *----------------------------------------------------------------------
 */
Disk_Info *
ScanDiskTab()
{
    FILE		*fp;
    int			len;
    char		*bufPtr;
    int			fullBufLen;
    Fsdm_DiskPartition	partTable[FSDM_NUM_DISK_PARTS];
    int			i;
    int			sectorsPerTrack;
    int			tracksPerCylinder;
    int			sectorsPerCylinder;
    int			numCylinders;
    Disk_Info		*diskInfoPtr;


    fp = fopen("/etc/disktab", "r");
    if (fp == NULL) {
	perror("/etc/disktab");
	exit(1);
    }
    len = strlen(diskType);
    /*
     * Scan until we reach a line that contains the disk type in it.
     */
    while (fgets(buf, BUF_SIZE, fp) != NULL) {
	if (strncmp(diskType, buf, len) == 0 &&
	    buf[len] == '|') {
	    /*
	     * We found the disk type.
	     */
	    break;
	}
    }
    if (strncmp(diskType, buf, len) != 0) {
	fprintf(stderr, "`%s' not in disktab\n", diskType);
	exit(1);
    }

    fullBufLen = 0;
    /*
     * Now cram all of the lines that end in "\" together.
     */
    while (1) {
	for (bufPtr = buf; *bufPtr != '\n' && *bufPtr != '\\'; bufPtr++) {
	    if (*bufPtr != ' ' && *bufPtr != '\t') {
		fullBuf[fullBufLen] = *bufPtr;
		fullBufLen++;
	    }
	}
	if (*bufPtr == '\n') {
	    fullBuf[fullBufLen] = 0;
	    break;
	}
	if (fgets(buf, BUF_SIZE, fp) == NULL) {
	    fprintf(stderr, "Premature EOF\n");
	    exit(1);
	}
    }
    /*
     * Now build up a partition table.
     */
    for (i = 0; i < FSDM_NUM_DISK_PARTS; i++) {
	partTable[i].firstCylinder = 0;
	partTable[i].numCylinders = 0;
    }
    for (bufPtr = fullBuf; *bufPtr != 0; bufPtr++) {
	int	partition;

	if (strncmp(bufPtr, ":ns#", 4) == 0) {
	    bufPtr += 4;
	    sscanf(bufPtr, "%d", &sectorsPerTrack);
	} else if (strncmp(bufPtr, ":nt#", 4) == 0) {
	    bufPtr += 4;
	    sscanf(bufPtr, "%d", &tracksPerCylinder);
	} else if (strncmp(bufPtr, ":nc#", 4) == 0) {
	    bufPtr += 4;
	    sscanf(bufPtr, "%d", &numCylinders);
	} else if (strncmp(bufPtr, ":p", 2) == 0) {
	    /*
	     * Skip past the ":p".
	     */
	    bufPtr += 2;
	    partition = *bufPtr - 'a';
	    /*
	     * Skip past the partition character and the #.
	     */
	    bufPtr += 2;
	    sscanf(bufPtr, "%d", &partTable[partition].numCylinders);
	}
    }
    /*
     * Now that we've built up the number of cylinders build up the
     * cylinder offsets.
     */
    sectorsPerCylinder = sectorsPerTrack * tracksPerCylinder;
    for (i = 0; i < FSDM_NUM_DISK_PARTS; i++) {
	partTable[i].numCylinders /= sectorsPerCylinder;
    }

    partTable[A_PART].firstCylinder = 0;
    partTable[B_PART].firstCylinder = partTable[A_PART].numCylinders;
    partTable[C_PART].firstCylinder = 0;
    partTable[D_PART].firstCylinder = partTable[B_PART].firstCylinder + 
			partTable[B_PART].numCylinders;
    partTable[E_PART].firstCylinder = partTable[D_PART].firstCylinder + 
			partTable[D_PART].numCylinders;
    partTable[F_PART].firstCylinder = partTable[E_PART].firstCylinder + 
			partTable[E_PART].numCylinders;
    partTable[F_PART].numCylinders = 
			numCylinders - (partTable[E_PART].firstCylinder +
					partTable[E_PART].numCylinders);
    partTable[G_PART].firstCylinder = partTable[B_PART].firstCylinder + 
			partTable[B_PART].numCylinders;
    partTable[G_PART].numCylinders = 
			numCylinders - (partTable[B_PART].firstCylinder +
					partTable[B_PART].numCylinders);

    /*
     * Print out the partition table.
     */
    printf("Sectors-per-track:	%d\n", sectorsPerTrack);
    printf("Tracks-per-cylinder:	%d\n", tracksPerCylinder);
    printf("Sectors-per-cylinder:	%d\n", sectorsPerCylinder);
    printf("Num-cylinders:		%d\n\n", numCylinders);
    printf("Partition	First-Cylinder		Num-Cylinders\n");
    for (i = 0; i < FSDM_NUM_DISK_PARTS; i++) {
	printf("%c		%d			%d\n",
	       'a' + i, partTable[i].firstCylinder, 
	       partTable[i].numCylinders);
    }

    /*
     * Set up a disk header and write it to sector 0.
     */

    if (!printOnly) {
	int		*headerPtr;
	Fsdm_DiskHeader	header;
	int		checkSum;
	int		status;

	bzero(&header, sizeof(header));
	strcpy(header.asciiLabel, diskType);
	header.magic = FSDM_DISK_MAGIC;
	header.numCylinders = numCylinders;
	header.numAltCylinders = 0;
	header.numHeads = tracksPerCylinder;
	header.numSectors = sectorsPerTrack;
	header.bootSector = 1;
	header.numBootSectors = 15;
	header.summarySector = 17;
	header.domainSector = 18;
	header.numDomainSectors = FSDM_NUM_DOMAIN_SECTORS;
	header.partition = partition;
	bcopy(partTable, header.map, sizeof(header.map));
	/*
	 * Compute the checksum.
	 */
	header.checkSum = FSDM_DISK_MAGIC;
	checkSum = 0;
	for (i = 0, headerPtr = (int *)&header; 
	     i < DEV_BYTES_PER_SECTOR; 
	     i += sizeof(int), headerPtr++) {
	    checkSum ^= *headerPtr;
	}
	header.checkSum = checkSum;
	/*
	 * Recompute the checksum and make sure it matches.
	 */
	checkSum = 0;
	for (i = 0, headerPtr = (int *)&header; 
	     i < DEV_BYTES_PER_SECTOR; 
	     i += sizeof(int), headerPtr++) {
	    checkSum ^= *headerPtr;
	}
	if (checkSum != FSDM_DISK_MAGIC) {
	    fprintf(stderr, "Bad checksum\n");
	    exit(1);
	}

	/*
	 * Write out the disk header.  Unless this is a sun.
	 * On suns the disk label is a special format so the
	 * prom can read it.  So we don't mess with it.
	 */

#if !defined(sun) && !defined(sun3) && !defined(sun4)
	status = Disk_SectorWrite(partFID, 0, 1, (Address)&header);
	if (status != 0) {
	    perror("Couldn't write out the disk header");
	    exit(1);
	}
#endif

    }


    /*
     * Allocate, initialize and return the disk info struct.
     */
    diskInfoPtr = (Disk_Info *)malloc(sizeof(Disk_Info));
    (void)strcpy(diskInfoPtr->asciiLabel, diskType);
    diskInfoPtr->bootSector = 1;
    diskInfoPtr->numBootSectors = 15;
    diskInfoPtr->summarySector = 17;
    diskInfoPtr->domainSector = 18;
    diskInfoPtr->numDomainSectors = FSDM_NUM_DOMAIN_SECTORS;
    diskInfoPtr->firstCylinder = partTable[partition].firstCylinder;
    diskInfoPtr->numCylinders = partTable[partition].numCylinders;
    diskInfoPtr->numHeads = tracksPerCylinder;
    diskInfoPtr->numSectors = sectorsPerTrack;

    return(diskInfoPtr);
}


/*
 *----------------------------------------------------------------------
 *
 * ReadFileDescBitmap --
 *
 *	Read in the file descriptor bitmap.
 *
 * Results:
 *	A pointer to the file descriptor bit map.
 *
 * Side effects:
 *	Memory allocated for the bit map.
 *
 *----------------------------------------------------------------------
 */
unsigned char *
ReadFileDescBitmap()
{
    register unsigned char *bitmap;

    /*
     * Allocate the bitmap.
     */
    bitmap = (unsigned char *)malloc(domainPtr->fdBitmapBlocks * FS_BLOCK_SIZE);
    if (Disk_BlockRead(partFID, domainPtr, domainPtr->fdBitmapOffset,
		  domainPtr->fdBitmapBlocks, (Address)bitmap) < 0) {
	fprintf(stderr, "ReadFileDescBitmap: Read failed");
	exit(1);
    }
    return(bitmap);
}


/*
 *----------------------------------------------------------------------
 *
 * WriteFileDescBitmap --
 *
 *	Write out the file descriptor bitmap.
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
WriteFileDescBitmap(bitmap)
    register unsigned char 	*bitmap;	/* Bitmap to write. */
{
    if (Disk_BlockWrite(partFID, domainPtr, domainPtr->fdBitmapOffset,
		   domainPtr->fdBitmapBlocks, (Address)bitmap) < 0) {
	fprintf(stderr, "WriteFileDescBitmap: Write failed");
	exit(1);
    }
}


/*
 *----------------------------------------------------------------------
 *
 * ReadBitmap --
 *
 *	Read the bitmap off disk.
 *
 * Results:
 *	A pointer to the bitmap.
 *
 * Side effects:
 *	Memory allocated for the bit map.
 *
 *----------------------------------------------------------------------
 */
unsigned char *
ReadBitmap()
{
    unsigned char *bitmap;

    bitmap = (unsigned char *)malloc(domainPtr->bitmapBlocks * FS_BLOCK_SIZE);
    if (Disk_BlockRead(partFID, domainPtr, domainPtr->bitmapOffset,
		  domainPtr->bitmapBlocks, (Address) bitmap) < 0) {
	fprintf(stderr, "ReadBitmap: Read failed");
	exit(1);
    }
    return(bitmap);
}


/*
 *----------------------------------------------------------------------
 *
 * WriteBitmap --
 *
 *	Write the bitmap to disk.
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
WriteBitmap(bitmap)
    unsigned char		*bitmap;	/* Bitmap to write. */
{
    if (Disk_BlockWrite(partFID, domainPtr, domainPtr->bitmapOffset,
		   domainPtr->bitmapBlocks, (Address) bitmap) < 0) {
	fprintf(stderr, "WriteBitmap: Write failed");
	exit(1);
    }
}


/*
 *----------------------------------------------------------------------
 *
 * ReadFileDesc --
 *
 *	Return the given file descriptor.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The file descriptor struct is filled in.
 *
 *----------------------------------------------------------------------
 */
void
ReadFileDesc(fdNum, fdPtr)
    int			fdNum;
    Fsdm_FileDescriptor	*fdPtr;
{
    static char		block[FS_BLOCK_SIZE];
    int			blockNum;
    int			offset;

    blockNum = domainPtr->fileDescOffset + fdNum / FSDM_FILE_DESC_PER_BLOCK;
    offset = (fdNum & (FSDM_FILE_DESC_PER_BLOCK - 1)) * FSDM_MAX_FILE_DESC_SIZE;
    if (Disk_BlockRead(partFID, domainPtr, blockNum, 1, 
		       (Address) block) < 0) {
	fprintf(stderr, "ReadFileDesc: Read failed\n");
	exit(1);
    }
    bcopy((Address)&block[offset], (Address)fdPtr, sizeof(Fsdm_FileDescriptor));
}


/*
 *----------------------------------------------------------------------
 *
 * WriteFileDesc --
 *
 *	Return the given file descriptor.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The file descriptor struct is filled in.
 *
 *----------------------------------------------------------------------
 */
void
WriteFileDesc(fdNum, fdPtr)
    int			fdNum;
    Fsdm_FileDescriptor	*fdPtr;
{
    static char		block[FS_BLOCK_SIZE];
    int			blockNum;
    int			offset;

    blockNum = domainPtr->fileDescOffset + fdNum / FSDM_FILE_DESC_PER_BLOCK;
    offset = (fdNum & (FSDM_FILE_DESC_PER_BLOCK - 1)) * FSDM_MAX_FILE_DESC_SIZE;
    if (Disk_BlockRead(partFID, domainPtr, blockNum, 1, 
		       (Address) block) < 0) {
	fprintf(stderr, "WriteFileDesc: Read failed\n");
	exit(1);
    }
    bcopy(fdPtr, (Address)&block[offset], sizeof(Fsdm_FileDescriptor));
    if (Disk_BlockWrite(partFID, domainPtr, blockNum, 1, 
		       (Address) block) < 0) {
	fprintf(stderr, "WriteFileDesc: Write failed\n");
	exit(1);
    }
}

char	fileBlock[FS_BLOCK_SIZE];
char	indirectBlock[FS_BLOCK_SIZE];
int	*indIndexPtr = (int *)indirectBlock;


/*
 *----------------------------------------------------------------------
 *
 * CopyTree --
 *
 *	Copy the tree of files in the given directory
 *	the disk table.
 *
 * Results:
 *	A pointer to a disk info struct.
 *
 * Side effects:
 *	Disk info struct malloc'd and initialized.  The disk header will be
 *	written if is successfully set up.
 *
 *----------------------------------------------------------------------
 */
void
CopyTree(dirName, dirFDNum, dirFDPtr, parentFDNum, createDir, path)
    char		*dirName;	/* Name of directory to copy. */
    int			dirFDNum;	/* File number of directory. */
    Fsdm_FileDescriptor	*dirFDPtr;	/* File descriptor of directory. */
    int			parentFDNum;	/* File number of parent. */
    Boolean		createDir;	/* Should create the directory. */
    char		*path;
{
    DIR			*unixDirPtr;
    Fslcl_DirEntry		*unixDirEntPtr;
    DirIndexInfo	indexInfo;
    Fslcl_DirEntry		*spriteDirEntPtr;
    char		fileName[FS_MAX_NAME_LENGTH + 1];
    int			newFDNum;
    Fsdm_FileDescriptor	newFD;
    Fsdm_FileDescriptor	*newFDPtr;
    struct	stat	statBuf;
    int			followLinks;
    char		pathName[1024];

    /*
     * Get our absolute path name so we can get back if we follow a
     * symbolic link.
     */
    getwd(pathName);

    if (chdir(dirName) < 0) {
	perror(dirName);
	exit(1);
    }

    /*
     * Get a pointer to the UNIX directory.
     */
    unixDirPtr = opendir(".");
    if (unixDirPtr == NULL) {
	fprintf(stderr, "Can't open directory %s\n", dirName);
	exit(1);
    }
    /*
     * Open the Sprite directory.
     */
    spriteDirEntPtr = OpenDir(dirFDPtr, &indexInfo);
    if (spriteDirEntPtr == (Fslcl_DirEntry *)NULL) {
	if (chdir(pathName) < 0) {
	    perror(pathName);
	    exit(1);
	}
    }
    /*
     * See if there is a "follow.links" file in this directory.  If so
     * we are supposed to follow symbolic links rather than just copying
     * the links.
     */
    if (stat("follow.links", &statBuf) < 0) {
	followLinks = 0;
    } else {
	printf("Following links ...\n");
	followLinks = 1;
    }
    if (createDir) {
	CreateDir(indexInfo.dirBlock, dirFDNum, parentFDNum);
    }

    for (unixDirEntPtr = (Fslcl_DirEntry *)readdir(unixDirPtr);
	   unixDirEntPtr != NULL;
	   unixDirEntPtr = (Fslcl_DirEntry *)readdir(unixDirPtr)) {

	if (unixDirEntPtr->nameLength == 1 && 
	    strncmp(unixDirEntPtr->fileName, ".", 1) == 0) {
	    continue;
	}
	if (unixDirEntPtr->nameLength == 2 && 
	    strncmp(unixDirEntPtr->fileName, "..", 2) == 0) {
	    continue;
	}
	strncpy(fileName, unixDirEntPtr->fileName, unixDirEntPtr->nameLength);
	fileName[unixDirEntPtr->nameLength] = 0;
	if (followLinks) {
	    if (stat(fileName, &statBuf) < 0) {
		perror(fileName);
		exit(1);
	    }
	} else {
	    if (lstat(fileName, &statBuf) < 0) {
		perror(fileName);
		exit(1);
	    }
	}

	newFDNum = freeFDNum;
	freeFDNum++;
	MarkFDBitmap(newFDNum, fdBitmapPtr);
	summaryPtr->numFreeFileDesc--;
	/*
	 * Read out the file descriptor being careful to save it around
	 * if we found the /dev descriptor.
	 */
	if ((strcmp(fileName, "dev") == 0) &&
	    (dirFDNum == FSDM_ROOT_FILE_NUMBER)) {
	    if (!(statBuf.st_mode & S_GFDIR)) {
		fprintf(stderr, "dev isn't a directory\n");
		exit(1);
	    }
	    ReadFileDesc(newFDNum, &devFD);
	    newFDPtr = &devFD;
	    devFDNum = newFDNum;
	    devFDPtr = newFDPtr;
	} else {
	    ReadFileDesc(newFDNum, &newFD);
	    newFDPtr = &newFD;
	}
	spriteDirEntPtr = AddToDirectory(&indexInfo, spriteDirEntPtr,
					 newFDNum, fileName);
	if (statBuf.st_mode & S_GFDIR) {
	    char	newPath[FS_MAX_NAME_LENGTH];

	    /*
	     * Increment the current directories link count because once
	     * the child gets created it will point to the parent.
	     */
	    dirFDPtr->numLinks++;
	    /*
	     * Allocate the currently free file descriptor to this directory.
	     */
	    InitDesc(newFDPtr, FS_DIRECTORY, FS_BLOCK_SIZE, -1, -1, 
		     0, 0, statBuf.st_mode & 07777, statBuf.st_mtime);
	    /*
	     * Give the directory one full block.  The directory will
	     * be initialized by CopyTree when we call it recursively.
	     */
	    newFDPtr->direct[0] = freeBlockNum * FS_FRAGMENTS_PER_BLOCK;
	    MarkDataBitmap(domainPtr, cylBitmapPtr, freeBlockNum,
			   FS_FRAGMENTS_PER_BLOCK);
	    freeBlockNum++;
	    sprintf(newPath, "%s%s/", path, fileName);
	    printf("Directory: %s\n", newPath);
	    CopyTree(fileName, newFDNum, newFDPtr, dirFDNum, TRUE, newPath);
	} else if ((statBuf.st_mode & S_GFMT) == S_GFREG ||
		   (statBuf.st_mode & S_GFMT) == S_GFLNK) {
	    int	fd;
	    int	blockNum;
	    int	toRead;
	    int	len;

	    blockNum = 0;
	    if ((statBuf.st_mode & S_GFMT) == S_GFREG) {
		printf("File: %s%s\n", path, fileName);
		InitDesc(newFDPtr, FS_FILE, statBuf.st_size, -1, -1,
			 0, 0, statBuf.st_mode & 07777, statBuf.st_mtime);
		/*
		 * Copy the file over.
		 */
		fd = open(fileName, 0);
		if (fd < 0) {
		    perror(fileName);
		    exit(1);
		}
		len = read(fd, fileBlock, FS_BLOCK_SIZE);
		if (len < 0) {
		    perror(fileName);
		    exit(1);
		}
		toRead = statBuf.st_size;
	    } else {
		len = readlink(fileName, fileBlock, FS_BLOCK_SIZE);
		if (len < 0) {
		    perror(fileName);
		    exit(1);
		}
		fileBlock[len] = '\0';
		InitDesc(newFDPtr, FS_SYMBOLIC_LINK, len + 1, -1, -1, 
			 0, 0, 0777, statBuf.st_mtime);
		printf("Symbolic link: %s%s -> %s\n", 
			path, fileName, fileBlock);
		toRead = len + 1;
	    }

	    while (len > 0) {
		if (blockNum == FSDM_NUM_DIRECT_BLOCKS) {
		    int	i;
		    int	*intPtr;
		    /*
		     * Must allocate an indirect block.
		     */
		    newFDPtr->indirect[0] =
			VirtToPhys(freeBlockNum * FS_FRAGMENTS_PER_BLOCK);
		    MarkDataBitmap(domainPtr, cylBitmapPtr, freeBlockNum,
				   FS_FRAGMENTS_PER_BLOCK);
		    freeBlockNum++;
		    summaryPtr->numFreeKbytes -= FS_FRAGMENTS_PER_BLOCK;
		    for (i = 0, intPtr = (int *)indirectBlock; 
			 i < FS_BLOCK_SIZE / sizeof(int); 
			 i++, intPtr++) {
			 *intPtr = FSDM_NIL_INDEX;
		    }
		}
		if (blockNum >= FSDM_NUM_DIRECT_BLOCKS) {
		    indIndexPtr[blockNum - FSDM_NUM_DIRECT_BLOCKS] = 
					freeBlockNum * FS_FRAGMENTS_PER_BLOCK;
		    MarkDataBitmap(domainPtr, cylBitmapPtr, freeBlockNum,
				   FS_FRAGMENTS_PER_BLOCK);
		    summaryPtr->numFreeKbytes -= FS_FRAGMENTS_PER_BLOCK;
		} else {
		    newFDPtr->direct[blockNum] = 
					freeBlockNum * FS_FRAGMENTS_PER_BLOCK;
		    if (toRead > FS_BLOCK_SIZE) {
			MarkDataBitmap(domainPtr, cylBitmapPtr, freeBlockNum,
				       FS_FRAGMENTS_PER_BLOCK);
			summaryPtr->numFreeKbytes -= FS_FRAGMENTS_PER_BLOCK;
		    } else {
			MarkDataBitmap(domainPtr, cylBitmapPtr, freeBlockNum,
				       (toRead - 1) / FS_FRAGMENT_SIZE + 1);
			summaryPtr->numFreeKbytes -= 
					(toRead - 1) / FS_FRAGMENT_SIZE + 1;
		    }
		}
		/*
		 * Write the block out to disk.
		 */
		if (Disk_BlockWrite(partFID, domainPtr, 
				    domainPtr->dataOffset + freeBlockNum,
				    1, (Address)fileBlock) != 0) {
		    fprintf(stderr, "Couldn't write file block\n");
		    exit(1);
		}
		blockNum++;
		freeBlockNum++;
		if ((statBuf.st_mode & S_GFMT) == S_GFLNK) {
		    break;
		}
		toRead -= len;
		len = read(fd, fileBlock, FS_BLOCK_SIZE);
		if (len < 0) {
		    perror(fileName);
		    exit(1);
		}
	    }
	    if (newFDPtr->indirect[0] != FSDM_NIL_INDEX) {
		if (Disk_BlockWrite(partFID, domainPtr,
			    newFDPtr->indirect[0] / FS_FRAGMENTS_PER_BLOCK,
			    1, (Address)indirectBlock) != 0) {
		    fprintf(stderr, "Couldn't write indirect block\n");
		    exit(1);
		}
	    }
	    close(fd);
	} else {
	    fprintf(stderr, "Non file or directory\n");
	    exit(1);
	}
	WriteFileDesc(newFDNum, newFDPtr);

	if (spriteDirEntPtr == (Fslcl_DirEntry *)NULL) {
	    fprintf(stderr, "%s is full\n", dirName);
	    break;
	}
    }

    CloseDir(&indexInfo);

    if (chdir(pathName) < 0) {
	perror(pathName);
	exit(1);
    }
    closedir(unixDirPtr);
    return;
}


/*
 *----------------------------------------------------------------------
 *
 * OpenDir --
 *
 *	Set up the structure to allow moving through the given directory.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The index structure is set up and *dirEntryPtrPtr set to point to
 *	the first directory entry.
 *
 *----------------------------------------------------------------------
 */
static Fslcl_DirEntry *
OpenDir(fdPtr, indexInfoPtr)
    Fsdm_FileDescriptor	*fdPtr;		/* The file descriptor for the
					 * directory. */
    DirIndexInfo 	*indexInfoPtr;	/* Index info struct */
{
    int			fragsToRead;

    if (fdPtr->lastByte == -1) {
	/*
	 * Empty directory.
	 */
	return((Fslcl_DirEntry *) NULL);
    } else if ((fdPtr->lastByte + 1) % FSLCL_DIR_BLOCK_SIZE != 0) {
	fprintf(stderr, "Directory not multiple of directory block size.\n");
	exit(1);
    } else if (fdPtr->fileType != FS_DIRECTORY) {
	fprintf(stderr, "OpenDir: Not a directory\n");
	return((Fslcl_DirEntry *)NULL);
    }

    /*
     * Initialize the index structure.
     */
    indexInfoPtr->fdPtr = fdPtr;
    indexInfoPtr->blockNum = 0;
    indexInfoPtr->blockAddr = fdPtr->direct[0] / FS_FRAGMENTS_PER_BLOCK + 
			      domainPtr->dataOffset;
    /*
     * Read in the directory block.
     */
    if (fdPtr->lastByte != FS_BLOCK_SIZE - 1) {
	fprintf(stderr, "We created a directory that's not 4K?\n");
	exit(1);
    }
    if (Disk_BlockRead(partFID, domainPtr,
		       indexInfoPtr->blockAddr,
		       1, indexInfoPtr->dirBlock) < 0) {
	fprintf(stderr, "OpenDir: Read failed block %d\n",
			indexInfoPtr->blockAddr);
	exit(1);
    } 
    indexInfoPtr->dirOffset = 0;
    return((Fslcl_DirEntry *) indexInfoPtr->dirBlock);
}


/*
 *----------------------------------------------------------------------
 *
 * NextDirEntry --
 *
 *	Return a pointer to the next directory entry.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The index structure is modified and *dirEntryPtrPtr set to point
 *	to the next directory entry.
 *
 *----------------------------------------------------------------------
 */
Fslcl_DirEntry *
NextDirEntry(indexInfoPtr, dirEntryPtr)
    DirIndexInfo 	*indexInfoPtr;
    Fslcl_DirEntry		*dirEntryPtr;
{
    indexInfoPtr->dirOffset += dirEntryPtr->recordLength;
    if (indexInfoPtr->dirOffset < FS_BLOCK_SIZE) {
	/*
	 * The next directory entry is in the current block.
	 */
	return((Fslcl_DirEntry *)
			&(indexInfoPtr->dirBlock[indexInfoPtr->dirOffset]));
    } else {
	Fsdm_FileDescriptor	*fdPtr;
	int			i;

	printf("Adding new block to directory ...\n");

	/*
	 * Write out the current block and set up the next one.
	 */
	if (!printOnly) {
	    if (Disk_BlockWrite(partFID, domainPtr, indexInfoPtr->blockAddr,
				1, indexInfoPtr->dirBlock) < 0) {
		fprintf(stderr, "NextDirEntry: Write failed block %d\n",
				indexInfoPtr->blockAddr);
		exit(1);
	    }
	}
	fdPtr = indexInfoPtr->fdPtr;
	fdPtr->lastByte += FS_BLOCK_SIZE;
	fdPtr->numKbytes += FS_FRAGMENTS_PER_BLOCK;
	indexInfoPtr->blockNum++;
	fdPtr->direct[indexInfoPtr->blockNum] = 
				freeBlockNum * FS_FRAGMENTS_PER_BLOCK;
	MarkDataBitmap(domainPtr, cylBitmapPtr, freeBlockNum,
		       FS_FRAGMENTS_PER_BLOCK);
	indexInfoPtr->blockAddr = freeBlockNum + domainPtr->dataOffset;
	freeBlockNum++;
	for (i = 0, dirEntryPtr = (Fslcl_DirEntry *)indexInfoPtr->dirBlock; 
	     i < FS_BLOCK_SIZE / FSLCL_DIR_BLOCK_SIZE;
     i++,dirEntryPtr=(Fslcl_DirEntry *)((unsigned)dirEntryPtr+FSLCL_DIR_BLOCK_SIZE)) {
	    dirEntryPtr->fileNumber = 0;
	    dirEntryPtr->recordLength = FSLCL_DIR_BLOCK_SIZE;
	    dirEntryPtr->nameLength = 0;
	}
	indexInfoPtr->dirOffset = 0;
	return((Fslcl_DirEntry *) indexInfoPtr->dirBlock);
    }
}


/*
 *----------------------------------------------------------------------
 *
 * CloseDir --
 *
 *	Flushes the current directory block to disk, if necessary.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The index structure is modified and *dirEntryPtrPtr set to point
 *	to the next directory entry.
 *
 *----------------------------------------------------------------------
 */
static void
CloseDir(indexInfoPtr)
    DirIndexInfo 	*indexInfoPtr;
{

    if (!printOnly) {
	if (Disk_BlockWrite(partFID, domainPtr, indexInfoPtr->blockAddr,
			    1, indexInfoPtr->dirBlock) < 0) {
	    fprintf(stderr, "CloseDir: Write (2) failed block %d\n",
			    indexInfoPtr->blockAddr);
	    exit(1);
	}
    }
}


/*
 *----------------------------------------------------------------------
 *
 * InitDesc --
 *
 *	Set up a file descriptor as allocated.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	File descriptor fields filled in.
 *
 *----------------------------------------------------------------------
 */
void
InitDesc(fileDescPtr, fileType, numBytes, devType, devUnit, uid,
	gid, permissions, time)
    Fsdm_FileDescriptor	*fileDescPtr;
    int			fileType;
    int			numBytes;
    int			devType;
    int			devUnit;
    int			uid;
    int			gid;
    int			permissions;
    int			time;
{
    int		index;

    fileDescPtr->flags = FSDM_FD_ALLOC;
    fileDescPtr->fileType = fileType;
    fileDescPtr->permissions = permissions;
    fileDescPtr->uid = uid;
    fileDescPtr->gid = gid;
    fileDescPtr->lastByte = numBytes - 1;
    fileDescPtr->firstByte = -1;
    if (fileType == FS_DIRECTORY) {
	fileDescPtr->numLinks = 2;
    } else {
	fileDescPtr->numLinks = 1;
    }
    /*
     * Can't know device information because that depends on
     * the way the system is configured.
     */
    fileDescPtr->devServerID = -1;
    fileDescPtr->devType = devType;
    fileDescPtr->devUnit = devUnit;

    /*
     * Set the time stamps.  This assumes that universal time,
     * not local time, is used for time stamps.
     */
    fileDescPtr->createTime = time;
    fileDescPtr->accessTime = 0;
    fileDescPtr->descModifyTime = time;
    fileDescPtr->dataModifyTime = time;

    /*
     * Place the data in the first filesystem block.
     */
    for (index = 0; index < FSDM_NUM_DIRECT_BLOCKS ; index++) {
	fileDescPtr->direct[index] = FSDM_NIL_INDEX;
    }
    for (index = 0; index < FSDM_NUM_INDIRECT_BLOCKS ; index++) {
	fileDescPtr->indirect[index] = FSDM_NIL_INDEX;
    }
    if (numBytes > 0) {
	int	numBlocks;

	numBlocks = (numBytes - 1) / FS_BLOCK_SIZE + 1;
	if (numBlocks > FSDM_NUM_DIRECT_BLOCKS) {
	    fileDescPtr->numKbytes = (numBlocks + 1) * (FS_BLOCK_SIZE / 1024);
	} else {
	    fileDescPtr->numKbytes = (numBytes + 1023) / 1024;
	}
    } else {
	fileDescPtr->numKbytes = 0;
    }

    fileDescPtr->version = 1;
}

int fragMasks[FS_FRAGMENTS_PER_BLOCK + 1] = {0x0, 0x08, 0x0c, 0x0e, 0x0f};


/*
 *----------------------------------------------------------------------
 *
 * MarkDataBitmap --
 *
 *	Mark the appropriate bits in the data block bitmap.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Data block marked.
 *
 *----------------------------------------------------------------------
 */
void
MarkDataBitmap(domainPtr, cylBitmapPtr, blockNum, numFrags)
    Fsdm_DomainHeader	*domainPtr;
    unsigned char	*cylBitmapPtr;
    int			blockNum;
    int			numFrags;
{
    unsigned char	*bitmapPtr;

    bitmapPtr = GetBitmapPtr(domainPtr, cylBitmapPtr, blockNum);
    if ((blockNum % domainPtr->geometry.blocksPerCylinder) & 0x1) {
	*bitmapPtr |= fragMasks[numFrags];
    } else {
	*bitmapPtr |= fragMasks[numFrags] << 4;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * CreateDir --
 *
 *	Create a directory out of a single file system block.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	File system block set up as a directory.
 *
 *----------------------------------------------------------------------
 */
void
CreateDir(block, dot, dotDot)
    Address	block;		/* Block to create directory in. */
    int		dot;		/* File number of directory. */
    int		dotDot;		/* File number of parent. */
{
    Fslcl_DirEntry	*dirEntryPtr;
    char	*fileName;
    int		offset;
    int		length;
    int		i;

    dirEntryPtr = (Fslcl_DirEntry *)block;
    fileName = ".";
    length = strlen(fileName);
    dirEntryPtr->fileNumber = dot;
    dirEntryPtr->recordLength = Fslcl_DirRecLength(length);
    dirEntryPtr->nameLength = length;
    strcpy(dirEntryPtr->fileName, fileName);
    offset = dirEntryPtr->recordLength;

    dirEntryPtr = (Fslcl_DirEntry *)((int)block + offset);
    fileName = "..";
    length = strlen(fileName);
    dirEntryPtr->fileNumber = dotDot;
    dirEntryPtr->recordLength = FSLCL_DIR_BLOCK_SIZE - offset;
    dirEntryPtr->nameLength = length;
    strcpy(dirEntryPtr->fileName, fileName);
    /*
     * Fill out the rest of the directory with empty blocks.
     */
    for (dirEntryPtr = (Fslcl_DirEntry *)&block[FSLCL_DIR_BLOCK_SIZE], i = 1; 
	 i < FS_BLOCK_SIZE / FSLCL_DIR_BLOCK_SIZE;
	 i++,dirEntryPtr=(Fslcl_DirEntry *)((int)dirEntryPtr + FSLCL_DIR_BLOCK_SIZE)) {
	 dirEntryPtr->fileNumber = 0;
	 dirEntryPtr->recordLength = FSLCL_DIR_BLOCK_SIZE;
	 dirEntryPtr->nameLength = 0;
    }
}


/*
 *----------------------------------------------------------------------
 *
 * AddToDirectory --
 *
 *	Add the file descriptor to a directory.  
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The directory is modified to contain the orphaned file.
 *
 *----------------------------------------------------------------------
 */
Fslcl_DirEntry *
AddToDirectory(dirIndexPtr, dirEntryPtr, fileNumber, fileName)
    DirIndexInfo	*dirIndexPtr;
    Fslcl_DirEntry		*dirEntryPtr;
    int		 	fileNumber;
    char 		*fileName;
{
    int		 	nameLength;
    int		 	recordLength;
    int		 	leftOver;
    int			oldRecLength;

    nameLength = strlen(fileName);
    recordLength = Fslcl_DirRecLength(nameLength);

    while (dirEntryPtr != (Fslcl_DirEntry *) NULL) {
	if (dirEntryPtr->fileNumber != 0) {
	    oldRecLength = Fslcl_DirRecLength(dirEntryPtr->nameLength);
	    leftOver = dirEntryPtr->recordLength - oldRecLength;
	    if (leftOver >= recordLength) {
		dirEntryPtr->recordLength = oldRecLength;
		dirEntryPtr = 
			(Fslcl_DirEntry *) ((int) dirEntryPtr + oldRecLength);
		dirEntryPtr->recordLength = leftOver;
		dirIndexPtr->dirOffset += oldRecLength;
	    } else {
		dirEntryPtr = NextDirEntry(dirIndexPtr, dirEntryPtr);
		continue;
	    }
	} else if (dirEntryPtr->recordLength < recordLength) {
	    dirEntryPtr = NextDirEntry(dirIndexPtr, dirEntryPtr);
	    continue;
	}

	dirEntryPtr->fileNumber = fileNumber;
	dirEntryPtr->nameLength = nameLength;
	(void)strcpy(dirEntryPtr->fileName, fileName);
	leftOver = dirEntryPtr->recordLength - recordLength;
	if (leftOver > FSLCL_DIR_ENTRY_HEADER) {
	    dirEntryPtr->recordLength = recordLength;
	    dirEntryPtr =(Fslcl_DirEntry *) ((int) dirEntryPtr + recordLength);
	    dirEntryPtr->fileNumber = 0;
	    dirEntryPtr->recordLength = leftOver;
	    dirIndexPtr->dirOffset += recordLength;
	} else {
	    dirEntryPtr = NextDirEntry(dirIndexPtr, dirEntryPtr);
	}
	return(dirEntryPtr);
    }

    fprintf(stderr, "Directory full.\n");
    exit(1);
}


/*
 *----------------------------------------------------------------------
 *
 * MakeDevices --
 *
 *	Add devices to the dev directory.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Devices added to the dev directory.
 *
 *----------------------------------------------------------------------
 */
void
MakeDevices()
{
    FILE		*fp;
    Fslcl_DirEntry		*dirEntryPtr;
    DirIndexInfo	indexInfo;
    char		fileName[FS_MAX_NAME_LENGTH];
    int			devType;
    int			devUnit;
    int			devPermissions;
    char		buf[FS_MAX_NAME_LENGTH];
    Fsdm_FileDescriptor	fileFD;

    fp = fopen(devFileName, "r");
    if (fp == NULL) {
	perror(devFileName);
	exit(1);
    }

    dirEntryPtr = OpenDir(devFDPtr, &indexInfo);
    if (dirEntryPtr == (Fslcl_DirEntry *)NULL) {
	fprintf(stderr, "MakeDevices: Dev is bogus\n");
	exit(1);
    }
    while (fgets(buf, FS_MAX_NAME_LENGTH, fp) != NULL) {
	if (sscanf(buf, "%s %d %d %o", fileName, &devType, &devUnit,
		&devPermissions) != 4) {
	    continue;
	}
	MarkFDBitmap(freeFDNum, fdBitmapPtr);
	ReadFileDesc(freeFDNum, &fileFD);
	InitDesc(&fileFD, FS_DEVICE, 0, devType, devUnit, 0, 0,
		devPermissions, curTime.tv_sec);
	dirEntryPtr = AddToDirectory(&indexInfo, dirEntryPtr, freeFDNum,
				     fileName);
	if (!printOnly) {
	    WriteFileDesc(freeFDNum, &fileFD);
	}
	printf("Device: %s, %d, %d 0%o\n",
		fileName, devType, devUnit, devPermissions);

	summaryPtr->numFreeFileDesc--;
	freeFDNum++;
	if (dirEntryPtr == (Fslcl_DirEntry *)NULL) {
	    fprintf(stderr, "MakeDevices: dev directory is full\n");
	    break;
	}
    }
    CloseDir(&indexInfo);
}

#if 0
/*
 *----------------------------------------------------------------------
 *
 * CopySuperBlock --
 *
 *	Copy the super block from the first sector of the disk to
 *	the first sector of the partition being formatted.
 *
 * Results:
 *	A return code from the I/O.
 *
 * Side effects:
 *	Writes on the zero'th sector of the partition.
 *
 *----------------------------------------------------------------------
 */
ReturnStatus
CopySuperBlock(firstPartFID, partFID)
    int firstPartFID;
    int partFID;
{
    ReturnStatus status;
    char *block;

    block = (char *)malloc(DEV_BYTES_PER_SECTOR);

    status = Disk_SectorRead(firstPartFID, 0, 1, block);
    if (status != SUCCESS) {
	return(status);
    }
    status = Disk_SectorWrite(partFID, 0, 1, block);
    return(status);
}
#endif
