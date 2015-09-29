/* 
 * fscheck.c --
 *
 *	Perform consistency checks on a filesystem.
  *
 * Copyright 1989 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /sprite/src/cmds/fscheck/RCS/fscheck.c,v 1.38 92/06/09 21:48:59 jhh Exp $ SPRITE (Berkeley)";
#endif not lint

#include "option.h"
#include "list.h"
#include "fscheck.h"
#include <string.h>
#include <host.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <stdio.h>

int	numBlocks = 0;
int	numFiles = 0;
int	numBadDesc = 0;
int	numFrags = 0;
int	errorType = EXIT_OK;
int	foundError = 0;
int	fdBitmapError = 0;
Boolean	tooBig = FALSE;
Ofs_DomainHeader  *domainPtr;
int		partFID;
Boolean patchHeader = FALSE;
Boolean	attached	= FALSE;

/*
 * The following are used to go from a command line like
 * fscheck -dev rsd0 -part b
 * to /dev/rsd0a 	- for the partition that has the disk label
 * and to /dev/rsd0b	- for the partition to format.
 */
char *deviceName;		/* Set to "rsd0" or "rxy1", etc. */
char *partName;			/* Set to "a", "b", "c" ... "g" */
char defaultFirstPartName[] = "a";
char *firstPartName = defaultFirstPartName;
char defaultDevDirectory[] = "/dev/";
char *devDirectory = defaultDevDirectory;
char *outputFileName = NULL;

int hostID = -1;
int writeDisk = 0;
int verbose = 0;
int silent = 0;
int clearDomainNumber = 0;
int recoveryCheck = 0;
int badBlockInit = 0;
int patchRoot = 0;
int rawOutput = FALSE;
int maxHeapSize = -1;
int bufferSize = BUFSIZ;
int heapSize = 0;
int noCopy = FALSE;
int blocksToRead = 1;
int debug = FALSE;
int clearFixCount = FALSE;
int bitmapVerbose = 0;
int numReboot = 4;
int blockToFind = -1;
int fileToPrint = -1;
int dontRecheck = 0;
int setCheckedBit = FALSE;

Option optionArray[] = {
    {OPT_STRING, "dev", (Address)&deviceName,
	"Required: Name of device, eg \"rsd0\" or \"rxy1\""},
    {OPT_STRING, "part", (Address)&partName,
	"Required: Partition ID: (a, b, c, d, e, f, g)"},
    {OPT_STRING, "dir", (Address)&devDirectory,
	"Name of device directory (\"/dev/\")"},
    {OPT_STRING, "initialPart", (Address)&firstPartName,
	"Name of initial partition (\"a\")"},
    {OPT_TRUE, "write", (Address)&writeDisk,
	"Write disk "},
    {OPT_TRUE, "silent", (Address)&silent,
	"Don't say anything unless there's a problem "},
    {OPT_TRUE, "verbose", (Address)&verbose,
	"Output information about differences in bitmaps "},
    {OPT_TRUE, "fixRoot", (Address)&patchRoot,
     "Re-create the missing/corrupt root directory."},
    {OPT_TRUE, "clear", (Address)&clearDomainNumber,
	"Clear the domain number field stored in the summary sector"},
    {OPT_INT, "hostID", (Address)&hostID,
	"Update the host ID in the disk header"},
    {OPT_TRUE, "badBlock", (Address)&badBlockInit,
	"Initialize the bad block file descriptor"},
    {OPT_STRING, "outputFile", (Address)&outputFileName,
	"Name of file in which to store output."},	
    {OPT_TRUE, "rawOutput", (Address) &rawOutput,
	"Bypass the filesystem when writing the output into the file."},
    {OPT_INT, "heapLimit", (Address)&maxHeapSize,
	"Maximum amount of dynamic storage allowed."},
    {OPT_INT, "bufferSize", (Address)&bufferSize,
	"Size of buffer for output file stream."},
    {OPT_TRUE, "delete", (Address)&noCopy,
	"Truncate files containing duplicate blocks."},
    {OPT_TRUE, "debug", (Address)&debug,
	"Print debugging information."},
    {OPT_INT, "readBlock", (Address)&blocksToRead,
	"Number of blocks to read at a time."},
    {OPT_TRUE, "clearFixCount", (Address)&clearFixCount,
	"Clear the count of consecutive disk fixes in summary sector"},
    {OPT_TRUE, "bitmapVerbose", (Address)&bitmapVerbose,
	"Print information about bitmap errors."},
    {OPT_INT, "numReboot", (Address)&numReboot,
	"Number of consecutive runs before returning EXIT_NOREBOOT"},
    {OPT_INT, "block", (Address)&blockToFind,
	"Block to look for"},
    {OPT_INT, "file", (Address)&fileToPrint,
	"File to print"},
    {OPT_TRUE, "cond", (Address)&dontRecheck,
	"Don't check the disk if it has just been checked successfully"},
    {OPT_TRUE, "setCheck", (Address)&setCheckedBit,
	"If the disk is checked and fixed ok set a bit in the summary sector"},
};
int numOptions = sizeof(optionArray) / sizeof(Option);

/*
 * Forward Declarations.
 */
void		CheckFilesystem();
int		RecoveryCheck();

/*
 * The last file that an error message was printed about.
 */
int		lastErrorFD = -1;
int		fixCount = 0;


/*
 *----------------------------------------------------------------------
 *
 * main --
 *
 *	Create the required file names from the command line
 *	arguments.  Then open the first partition on the disk
 *	because it contains the disk label, and open the partition
 *	that is to be checked.
 *
 * Results:
 *
 * Side effects:
 *	File opening.
 *
 *----------------------------------------------------------------------
 */
main(argc, argv)
    int argc;
    char *argv[];
{
    char	firstPartitionName[64];
    char	partitionName[64];
    int		partition;
    u_char  	*streamBuffer;
    int		outputFID;
    char	*timeString;
    struct timeval tp;
    struct timezone tzp;
    int		argsReturned;


    argsReturned = Opt_Parse(argc, argv, optionArray, numOptions, 0);
    if (argsReturned > 1) {
	Opt_PrintUsage(argv[0], optionArray, Opt_Number(optionArray));
	exit(EXIT_BAD_ARG);
    }
    /*
     * Set up stream for output to file, as well as output to stderr.
     */
    if (outputFileName == NULL) {
	outputFile = NULL;
    } else if (!rawOutput) {
	outputFile = fopen(outputFileName,"a+");
	if (outputFile == NULL ) {
	    OutputPerror("fscheck: Can't open output file \"%s\\n", 
		outputFileName);
	    exit(EXIT_HARD_ERROR);
	}
	if (bufferSize != BUFSIZ) {
	    if (setvbuf(outputFile,(char *) NULL,_IOFBF,bufferSize)) {
		Output(stderr,
		       "fscheck: Unable to change output buffer size.\n");
		if (maxHeapSize > 0) {
		    maxHeapSize -= bufferSize;
		}
	    }
	}
    } else {
	if (bufferSize > FSDM_NUM_DIRECT_BLOCKS * FS_BLOCK_SIZE) {
	    Output(stderr,
"Maximum buffer size allowed when using raw output is %d bytes.\n",
	    FSDM_NUM_DIRECT_BLOCKS * FS_BLOCK_SIZE);
	    exit(EXIT_BAD_ARG);
	}
	Alloc(streamBuffer,u_char,bufferSize);
        Stdio_Setup(outputFile,0,1,streamBuffer,bufferSize,NULL,
	    WriteOutputFile, CloseOutputFile,NULL);
    }
    if (rawOutput) {
	Output(stderr,"      \n");
    }
    gettimeofday(&tp,&tzp);
    timeString = asctime(localtime(&tp.tv_sec));
    if (verbose) {
	Output(stderr,"***** Fscheck *****\n");
    }
    if (!silent) {
	Output(stderr,"%s",timeString);
    }
    if (tooBig) {
	Output(stderr,"fscheck: Heap limit too small.\n");
	exit(EXIT_MORE_MEMORY);
    }
    if (deviceName == (char *)0) {
	Output(stderr, "Specify device name with -dev option\n");
	exit(EXIT_BAD_ARG);
    }
    if (partName == (char *)0) {
	Output(stderr, "Specify partition with -part option\n");
	exit(EXIT_BAD_ARG);
    }
    if (blocksToRead <= 0) {
	Output(stderr,"blocksToRead value %d illegal - using 1.\n");
	blocksToRead = 1;
    }
    if (patchRoot && !writeDisk) {
	Output(stderr, 
	     "Sorry but you can't patch the root without writing the disk.\n");
	    exit(EXIT_BAD_ARG);
    }
    if (hostID != -1) {
	patchHeader = TRUE;
    }
    /*
     * Gen up the name of the first partition on the disk,
     * and the name of the partition that needs to be checked.
     */
    (void)strcpy(firstPartitionName, devDirectory);  /* eg. /dev/ */
    (void)strcpy(partitionName, devDirectory);    
    (void)strcat(firstPartitionName, deviceName);	  /* eg. /dev/rxy0 */
    (void)strcat(partitionName, deviceName);    
    (void)strcat(firstPartitionName, firstPartName);  /* eg. /dev/rxy0a */
    (void)strcat(partitionName, partName);		  /* eg. /dev/rxy0b */

    partFID = open(partitionName, writeDisk ? O_RDWR : O_RDONLY);
    if (partFID < 0) {
	OutputPerror("fscheck: Can't open partition to check ");
	exit(EXIT_HARD_ERROR);
    }

    partition = partName[0] - 'a';
    if (partition < 0 || partition > 7) {
	Output(stderr,
         "fscheck: Can't determine partition index from the partition name\n");
	exit(EXIT_BAD_ARG);
    }
    if ((maxHeapSize > 0) && (bufferSize >= maxHeapSize)) {
	Output(stderr,
	       "fscheck: Size of output buffer exceeds maximum heap size.\n");
	exit(EXIT_MORE_MEMORY);
    }
    CheckFilesystem(partFID, partition);

    if (outputFile != NULL) {
	(void)fclose(outputFile);
    }
    (void)close(partFID);
    if (foundError) {
	if (attached) {
	    exit(EXIT_REBOOT);
	}
	if (errorType != EXIT_OK) {
	    exit(errorType);
	}
	exit(EXIT_SOFT_ERROR);
    }
    exit(EXIT_OK);
}

unsigned char *fdBitmapPtr;
unsigned char *cylBitmapPtr;

/*
 * Array to provide the ability to set and extract bits out of a bitmap byte.
 */
unsigned char bitmasks[BITS_PER_BYTE] = {
    0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1
};

void 		ReadDomainHeader();
void		CheckDirTree();
void		CheckBlocks();
unsigned char 	*ReadFileDescBitmap();
unsigned char 	*ReadBitmap();
FdInfo		*ReadFileDesc();
void		WriteDomainHeader();
void		WriteFileDescBitmap();
void		WriteBitmap();
void		WriteFileDesc();
void		WriteSummaryInfo();
void		CheckFDBitmap();
void		SetBadDescBitmap();
void		RelocateFD();

int	num1KBlocks;
int	bytesPerCylinder;

List_Links	copyListHdr;
List_Links	relocListHdr;
List_Links	modListHdr;
#define tempCopyList &tempCopyListHdr;


/*
 *----------------------------------------------------------------------
 *
 * CheckFilesystem --
 *
 *	Perform consistency checks on the file system.
 *
 * Results:
 *	An error code, probably from a read.
 *
 * Side effects:
 *	Error flags may be set
 *
 *----------------------------------------------------------------------
 */
void
CheckFilesystem(partFID, partition)
    int partFID;	/* Handle on the partition of the disk to format */
    int partition;	/* Index of parition that is to be dumped */
{
    Disk_Label			*labelPtr;
    FdInfo	  	  	*descInfoPtr;
    register	FdInfo	  	*tDescInfoPtr;
    unsigned char		*tFdBitmapPtr;
    unsigned	char	  	*newCylBitmapPtr;
    int				i, j, k;
    int				fdNum;
    char			*block;
    Fsdm_FileDescriptor		*fdPtr;
    int				sector;
    unsigned int		validMask;
    int				valid;
    int				salvage;
    unsigned char 		*oldPtr;
    unsigned char 		*newPtr;
    int				blockNum;
    int				bitmapError = 0;
    RelocListElement		*relocElemPtr;
    CopyListElement		*copyElemPtr;
    Boolean			blockModified;
    Fsdm_FileDescriptor		*fdCopyPtr = NULL;
    Boolean			copyUsed;
    Boolean			batchRead;
    char			*blockBuffer;
    Boolean			blockBufferModified;
    int				numFileDescBlocks;
    Boolean			fdMagicOkay;
    int 			status;



    /*
     * Read the copy of the super block at the beginning of the partition
     * to find out basic disk geometry and where to find the domain header.
     */
    labelPtr = Disk_ReadLabel(partFID);
    if (labelPtr == NULL) {
	Output(stderr, "CheckFilesystem: Could not read disk label\n");
	exit(EXIT_READ_FAILURE);
    }
    fflush(stderr);
    fflush(stdout);
    Alloc(blockBuffer, char, blocksToRead * FS_BLOCK_SIZE);
    if (tooBig) {
	Output(stderr,"CheckFileSystem: Heap limit too small.\n");
	exit(EXIT_MORE_MEMORY);
    }
    domainPtr = Disk_ReadDomainHeader(partFID, labelPtr);
    if (domainPtr == NULL) {
	Output(stderr, "CheckFilesystem: Could not read domain header\n");
	exit(EXIT_READ_FAILURE);
    }
    /*
     * See if we need to patch the host ID in the domain header.
     */
    if (patchHeader) {
	int		spriteID;
	struct stat	attrs;

	if (hostID != 0) {
	    spriteID = hostID;
	} else {
	    /*
	     * Determine where the disk is located so we can set the
	     * spriteID in the header correctly.  If the disk device is generic
	     * we use our own hostID, otherwise use the hostID specified
	     * by the device file.
	     */
	    fstat(partFID, &attrs);
	    if (attrs.st_devServerID == FS_LOCALHOST_ID) {
		Proc_GetHostIDs((int *) NULL, &spriteID);
	    } else {
		spriteID = attrs.st_devServerID;
	    }
	}
	if (spriteID != domainPtr->device.serverID) {
	    if (!silent) {
		Output(stderr, "Setting hostID in disk header to 0x%x\n",
			    spriteID);
	    }
	    domainPtr->device.serverID = spriteID;
	} else {
	    if (!silent) {
		Output(stderr, "Leaving hostID as 0x%x\n",spriteID);
	    }
	    patchHeader = FALSE;
	}
    } else if (debug) {
	Output(stderr,"HostID read off disk is 0x%x\n",
	       domainPtr->device.serverID);
    }
    /*
     * Read in the file descriptor bit map and the cylinder bit map.
     */
    fdBitmapPtr = ReadFileDescBitmap(partFID, domainPtr);
    cylBitmapPtr = ReadBitmap(partFID, domainPtr);
    AllocByte(newCylBitmapPtr,unsigned char,domainPtr->bitmapBlocks * 
	FS_BLOCK_SIZE);
    if (tooBig) {
	Output(stderr,"CheckFileSystem: Heap limit too small.\n");
	exit(EXIT_MORE_MEMORY);
    }
    bzero((Address)newCylBitmapPtr, domainPtr->bitmapBlocks * FS_BLOCK_SIZE); 
    /*
     * Allocate the descriptor info array.
     */
    Alloc(descInfoPtr,FdInfo,domainPtr->numFileDesc);
    if (tooBig) {
	Output(stderr,"CheckFileSystem: Heap limit too small.\n");
	exit(EXIT_MORE_MEMORY);
    }
    bzero((Address)descInfoPtr, domainPtr->numFileDesc * sizeof(FdInfo));

    num1KBlocks = domainPtr->dataCylinders * 
	      domainPtr->geometry.blocksPerCylinder * FS_FRAGMENTS_PER_BLOCK;
    bytesPerCylinder = (domainPtr->geometry.blocksPerCylinder + 1) / 2;
    List_Init(modList);
    List_Init(relocList);
    List_Init(copyList);

    /*
     * Check the disk to see if the disk has been checked already.
     */
    if (verbose) {
	    Output(stderr, "Performing recovery check\n");
    }
    status = RecoveryCheck(partFID, labelPtr);
    if (dontRecheck && status) {
	if (!silent) {
	    Output(stderr, 
    "Disk is marked as just checked. I won't bother checking it again.\n");
	}
	FindOutputFile();
	return;
    }
    /*
     * Recreate the file descriptor bit map and check the block counts and 
     * make sure that all blocks are referenced.  Also recreate the data block
     * bitmap.
     */
    if (verbose) {
	Output(stderr, "Checking file descriptors:\n");
    }

    tDescInfoPtr = descInfoPtr;
    tFdBitmapPtr = fdBitmapPtr;
    fdNum = 0;
    numFileDescBlocks = (domainPtr->numFileDesc + FSDM_FILE_DESC_PER_BLOCK - 1) / 
		FSDM_FILE_DESC_PER_BLOCK;
    if (blocksToRead > numFileDescBlocks) {
	blocksToRead = numFileDescBlocks;
    }
    for (i = 0; 
	 (i < numFileDescBlocks) && !tooBig;
	i += blocksToRead) {
	batchRead = TRUE;
	if (Disk_BlockRead(partFID, domainPtr, 
			   domainPtr->fileDescOffset + i,
			   blocksToRead, (Address) blockBuffer) < 0) {
	    batchRead = FALSE;
	    OutputPerror("CheckFilesystem: read of fd's failed");
	}
	block = blockBuffer;
	blockBufferModified = FALSE;
	for (k = 0; k < blocksToRead; k++) {
	    if (batchRead) {
		block = &(blockBuffer[k * FS_BLOCK_SIZE]);
		salvage = 0;
	    } else {
		/*
		 * We couldn't read all of the disk blocks at once, so try
		 * to read them one at a time.
		 */
		if (Disk_BlockRead(partFID, domainPtr, 
				   domainPtr->fileDescOffset + i + k,
				   1, (Address) block) < 0) {
		    /*
		     * Hit a disk error reading the block.  Read it a sector
		     * at a time and try to salvage what we can.
		     */
		    OutputPerror("CheckFilesystem: BlockRead: Read failed");
		    validMask = Disk_BadBlockRead(partFID, domainPtr,
					      domainPtr->fileDescOffset + i + k,
					      (Address) block);
		    SetBadDescBitmap(domainPtr, fdNum, &tFdBitmapPtr);
		    salvage = 1;
		}
	    }
	    if (!salvage) {
		CheckFDBitmap(domainPtr, fdNum, block, &tFdBitmapPtr);
		valid = 1;
	    }
	    blockModified = FALSE;
	    for (sector = 0; 
	         sector < DISK_SECTORS_PER_BLOCK && !tooBig; 
		 sector++) {
		if (salvage) {
		    valid = (validMask & (1 << sector)) != 0;
		    numBadDesc++;
		}
		for (j = 0; 
		     j < FILE_DESC_PER_SECTOR && 
		     fdNum < domainPtr->numFileDesc && !tooBig;
		     j++, fdNum++, tDescInfoPtr++, fdPtr++) {
    
		    int	modified = 0;
		    fdPtr = (Fsdm_FileDescriptor *)	
			    &block[(j + (FILE_DESC_PER_SECTOR * sector)) *
			    FSDM_MAX_FILE_DESC_SIZE];
		     if (fdPtr->magic != FSDM_FD_MAGIC) {
			 if (!silent) {
			     Output(stderr, 
	 "File %d has an invalid magic number <0x%x> and is being reset.\n",
				    fdNum, fdPtr->magic);
			     Output(stderr,
			     "Current block is %d.\n", 
			     i + k + domainPtr->fileDescOffset);
			 }
			 UnmarkFDBitmap(fdNum,fdBitmapPtr);
			 fdMagicOkay = FALSE;
		     } else {
			 fdMagicOkay = TRUE;
		     }
		    /* 
		     * Make a copy of the fd to be used by elements in 
		     * the lists. We have to do this because we can't keep
		     * all of the fd's in memory at once.
		     */
		    if (fdCopyPtr == NULL) { 
			Alloc(fdCopyPtr,Fsdm_FileDescriptor,1);
			if (tooBig) {
			    continue;
			}
		    }
		    bcopy((Address)fdPtr, (Address)fdCopyPtr, 
			  sizeof(Fsdm_FileDescriptor));
		    copyUsed = FALSE;
		    relocElemPtr = NULL;
		    copyElemPtr = NULL;
		    if (salvage) {
			if (valid && fdMagicOkay && 
			    (fdPtr->flags & FSDM_FD_ALLOC)) {
			    /* 
			     * If the descriptor is valid and is in a disk block
			     * that is bad, put the fd on a list for relocation.
			     */
			    Alloc(relocElemPtr,RelocListElement,1);
			    if (tooBig) {
				continue;
			    }
			    tDescInfoPtr->flags |= FD_RELOCATE;
			    relocElemPtr->origFdNum = fdNum;
			    relocElemPtr->newFdNum = -1;
			    relocElemPtr->fdPtr = fdCopyPtr;
			    copyUsed = TRUE;
			}
			if (!valid) {
			    tDescInfoPtr->flags |= FD_UNREADABLE;
			}
		    }
		    if (valid && fdMagicOkay) {
			if (fdPtr->fileType == FS_DIRECTORY) {
			    tDescInfoPtr->flags |= IS_A_DIRECTORY;
			}
			if (fdPtr->flags & FSDM_FD_ALLOC) {
			    tDescInfoPtr->flags |= FD_ALLOCATED;
			    tDescInfoPtr->origLinkCount = fdPtr->numLinks;

			    CheckBlocks(partFID, domainPtr, fdNum, fdCopyPtr,
					newCylBitmapPtr, &modified,&copyUsed);
			    if (modified) {
				blockModified = TRUE;
			    }
			    if (tooBig) {
				continue;
			    }
			} else if (debug && 
			           fdNum == FSDM_BAD_BLOCK_FILE_NUMBER &&
				   fdPtr->firstByte == -1){
			    Output(stderr,"bad block has been reset.\n");
			}
		    }
		    if ((badBlockInit && fdNum == FSDM_BAD_BLOCK_FILE_NUMBER) ||
			    (salvage && !valid) || !(fdMagicOkay)) {
			if (badBlockInit && 
			    fdNum == FSDM_BAD_BLOCK_FILE_NUMBER) {
			    Output(stderr,  
				   "Clearing bad block file descriptor.\n");
			}
			ClearFd(fdMagicOkay ? FSDM_FD_ALLOC : FSDM_FD_FREE, 
				fdCopyPtr);
			blockModified = TRUE;
			modified = 1;
		    }
		    if (relocElemPtr != NULL) {
		       List_Insert((List_Links *)relocElemPtr,
				   LIST_ATREAR(relocList));
		    }
		    if (modified) {
			bcopy((Address)fdCopyPtr, (Address)fdPtr, 
			      sizeof(Fsdm_FileDescriptor));
		        fdPtr->version++;
		    }
		    if (copyUsed) {
			fdCopyPtr = NULL;
		    }
		}
	    }
	    if (blockModified) {
		blockBufferModified = TRUE;
		if (!batchRead && writeDisk) {
		    if (Disk_BlockWrite(partFID, domainPtr,
					domainPtr->fileDescOffset + i + k,
					1, (Address) block) < 0) {
			OutputPerror("CheckFileSystem: FD write failed");
			exit(EXIT_WRITE_FAILURE);
		    }
		}
	    }
	}
	if (batchRead && blockBufferModified && writeDisk) {
	    if (Disk_BlockWrite(partFID, domainPtr,
				domainPtr->fileDescOffset + i,
				blocksToRead, (Address) blockBuffer) < 0) {
		OutputPerror("CheckFileSystem: FD write failed");
		exit(EXIT_WRITE_FAILURE);
	    }
	}
    }
    if (!silent && fdBitmapError) {
	Output(stderr, "Found error in file descriptor bitmap\n");
    }


    /*
     * We now know which descriptors, if any, need to be relocated.  Allocate
     * new descriptors to hold the information.
     */

    if (!(List_IsEmpty(relocList))) {
	LIST_FORALL(relocList, (List_Links *)relocElemPtr) {
	    RelocateFD(domainPtr, descInfoPtr, relocElemPtr);
	}
    }
    /*
     * Fix all blocks that appeared in more than one file.
     */
    if (!(List_IsEmpty(copyList))) {
	int status = 0;

	if (verbose) {
	    Output(stderr, "\nCopying duplicate blocks\n");
	}
	LIST_FORALL(copyList, (List_Links *)copyElemPtr) {
	    List_Remove((List_Links *) copyElemPtr);
	    if (status != -1 && !noCopy) {
		status = CopyBlock(domainPtr, descInfoPtr, partFID, 
				   newCylBitmapPtr, copyElemPtr);
	    }
	    if ((copyElemPtr->parentType == FD) && 
		!((descInfoPtr[copyElemPtr->parentNum].flags & 
		 (ON_MOD_LIST | FD_RELOCATE)))) {
		 free((Address) copyElemPtr->fdPtr);
	    }
	    free((Address) copyElemPtr);
	}
    }
    /*
     * Go through the file system starting at the root and perform a 
     * consistency check.
     */
    if (!tooBig) {
	if (verbose) {
	    Output(stderr, "Traversing directory tree:\n\n");
	}
    
	CheckDirTree(partFID, domainPtr, descInfoPtr, fdBitmapPtr,
		     newCylBitmapPtr);
    } else {
	Output(stderr,
	       "NOT traversing directory tree because heap limit exceeded.\n"
	       );
    }
    /*
     * Now compare the two data block bitmaps.
     */
    if (verbose) {
	Output(stderr, "Comparing old and new data block bit maps:\n");
    }
    /*
     * Block 0 is reserved for the root directory, so mark it as in use.
     * There is code in the file system that will panic if block 0 is
     * reallocated.
     */
    if ((*cylBitmapPtr & 0xf0) != 0xf0) {
	if (!silent) {
	    Output(stderr, "Block 0 is free. Marking it used.\n");
	}
	foundError = 1;
	bitmapError = 1;
    }
    *newCylBitmapPtr |= 0xf0;
    *cylBitmapPtr |= 0xf0;
    for (oldPtr = cylBitmapPtr, newPtr = newCylBitmapPtr, i = 0, blockNum = 0; 
	 i < domainPtr->dataCylinders; 
	 i++, oldPtr = &cylBitmapPtr[bytesPerCylinder * i], 
	      newPtr = &newCylBitmapPtr[bytesPerCylinder * i]) {
	for (j = 0; 
	     j < bytesPerCylinder; 
	     j++, oldPtr++, newPtr++, blockNum += 2) {
	    if ((*oldPtr & 0xf0) != (*newPtr & 0xf0)) {
		if (bitmapVerbose) {
		    Output(stderr,"Block %d: old %x new %x.\n",
			blockNum * FS_FRAGMENTS_PER_BLOCK,
			(*oldPtr & 0xf0) >> 4, (*newPtr & 0xf0) >> 4);
		}
		foundError = 1;
		bitmapError = 1;
	    } 
	    if ((*oldPtr & 0x0f) != (*newPtr & 0x0f)) {
		if (bitmapVerbose) {
		    Output(stderr,"Block %d: old %x new %x.\n",
		       (blockNum + 1) * FS_FRAGMENTS_PER_BLOCK,
		       *oldPtr & 0x0f, *newPtr & 0x0f);
		}
		foundError = 1;
		bitmapError = 1;
	    }
	}
    }
    if (!silent && bitmapError) {
	Output(stderr, "Found error in data block bitmap\n");
    }
    fflush(stderr);

    /*
     * Print status information about disk.
     */
    Output(stdout, 
	    "%d files, %d blocks in use, %d blocks free, %d fragments\n",
	    numFiles, numBlocks, domainPtr->dataBlocks * 4 - numBlocks,
	    numFrags);


    if (foundError) {
	fixCount++;
	if (fixCount > numReboot) {
	    foundError = 1;
	    errorType = EXIT_NOREBOOT;
	}
    } else {
	fixCount = 0;
    }
    if (!writeDisk) {
	return;
    }

    if (verbose) {
	Output(stderr, "Writing disk\n");
    }
    fflush(stderr);

    /*
     * Write the file descriptor and data block bitmaps back out along with
     * the file descriptors.
     */
    if (patchHeader) {
	status = Disk_WriteDomainHeader(partFID, labelPtr, domainPtr);
	if (status != 0) {
	    Output(stderr, "Unable to write domain header.\n");
	    exit(EXIT_WRITE_FAILURE);
	}
    }

    WriteFileDescBitmap(partFID, domainPtr, fdBitmapPtr);
    WriteBitmap(partFID, domainPtr, newCylBitmapPtr);

    /*
     * Write out any modified file descriptors on the modified list.
     */
    WriteFileDesc(partFID, domainPtr, modList, descInfoPtr);
    WriteFileDesc(partFID, domainPtr, relocList, descInfoPtr);
    WriteSummaryInfo(partFID, labelPtr, domainPtr, 
				numBlocks, numFiles);
}


/*
 *----------------------------------------------------------------------
 *
 * CheckBlocks --
 *
 *	Check all the blocks associated with a file descriptor.
 *
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Error flags may be set.
 *
 *----------------------------------------------------------------------
 */
void
CheckBlocks(partFID, domainPtr, fdNum, fdPtr, newCylBitmapPtr, modifiedPtr,
	    copyUsedPtr)
    int 		partFID;		/* Raw handle on disk. */
    Ofs_DomainHeader	*domainPtr;		/* Ptr to domain info. */
    int			fdNum;			/* File descriptor # to check.*/
    Fsdm_FileDescriptor	*fdPtr;			/* Ptr to file descriptor that
						 * are checking. */
    unsigned char 	*newCylBitmapPtr;	/* Pointer to the disk block
						 * bit map. */
    int			*modifiedPtr;		/* TRUE => modified the file */
    Boolean		*copyUsedPtr;		/* TRUE => copy of fd was stored 						 * somewhere. */
 {
    register int		*indexPtr;
    register int		i, j;
    static int 			*dblIndirectBlock[FSDM_INDICES_PER_BLOCK];
    int				indBlock;
    int				lastBlock;
    int				lastFrag;
    int				tBlock;
    int				dirty;
    int				lastRealBlock;
    int				blockCount = 0;
    int				status = 0;
    Boolean			duplicate;
    int				virtualIndex;

    if (!(fdPtr->flags & FSDM_FD_ALLOC)) {
	return;
    }
    numFiles++;
    lastRealBlock = -1;
    /*
     * -1 is an empty file, anything less is an error.
     */
    if (fdPtr->lastByte < -1) {
	if (verbose || lastErrorFD != fdNum) {
	    Output(stderr, "File %d has bogus size %d. Setting to -1.\n",
		    fdNum, fdPtr->lastByte);
	}
	fdPtr->lastByte = -1;
	foundError = 1;
	*modifiedPtr = 1;
    }
    if (fdPtr->lastByte == -1) {
	lastBlock = -1;
	lastFrag = FS_FRAGMENTS_PER_BLOCK - 1;
    } else {
	lastBlock = fdPtr->lastByte / FS_BLOCK_SIZE;
	if (lastBlock < FSDM_NUM_DIRECT_BLOCKS) {
	    lastFrag = (fdPtr->lastByte & FS_BLOCK_OFFSET_MASK) / FS_FRAGMENT_SIZE;
	} else {
	    lastFrag = FS_FRAGMENTS_PER_BLOCK - 1;
	}
    }

    /*
     * First check direct blocks. Loop through them first and see if the 
     * file size corresponds to the number of blocks.
     */
    for (i = 0; i < FSDM_NUM_DIRECT_BLOCKS; i++) {
	if (i > lastBlock && fdPtr->direct[i] != FSDM_NIL_INDEX) {
	    lastBlock++;
	    fdPtr->lastByte += FS_BLOCK_SIZE;
	    foundError = 1;
	    *modifiedPtr = 1;
	    if (lastFrag != FS_FRAGMENTS_PER_BLOCK - 1) {
		fdPtr->lastByte += (FS_FRAGMENTS_PER_BLOCK -1 - lastFrag) *
				   FS_FRAGMENT_SIZE;
		lastFrag = FS_FRAGMENTS_PER_BLOCK - 1;
	    }
	    if (verbose || lastErrorFD != fdNum) {
		Output(stderr,
	"Found a direct block beyond end of file %d. Increasing file size.\n",
			fdNum);
		lastErrorFD = fdNum;
	    }
	}
    }
    for (i = 0; i < FSDM_NUM_DIRECT_BLOCKS; i++) {
	if (fileToPrint == fdNum) {
	    Output(stderr, "File %d, d[%d] = %d.\n", fdNum, i, 
		fdPtr->direct[i]);
	}
	if (fdPtr->direct[i] != FSDM_NIL_INDEX) {
	    if (blockToFind == fdPtr->direct[i]) {
		Output(stderr, "Block %d is d[%d] in file %d.\n", 
		    fdPtr->direct[i], i, fdNum);
	    }
	    if (i == lastBlock) {
		status = MarkBitmap(fdNum, fdPtr->direct[i],
				    newCylBitmapPtr,
				    lastFrag + 1, domainPtr);
		if (status >= 0) {
		    if (lastFrag + 1 != FS_FRAGMENTS_PER_BLOCK) {
			numFrags++;
		    }
		    blockCount += lastFrag + 1;
		    numBlocks += lastFrag + 1;
		}
	    } else {
		if (fdPtr->direct[i] & 
		    (FS_FRAGMENTS_PER_BLOCK - 1)) {
		    if (verbose || lastErrorFD != fdNum) {
			Output(stderr,
  "Block pointer %d invalid  in direct block %d of file %d. Block deleted.\n",
				    fdPtr->direct[i],i,fdNum);
			lastErrorFD = fdNum;
		    }
		    foundError = 1;
		    status = -1;
		} else {
		    status = MarkBitmap(fdNum, fdPtr->direct[i],
					newCylBitmapPtr,
					FS_FRAGMENTS_PER_BLOCK, domainPtr);
		    if (status >= 0) {
			blockCount += FS_FRAGMENTS_PER_BLOCK;
			numBlocks += FS_FRAGMENTS_PER_BLOCK;
		    }
		}
	    }
	    /*
	     * Current block is also used by another file. Put info in copy
	     * list so block is copied.
	     */
	    if (status == 1) {
		if (!tooBig) {
		    AddToCopyList(FD,fdPtr,fdNum,0,i,DIRECT, 
				  (i == lastBlock) ? lastFrag + 1 :
			          FS_FRAGMENTS_PER_BLOCK,  copyUsedPtr);
		}
	    }
	    if (status < 0) {
		fdPtr->direct[i] = FSDM_NIL_INDEX;
		if (fdPtr->fileType == FS_DIRECTORY) {
		    if (verbose || lastErrorFD != fdNum) {
			Output(stderr,
			"Hole in directory %d at direct block %d.\n", fdNum,
			i);
			lastErrorFD = fdNum;
		    }
		    AddToCopyList(FD,fdPtr,fdNum,0,i,DIRECT, 
				  (i == lastBlock) ? lastFrag + 1 :
				  FS_FRAGMENTS_PER_BLOCK,  copyUsedPtr);
		}
	    } else {
		lastRealBlock = i;
		if (i == lastBlock) {
		    break;
		}
	    }
	} else if (fdPtr->fileType == FS_DIRECTORY) {
	    if (verbose || lastErrorFD != fdNum) {
		Output(stderr, "Hole in directory %d at direct block %d.\n", 
		fdNum, i);
		lastErrorFD = fdNum;
	    }
	    AddToCopyList(FD,fdPtr,fdNum,0,i,DIRECT, 
			  (i == lastBlock) ? lastFrag + 1 :
			  FS_FRAGMENTS_PER_BLOCK,  copyUsedPtr);
	}
    }
    /*
     * Now check the singly indirect block.
     */
    if (fdPtr->indirect[0] == FSDM_NIL_INDEX) {
	if (fdPtr->fileType == FS_DIRECTORY && 
	    lastBlock > FSDM_NUM_DIRECT_BLOCKS) {
	    if (verbose || lastErrorFD != fdNum) {
		Output(stderr,
		"Hole in directory %d at single indirect block\n",
			       fdNum);
		lastErrorFD = fdNum;
	    }
	    AddToCopyList(FD,fdPtr,fdNum,0,FSDM_NUM_DIRECT_BLOCKS,INDIRECT, 
			  FS_FRAGMENTS_PER_BLOCK, copyUsedPtr);
	}
	i += FSDM_INDICES_PER_BLOCK;
    } else {
	tBlock = i;
	status = ProcessIndirectBlock(fdNum, partFID,lastBlock, FALSE, fdPtr, 
				      &fdPtr->indirect[0], newCylBitmapPtr,
				      domainPtr,&tBlock, &dirty,
				      &lastRealBlock, modifiedPtr, &blockCount,
				      copyUsedPtr);
	i = tBlock;
	/*
	 * We need to copy block
	 */
	if (status == 1) {
	    AddToCopyList(FD, fdPtr, fdNum, 0, FSDM_NUM_DIRECT_BLOCKS, INDIRECT,
			  FS_FRAGMENTS_PER_BLOCK, copyUsedPtr);
	}  
	if (status < 0) {
	    if (verbose || lastErrorFD != fdNum) {
		Output(stderr, 
			    "Hole in directory %d at single indirect block\n",
			       fdNum);
		lastErrorFD = fdNum;
	    }
	    AddToCopyList(FD, fdPtr, fdNum, 0, FSDM_NUM_DIRECT_BLOCKS, INDIRECT,
			  FS_FRAGMENTS_PER_BLOCK, copyUsedPtr);
	} 
    }
    if (fdPtr->indirect[1] == FSDM_NIL_INDEX) {
	goto checkBlockCount;
    }

    /*
     * Now check doubly indirect blocks.
     */
    indBlock = fdPtr->indirect[1];
    if (indBlock & (FS_FRAGMENTS_PER_BLOCK - 1)) {
	if (verbose || lastErrorFD != fdNum) {
	    Output(stderr, 
 "Doubly indirect block %d on non-block boundary in file %d.  Block deleted.\n",
			  indBlock, fdNum);
	    lastErrorFD = fdNum;
	    setCheckedBit = FALSE;
	}
	foundError = 1;
	status = -1;
    } else {
	status = MarkBitmap(fdNum, PhysToVirt(domainPtr, indBlock),
		newCylBitmapPtr,
		FS_FRAGMENTS_PER_BLOCK, domainPtr);
    }
    if (status >= 0) {
	if (Disk_BlockRead(partFID, domainPtr, 
			   indBlock / FS_FRAGMENTS_PER_BLOCK,
			   1, (Address) dblIndirectBlock) < 0) {
	    OutputPerror("CheckBlocks: Read failed");
	    fdPtr->indirect[1] = FSDM_NIL_INDEX;
	    *modifiedPtr = 1;
	    goto truncFile;
	}
	/* 
	 * Look over contents of block first and see if they make sense.
	 */
	for (j = 0, indexPtr = (int *) dblIndirectBlock; 
	     j < FSDM_INDICES_PER_BLOCK; 
	     j++, indexPtr++) {
	    if (*indexPtr != FSDM_NIL_INDEX) {
		virtualIndex = PhysToVirt(domainPtr, *indexPtr);
		if (virtualIndex < 0 || virtualIndex >= num1KBlocks) {
		    if (verbose || lastErrorFD != fdNum) {
			Output(stderr, 
			"Double indirect block %d of file %d contains garbage index %d\n", 
				       indBlock, fdNum, *indexPtr);
			lastErrorFD = fdNum;
		    }
		    fdPtr->indirect[1] = FSDM_NIL_INDEX;
		    setCheckedBit = FALSE;
		    *modifiedPtr = 1;
		    goto truncFile;
		} else if ( i + j * FSDM_INDICES_PER_BLOCK > lastBlock) {
		    lastBlock = i + j * FSDM_INDICES_PER_BLOCK;
		    fdPtr->lastByte += FS_BLOCK_SIZE * FSDM_INDICES_PER_BLOCK;
		    *modifiedPtr = 1;
		}

	    }
	}
	duplicate = FALSE;
	if (status == 1) {
	    duplicate = TRUE;
	    AddToCopyList(FD, fdPtr, fdNum, 0, FSDM_NUM_DIRECT_BLOCKS + 1,
			  DBL_INDIRECT,	FS_FRAGMENTS_PER_BLOCK, copyUsedPtr);
	}
	for (j = 0, indexPtr = (int *) dblIndirectBlock; 
	     j < FSDM_INDICES_PER_BLOCK && i <= lastBlock; 
	     j++, indexPtr++) {
	    if (*indexPtr == FSDM_NIL_INDEX) {
		if (fdPtr->fileType == FS_DIRECTORY) {
		    if (verbose || lastErrorFD != fdNum) {
			Output(stderr, 
			"Hole in directory %d in doubly indirect block\n",
				       fdNum);
			lastErrorFD = fdNum;
		    }
		    AddToCopyList(FD, fdPtr, fdNum, 0, FSDM_NUM_DIRECT_BLOCKS + 1,
			  DBL_INDIRECT, FS_FRAGMENTS_PER_BLOCK, copyUsedPtr);
		}
		i += FSDM_INDICES_PER_BLOCK;
		continue;
	    }
	    tBlock = i;
	    status = ProcessIndirectBlock(fdNum, partFID, lastBlock, duplicate, 
					  fdPtr, indexPtr, newCylBitmapPtr,
					  domainPtr, &tBlock, &dirty,
					  &lastRealBlock, modifiedPtr, 
					  &blockCount, copyUsedPtr);
	   i = tBlock;
	   if (status < 0) {
		if (verbose || lastErrorFD != fdNum) {
		    Output(stderr, 
		    "Hole in directory %d in doubly indirect blocks (2)\n",
			           fdNum);
		    lastErrorFD = fdNum;
		}
		AddToCopyList(FD, fdPtr, fdNum, 0, FSDM_NUM_DIRECT_BLOCKS + 1,
		      DBL_INDIRECT, FS_FRAGMENTS_PER_BLOCK, copyUsedPtr);
	    }
	}
	if (status == 1 && !duplicate) {
	    AddToCopyList(BLOCK, fdPtr, 0, indBlock, j, INDIRECT, 
			  FS_FRAGMENTS_PER_BLOCK, copyUsedPtr);
	}
	if (dirty) {
	    if (writeDisk) {
		if (Disk_BlockWrite(partFID, domainPtr, 
			   indBlock / FS_FRAGMENTS_PER_BLOCK,
			   1, (Address) dblIndirectBlock) < 0) {
		    OutputPerror("CheckBlocks: Write failed");
		    exit(EXIT_WRITE_FAILURE);
		}
	    }
	}
	numBlocks += FS_FRAGMENTS_PER_BLOCK;
	blockCount += FS_FRAGMENTS_PER_BLOCK;

    } else {
	if (fdPtr->fileType == FS_DIRECTORY) {
	    AddToCopyList(FD, fdPtr, fdNum, 0, FSDM_NUM_DIRECT_BLOCKS + 1,
			  DBL_INDIRECT, FS_FRAGMENTS_PER_BLOCK, copyUsedPtr);
        } else {
	    fdPtr->indirect[1] = FSDM_NIL_INDEX;
	    *modifiedPtr = 1;
	}
    }

    if (lastRealBlock == lastBlock) {
	goto checkBlockCount;
    }

truncFile:

    if (lastRealBlock != -1) {
	fdPtr->lastByte = (lastRealBlock + 1) * FS_BLOCK_SIZE - 1;
    } else {
	fdPtr->lastByte = -1;
    }
    Output(stderr,"Truncating file %d to length %d\n", fdNum,
			     fdPtr->lastByte + 1);
    foundError = 1;
    *modifiedPtr = 1;

checkBlockCount:

    if (blockCount != fdPtr->numKbytes) {
	if (verbose || lastErrorFD != fdNum) {
	    Output(stderr,
		"Block count corrected for file %d.  Is %d should be %d.\n", 
			   fdNum, fdPtr->numKbytes, blockCount);
	    lastErrorFD = fdNum;
	}
	fdPtr->numKbytes = blockCount;
	foundError = 1;
	*modifiedPtr = 1;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * ProcessIndirectBlock --
 *
 *	Scan through the file descriptors in the indirect block and
 *	update the cylinder map to reflect which blocks are allocated.
 *
 * Results:
 *	-1 if found a hole in an indirect block for a directory
 *	1 if block is already in use
 *	0 otherwise.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
int
ProcessIndirectBlock(fdNum, partFID,lastBlock,duplicate, fdPtr, blockNumPtr, 
		     newCylBitmapPtr,domainPtr,fileBlockNumPtr, dirtyPtr,
		     lastRealBlockPtr, modifiedPtr, blockCountPtr,
		     copyUsedPtr)
    int			fdNum;		  /* Number of file descriptor that 
					     indirect block is in. */
    register Fsdm_FileDescriptor	*fdPtr;	  /* Actual file descriptor. */
    int			*blockNumPtr;	  /* Pointer to indirect block number */
    unsigned	char 	*newCylBitmapPtr; /* The cylinder bit map to set bits
					     in for allocated blocks. */
    int			partFID;	  /* File id to use to read in indirect
					     blocks from disk. */
    Ofs_DomainHeader	*domainPtr;	  /* Domain to read from. */
    int			lastBlock;	  /* The last block in the file. */
    int			*fileBlockNumPtr; /* Pointer to file block number. */
    int			*dirtyPtr;	  /* 1 if *blockNumPtr gets 
					     modified. */
    int			*lastRealBlockPtr; /* Pointer to the number of the last
					      valid block that was found for the
					      file. */
    int			*modifiedPtr;	   /* 1 if the file descriptor
					      is modified. */
    int			*blockCountPtr;	   /* Count of the number of blocks in
					    * the file. */
    Boolean		*copyUsedPtr;	   /* TRUE => copy of fd was stored
					    * somewhere and should not be
					    * reused. */
    Boolean		duplicate;	   /* TRUE => we are processing a 
					    * duplicate block, therefore all
					    * direct blocks will also be 
					    * duplicates. */
{
    static char		indirectBlock[FS_BLOCK_SIZE];
    int			*indexPtr;
    int			i;
    int			dirty = 0;
    int			status;
    int 		tempFileBlock;
    Boolean		foundDuplicate = FALSE;
    int			zeroCount;



    if (*blockNumPtr & (FS_FRAGMENTS_PER_BLOCK - 1)) {
	if (verbose || lastErrorFD != fdNum) {
	    Output(stderr, 
    "Indirect block on a non-block boundary for file %d.  Block deleted.\n",
			   fdNum);
	    lastErrorFD = fdNum;
	}
	foundError = 1;
	status = -1;
    } else {
	status = MarkBitmap(fdNum, PhysToVirt(domainPtr, *blockNumPtr), 
			    newCylBitmapPtr, 
			    FS_FRAGMENTS_PER_BLOCK, domainPtr);
    }
    if (status == 0) {
	    /*
	     * FIXME: need to be able to flag this as a bad block on error.
	     */
	status = Disk_BlockRead(partFID, domainPtr, 
		               *blockNumPtr / FS_FRAGMENTS_PER_BLOCK, 1, 
			       (Address) indirectBlock);
	if (status < 0) {
	    OutputPerror("ProcessIndirectBlock: Read failed");
	}
    } else if (status == 1 ) {
	foundDuplicate = TRUE;
    }
    if (status < 0) {
	if (verbose || lastErrorFD != fdNum) {
	    Output(stderr,
	    "Indirect block (%d) unreadable for file #%d.  Block deleted.\n", 
			   *blockNumPtr, fdNum);
	    lastErrorFD = fdNum;
	}
	*fileBlockNumPtr += FSDM_INDICES_PER_BLOCK;
	*blockNumPtr = FSDM_NIL_INDEX;
	*dirtyPtr = 1;
	*modifiedPtr = 1;
	setCheckedBit = FALSE;
	return(0);
    } else {
	*dirtyPtr = 0;
    }
    /* 
     * Look over contents of block first and see if they make sense.
     */
    zeroCount = 0;
    for (i = 0, indexPtr = (int *) indirectBlock,
	 tempFileBlock = *fileBlockNumPtr; 
	 i < FSDM_INDICES_PER_BLOCK; 
	 i++, tempFileBlock++, indexPtr++) {
	if (*indexPtr != FSDM_NIL_INDEX) {
	    if (*indexPtr == 0) {
		zeroCount++;
		continue;
	    }
	    if (*indexPtr == blockToFind) {
		Output(stderr, "Block %d is i[%d] in file %d.\n",
		    *indexPtr, i, fdNum);
	    }
	    if (*indexPtr < 0 || *indexPtr >= num1KBlocks) {
		if (verbose || lastErrorFD != fdNum) {
		    Output(stderr, 
		    "Indirect block %d of file %d contains garbage index %d\n", 
				   *blockNumPtr, fdNum, *indexPtr);
		    lastErrorFD = fdNum;
		    setCheckedBit = FALSE;
		}
	        *fileBlockNumPtr += FSDM_INDICES_PER_BLOCK;
	        *blockNumPtr = FSDM_NIL_INDEX;
	        *dirtyPtr = 1;
	        *modifiedPtr = 1;
	        foundError = 1;
		if (fdPtr->fileType == FS_DIRECTORY) {
		   return(-1);
		} else {
		   return(0);
	        }
	     } else if (tempFileBlock > lastBlock) {
		 lastBlock = tempFileBlock;
		 fdPtr->lastByte = tempFileBlock * FS_BLOCK_SIZE -1;
		 *modifiedPtr = 1;
	     }
	 }
     }
    if (zeroCount == FSDM_INDICES_PER_BLOCK) {
	if (verbose || lastErrorFD != fdNum) {
	    Output(stderr, "Indirect block %d is all zeros.\n", 
			   *blockNumPtr, *indexPtr);
	    lastErrorFD = fdNum;
	}
	*fileBlockNumPtr += FSDM_INDICES_PER_BLOCK;
	*blockNumPtr = FSDM_NIL_INDEX;
	*dirtyPtr = 1;
	*modifiedPtr = 1;
	foundError = 1;
	setCheckedBit = FALSE;
	if (fdPtr->fileType == FS_DIRECTORY) {
	   return(-1);
	} else {
	   return(0);
	}
    }
    numBlocks += FS_FRAGMENTS_PER_BLOCK;
    *blockCountPtr += FS_FRAGMENTS_PER_BLOCK;
     for (i = 0, indexPtr = (int *) indirectBlock; 
	 i < FSDM_INDICES_PER_BLOCK && *fileBlockNumPtr <= lastBlock; 
	 i++, (*fileBlockNumPtr)++, indexPtr++) {

	 if (*indexPtr == FSDM_NIL_INDEX) {
	    if (fdPtr->fileType == FS_DIRECTORY) {
		AddToCopyList(BLOCK, fdPtr, 0, 
			    *blockNumPtr, i, DIRECT, 
			     FS_FRAGMENTS_PER_BLOCK, 
			     copyUsedPtr);
	    }
	    continue;
	 }
	 if (*indexPtr & (FS_FRAGMENTS_PER_BLOCK - 1)) {
	     if (verbose || lastErrorFD != fdNum) {
		 Output(stderr, 
		 "Non-direct block fragmented for file %d.  Block deleted.\n",
			   fdNum);
		 lastErrorFD = fdNum;
	     }
	     foundError = 1;
	     status = -1;
	 } else {
	     status = MarkBitmap(fdNum, *indexPtr, 
				newCylBitmapPtr,
				FS_FRAGMENTS_PER_BLOCK, domainPtr);
	 }
	/* 
	 * One of the direct blocks is a duplicate.
	 */
	if (status == 1 && !duplicate) {
	    AddToCopyList(BLOCK, fdPtr , 0, *blockNumPtr, 
			 i, DIRECT, 
			 FS_FRAGMENTS_PER_BLOCK, copyUsedPtr);
	}
	if (status >= 0) {
	    *blockCountPtr += FS_FRAGMENTS_PER_BLOCK;
	    numBlocks += FS_FRAGMENTS_PER_BLOCK;
	    *lastRealBlockPtr = *fileBlockNumPtr;
	} else {
	    *indexPtr = FSDM_NIL_INDEX;
	    dirty = 1;
	}
    }
    if (dirty) {
	if (writeDisk) {
	    if (Disk_BlockWrite(partFID, domainPtr, 
				*blockNumPtr / FS_FRAGMENTS_PER_BLOCK,
				1, (Address) indirectBlock) < 0) {
		OutputPerror("ProcessIndirectBlock: Write failed");
		exit(EXIT_WRITE_FAILURE);
	    }
	}
    }
    if (foundDuplicate) {
	return 1;
    }
    return(0);
}



/*
 *----------------------------------------------------------------------
 *
 * RelocateFD ---
 *
 *	Allocate a new file descriptor as close to the old one as possible.
 *	This is used to relocate descriptors in blocks that get I/O errors.
 *	This code is based on the kernel routine FsGetNewFileNumber.
 *	
 * Results:
 *	None.
 *
 * Side effects:
 *	The new descriptor is marked as allocated in the bitmap.
 *
 *----------------------------------------------------------------------
 */

void
RelocateFD(domainPtr, descInfoPtr, relocPtr)
    register Ofs_DomainHeader *domainPtr;	/* Domain to allocate the file 
					 * descriptor out of. */
    FdInfo *descInfoPtr;		/* information for each file
					 * descriptor */
    RelocListElement *relocPtr;		/* Pointer to information for old
					 * descriptor. */
{
    register int 	   	i;
    register int		j;
    int				startByte;
    register unsigned char 	*bitmapPtr;
    register unsigned char 	*bitmaskPtr;
    int			   	found = 0;
    static int		   	outOfDescriptors = 0;
    int			   	descBytes;

    /*
     * If we can't relocate it, chuck it.
     */
    if (outOfDescriptors) {
	descInfoPtr[relocPtr->origFdNum].flags |= FD_UNREADABLE;
	return;
    }

    /*
     * Linear search forward the bit map a byte at a time.
     */
    startByte = relocPtr->origFdNum / BITS_PER_BYTE;
    bitmapPtr = &fdBitmapPtr[startByte];
    descBytes = domainPtr->numFileDesc >> 3;
    i = startByte;
    do {
	if (*bitmapPtr != 0xff) {
	    found = 1;
	    break;
	}
	i++;
	if (i == descBytes) {
	    i = 0;
	    bitmapPtr = fdBitmapPtr;
	} else {
	    bitmapPtr++;
	}
    } while (i != startByte);

    if (!found) {
	/*
	 * Couldn't relocate it, so throw it away and pretend we couldn't
	 * recover it.
	 */
	if (!outOfDescriptors) {
	    Output(stderr, "RelocateFD: out of file descriptors.\n");
	    abort();
	} else {
	    outOfDescriptors = 1;
	}
	descInfoPtr[relocPtr->origFdNum].flags |= FD_UNREADABLE;
	return;
    }

    /*
     * Now find which file descriptor is free within the byte.
     */
    for (j = 0, bitmaskPtr = bitmasks; 
	 j < 8 && (*bitmapPtr & *bitmaskPtr) != 0; 
	 j++, bitmaskPtr++) {
    }
    relocPtr->newFdNum = i * 8 + j;
    if (verbose || lastErrorFD != relocPtr->origFdNum) {
	Output(stderr,
	"Fd %d relocated to %d.\n",relocPtr->origFdNum,relocPtr->newFdNum);
	lastErrorFD = relocPtr->origFdNum;
    }
    *bitmapPtr |= *bitmaskPtr;
}


/*
 *----------------------------------------------------------------------
 *
 *  CopyBlock ---
 *
 *	Copy a block and fix up pointers to it.  FillNewBlock does the
 *	real work. CopyBlock gets the block info off the copyList and
 *	calls FillNewBlock.
 *         This procedure is also used to fill in holes in directories.
 * Results:
 *	0  - ok
 *	-1 -  disk is full
 *	-2 - some other error
 *
 * Side effects:
 *	Parent (either fd or indirect block) is changed to point to copy.
 *
 *----------------------------------------------------------------------
 */
int
CopyBlock(domainPtr, descInfoPtr, partFID, bitmapPtr, copyPtr)
    register Ofs_DomainHeader 	*domainPtr;	/* Domain to allocate the file 
						 * descriptor out of. */
    FdInfo 			*descInfoPtr;	/* information for each file
						 * descriptor */
    int				 partFID;	/* File id to read from disk */
    u_char			*bitmapPtr;	/* Cylinder bitmap pointer */
    CopyListElement 		*copyPtr;	/* Pointer to information for 
						 * block to be copied */
{
    int			blockNum;
    int			parentBlockNum;
    int			*blockNumPtr;
    Fsdm_FileDescriptor	*fdPtr = NULL;
    int			fdNum;
    static int 		block[FSDM_INDICES_PER_BLOCK];
    int			status;



    fdPtr = copyPtr->fdPtr;
    if (copyPtr->parentType == FD) {
	fdNum = copyPtr->parentNum;
	switch (copyPtr->blockType) {
	    case DIRECT: 
		blockNumPtr = &fdPtr->direct[copyPtr->index];
		break;
	    case INDIRECT:
		blockNumPtr = &fdPtr->indirect[0];
		break;
	    case DBL_INDIRECT:
		blockNumPtr = &fdPtr->indirect[1];
		break;
	}
    } else {
	parentBlockNum = copyPtr->parentNum;
	status = Disk_BlockRead(partFID, domainPtr, 
		               parentBlockNum / FS_FRAGMENTS_PER_BLOCK, 1, 
			       (Address) block);
	if (status < 0) {
	    OutputPerror(
		"CopyBlock: Previously readable block %d unreadable.\n",
		parentBlockNum);
	    foundError = 1;
	    return -2;
	}
	blockNumPtr = &(block[copyPtr->index]);
    }
    blockNum = *blockNumPtr;
    status = FillNewBlock(blockNum, copyPtr->blockType, copyPtr->fragments,
			  fdPtr, domainPtr, descInfoPtr, partFID, 
			  bitmapPtr, blockNumPtr);
    /* 
     * If status is -1 then disk is full so stop trying to copy blocks.
     */
    if (status == -1) {
	Output(stderr,"fscheck: disk is full.\n");
	foundError = 1;
	errorType = EXIT_DISK_FULL;
    }
    if (!writeDisk) {
	return 0;
    }
    if (copyPtr->parentType == FD) {
	if (!(descInfoPtr[fdNum].flags & (ON_MOD_LIST | FD_RELOCATE))) {
	     ModListElement	*modElemPtr;

	    Alloc(modElemPtr,ModListElement,1);
	    if (tooBig) {
		return 0;
	    }
	    descInfoPtr[fdNum].flags |= ON_MOD_LIST;
	    modElemPtr->fdNum = fdNum;
	    modElemPtr->fdPtr = fdPtr;
	    List_Insert((List_Links *)modElemPtr, LIST_ATREAR(modList));
	}
    } else {
	if (Disk_BlockWrite(partFID, domainPtr,
			    parentBlockNum / FS_FRAGMENTS_PER_BLOCK,
			    1, (Address) block) < 0) {
	    OutputPerror("CopyBlock: Unable to write block %d.\n",
		parentBlockNum);
	    foundError = 1;
	    return 0;
	}
    }
    return 0;
}

/*
 *----------------------------------------------------------------------
 *
 *  FillNewBlock ---
 *
 *	Looks through the bitmap for a free block and copies the given block
 *	into it. If the given block is an indirect block then we recurse to
 *	also copy all included blocks. Note that we only look for an empty
 *	full block (4 fragments). This may lead to a disk full error when
 *	the disk really isn't full.
 * 
 * Results:
 *	0 - ok
 *	-1 - disk full
 *	-2 - some other error
 *
 * Side effects:
 *	A block in the bitmap is marked as in use.
 *
 *----------------------------------------------------------------------
 */
int
FillNewBlock(blockNum, blockType, fragments, fdPtr, domainPtr, 
		     descInfoPtr, partFID, bitmapPtr, newBlockNumPtr)
    int 		blockNum;		/* Block number of block to be
						 * copied. */
    BlockIndexType 	blockType;		/* Type of block to be copied
						 */
    int 		fragments;		/* Number of fragments in block
						 */
    Fsdm_FileDescriptor	*fdPtr;			/* Ptr at fd of file. */
    Ofs_DomainHeader	*domainPtr;   		/* Ptr at domain info */
    FdInfo		*descInfoPtr;		/* Descriptor info array */
    int			partFID;		/* File id to read from disk */
    u_char		*bitmapPtr;		/* Cylinder data block bitmap */
    int 		*newBlockNumPtr;	/* Block number of new allocated
						 * block */
{
    char		block[FS_BLOCK_SIZE];
    int			*indBlock;
    int			newBlockNum;
    int			i;
    int			status;
    int			vBlockNum;

    if (blockNum != FSDM_NIL_INDEX) { 
	vBlockNum = (blockType == DIRECT) ? blockNum : 
		    PhysToVirt(domainPtr,blockNum);
	status = Disk_FragRead(partFID, domainPtr, 
				VirtToPhys(domainPtr,vBlockNum), 
				fragments, (Address) block);
	if (status < 0) {
	    OutputPerror("FillNewBlock: Unable to read block %d to copy.\n",
			 vBlockNum);
	    foundError = 1;
	    return -2;
	}
    } else if (fdPtr->fileType == FS_DIRECTORY) {
	vBlockNum = -1;
	bzero(block, FS_BLOCK_SIZE);
    } else {
	Output(stderr,"Internal error: trying to copy null index.\n");
	return -2;
    }
    newBlockNum = AllocBlock(domainPtr,fragments,bitmapPtr);
    if (newBlockNum == -1) {
	/*
	 * Disk is full
	 */
	return -1;
    }
    if (blockType != DIRECT) {
	BlockIndexType entryType;

	if (blockType == INDIRECT) {
	    entryType = DIRECT;
	} else {
	    entryType = INDIRECT;
	}
	for (i = 0,indBlock = (int *)block; 
	     i < FSDM_INDICES_PER_BLOCK; 
	     i++,indBlock++){

	     if (*indBlock != FSDM_NIL_INDEX || 
		 fdPtr->fileType == FS_DIRECTORY) {

		status = FillNewBlock(*indBlock, entryType, 
				      FS_FRAGMENTS_PER_BLOCK, fdPtr, domainPtr, 
				      descInfoPtr, partFID, bitmapPtr,
				      indBlock);
	    }
	    if (status == -1) {
		break;
	    } 
	}
    /*
     * If the block number is nil then we are filling in a hole in a directory.
     * Fill out the disk block with empty entries.
     */
    } else if (blockNum == FSDM_NIL_INDEX) {
	Fslcl_DirEntry *entryPtr;
	int i;

	bzero( block, FS_BLOCK_SIZE);

	for (entryPtr = (Fslcl_DirEntry *)block, i = 0; 
	     i < fragments * FS_FRAGMENT_SIZE  / FSLCL_DIR_BLOCK_SIZE;
	     i++,entryPtr=(Fslcl_DirEntry *)((int)entryPtr + FSLCL_DIR_BLOCK_SIZE)) {
	     entryPtr->fileNumber = 0;
	     entryPtr->recordLength = FSLCL_DIR_BLOCK_SIZE;
	     entryPtr->nameLength = 0;
	}
    }
    if (writeDisk) {
	status = Disk_FragWrite(partFID, domainPtr,
				 VirtToPhys(domainPtr, newBlockNum),
				 fragments,
			        (Address) block);
	if (status < 0) {
	    OutputPerror("FillNewBlock: Unable to write to new block %d.\n",
			 newBlockNum);
	    foundError = 1;
	    return -2;
	}
    }
    *newBlockNumPtr = (blockType == DIRECT) ? newBlockNum :
		      VirtToPhys(domainPtr, newBlockNum);
    if (verbose) {
	if (vBlockNum != -1) {
	    Output(stderr,"Copied %d fragments starting at %d to %d.\n", 
	    fragments, vBlockNum, newBlockNum);
	} else {
	    Output(stderr,"Added new directory block %d.\n", newBlockNum);
	}
    }
    return 0;
}
