/* 
 * devNet.c --
 *
 *	Device interface to the network.  The routines here implement
 *	filesystem devices for the various ethernet protocols.  Input
 *	queues of received packets are maintainted here, although the
 *	device has to be open before packets are queued.  There is a
 *	different queue for each different ethernet protocol, and the
 *	device unit number is used to identify the protocol.
 *
 *	TODO: this needs to be fixed to understand more than one network
 *	interface.  It seems that at open time the correct interface
 *	should be chosen.  Also, some interface-data needs to be passed
 *	down to the output routines so they can choose the right interface.
 *
 * Copyright 1987 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header$ SPRITE (Berkeley)";
#endif not lint


#include "sprite.h"
#include "net.h"
#include "devNet.h"
#include "user/netInet.h"
#include "stdlib.h"
#include "sync.h"
#include "fs.h"
#include "user/net.h"

Boolean devNetEtherDebug = FALSE;

/*
 * The  packets are kept in a circular queue whose length is a power of 2.
 * Stuff gets added to the queues at interrupt time so removal requires
 * synchronization with MASTER_LOCK/UNLOCK.
 */

#define PACKET_QUEUE_LEN	16
typedef struct {
    int		head;
    int		tail;
    Address	packet[PACKET_QUEUE_LEN];
    int		size[PACKET_QUEUE_LEN];
} PacketQueue;

/*
 * A circular queue is full if its tail is right in front of its
 * head (mod length). A queue is empty if its tail is its head.
 */

#define NextTail(queue)		(((queue).tail + 1) & (PACKET_QUEUE_LEN -1))
#define NextHead(queue)		(((queue).head + 1) & (PACKET_QUEUE_LEN -1))

#define QueueFull(queue)	(NextTail(queue) == (queue).head)
#define QueueEmpty(queue)       ((queue).tail == (queue).head)

/*
 * Event counters kept on a per-protocol basis.
 */
typedef struct ProtoStats {
    int		shorts;		/* Number of short packets received */
    int		drops;		/* Number of packets dropped when queue full */
} ProtoStats;

/*
 * State for the protocols.  They are linked together and scanned when
 * a packet is received.  Packets with protocols that don't match any
 * of the protocols in this list will be dropped.
 */

typedef struct ProtocolState {
    List_Links		links;
    int			protocol;	/* Ethernet protocol number */
    Boolean		open;		/* TRUE is the device is open.  Packets
					 * are only queued if it is open. */
    Boolean		kernel;		/* TRUE if kernel is using this proto */
    PacketQueue		queue;		/* Queue of received packets */
    ProtoStats		stats;		/* Event counters */
    Fs_NotifyToken	fsReadyToken;	/* Used for filesystem callback that
					 * notifies waiting processes that
					 * packets are here */
    int			(*inputProc)();	/* Used when the kernel is using the
					 * protocol. */
} ProtocolState;

/*
 * The header for the list of protocols.
 */
List_Links etherProtos;
static Boolean initList = FALSE;

/*
 * A master lock is used to synchronize access to the list of protocols.
 */
static Sync_Semaphore protoMutex = Sync_SemInitStatic("Dev:protoMutex");


/*
 *----------------------------------------------------------------------
 *
 * DevNet_FsOpen --
 *
 *	Open an ethernet protocol device.  Protocol state is set up and
 * 	linked in to a list of active protocols.  The protocol is marked
 * 	open to enable queueing of packets for this protocol.
 *
 * Results:
 *	SUCCESS		- the operation was successful.
 *	FS_FILE_BUSY	- the device was already opened.
 *
 * Side effects:
 *	Storage for the protocol state is allocated and linked into
 *	the list of active protocols.  If this has already been done,
 *	then the protocol is simply marked as open.
 *
 *----------------------------------------------------------------------
 */
/*ARGSUSED*/
ReturnStatus
DevNet_FsOpen(devicePtr, useFlags, data, flagsPtr)
    Fs_Device   *devicePtr;	/* Device info, unit number == protocol */
    int 	useFlags;	/* Flags from the stream being opened */
    Fs_NotifyToken  data;	/* Call-back for input notification */
    int		*flagsPtr;	/* OUT: Device flags. */
{
    register ProtocolState *protoPtr;
    register int i;
    register unsigned int protocol;
    ReturnStatus status = SUCCESS;
    ProtocolState *newProtoPtr;	

    /*
     * Allocate a new protocol state structure before we grap a master lock.
     * This is only needed if this protocol is being opened for this 
     * first time. 
     */
    newProtoPtr = (ProtocolState *)malloc(sizeof(ProtocolState));
    MASTER_LOCK(&protoMutex);
    if (!initList) {
	List_Init(&etherProtos);
	Sync_SemRegister(&protoMutex);
	initList = TRUE;
    }
    /*
     * We keep the protocol number in network byte order so as to match
     * the values coming off the net.
     */
    protocol = (unsigned int) 
		Net_HostToNetShort((unsigned short) (devicePtr->unit));  
    LIST_FORALL(&etherProtos, (List_Links *)protoPtr) {
	if (protoPtr->protocol == protocol) {
	    if (protoPtr->open) {
		printf("Warning: DevNet_FsOpen: Extra open of net device");
		status = FS_FILE_BUSY;
		goto exit;
	    }
	    goto found;
	}
    }

    /*
     * Protocol not in the list.  Just stick it at the end, although it
     * would be possible to sort the list...
     */

    protoPtr = newProtoPtr;
    newProtoPtr = (ProtocolState *) NIL;
    List_InitElement((List_Links*) protoPtr);
    List_Insert((List_Links *)protoPtr, LIST_ATREAR(&etherProtos));

    protoPtr->protocol = protocol;
    bzero((Address)&protoPtr->stats,sizeof(ProtoStats));
    /*
     * Pre-allocate buffer space for the input queue.
     * Vm_RawAlloc is used because this queue space is never recycled.
     */

    for (i=0 ; i< PACKET_QUEUE_LEN ; i++) {
	protoPtr->queue.packet[i] = (Address) Vm_RawAlloc(NET_ETHER_MAX_BYTES);
    }


found:

    /*
     * Differentiate between user-level reads and the kernel.
     * DevNetEtherHandler will handle packets differently in
     * the two cases.  It notifies the user-level process, or
     * it calls the kernel protocol handler.
     */
    if (useFlags & FS_USER) {
	protoPtr->kernel = FALSE;
	protoPtr->fsReadyToken = data;
    } else {
	protoPtr->kernel = TRUE;
	protoPtr->inputProc = (int(*)())data;
    }
    protoPtr->open = TRUE;
    protoPtr->queue.head = 0;
    protoPtr->queue.tail = 0;

    /*
     * These client data fields are set up for call backs to the filesystem
     * and so we can quickly get to the protocol state on read/write etc.
     */
    devicePtr->data = (ClientData) protoPtr;

exit:

    if (devNetEtherDebug) {
	printf("DevNet_FsOpen: Open proto 0x%x status 0x%x\n", 
			devicePtr->unit, status);
    }

    MASTER_UNLOCK(&protoMutex);
    /*
     * Free the potocol state pointer if we didn't need it.
     */
    if (newProtoPtr != (ProtocolState *) NIL) {
	free((char *) newProtoPtr);
    }
    return(status);
}

/*
 *----------------------------------------------------------------------
 *
 * DevNet_FsReopen --
 *
 *	Reopen an ethernet protocol device.  Call the regular
 *	open routine to do all the work.
 *
 * Results:
 *	SUCCESS		- the operation was successful.
 *	FS_FILE_BUSY	- the device was already opened.
 *
 * Side effects:
 *	Storage for the protocol state is allocated and linked into
 *	the list of active protocols.  If this has already been done,
 *	then the protocol is simply marked as open.
 *
 *----------------------------------------------------------------------
 */
/*ARGSUSED*/
ReturnStatus
DevNet_FsReopen(devicePtr, refs, writers, data, flagsPtr)
    Fs_Device   *devicePtr;	/* Device info, unit number == protocol */
    int 	refs;		/* Number of open network streams */
    int		writers;	/* Number that are open for writing */
    Fs_NotifyToken  data;	/* Call-back for input notification */
    int		*flagsPtr;	/* OUT: Device flags. */
{
    int useFlags = FS_READ;

    if (writers) {
	useFlags |= FS_WRITE;
    }
    return( DevNet_FsOpen(devicePtr, useFlags, data, flagsPtr) );
}


/*
 *----------------------------------------------------------------------
 *
 * DevNetEtherHandler --
 *
 *	Dispatcher for ethernet packets.  The list of active protocols
 *	is scanned for a matching protocol.  If found, the packet is
 *	enqueued for the protocol.
 *
 *	Note: This routine is called from the ethernet interrupt routine.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The packet is saved in the protocols queue (if there's room and
 *	the protocol device is currently open).
 *
 *----------------------------------------------------------------------
 */

void
DevNetEtherHandler(packetPtr, size)
    Address	packetPtr;	/* Pointer to the packet in the hardware
				 * receive buffer. */
    int		size;		/* Size of the packet. */
{
    register ProtocolState *protoPtr;
    register Net_EtherHdr *etherHdrPtr = (Net_EtherHdr *)packetPtr;

    MASTER_LOCK(&protoMutex);
    if (!initList) {
	List_Init(&etherProtos);
	initList = TRUE;
    }

    if (devNetEtherDebug) {
	printf("EtherHandler 0x%x %d\n", NET_ETHER_HDR_TYPE(*etherHdrPtr),
			size);
    }
    LIST_FORALL(&etherProtos, (List_Links *)protoPtr) {
	if (NET_ETHER_HDR_TYPE(*etherHdrPtr) == protoPtr->protocol && 
				protoPtr->open) { 
	    if (QueueFull(protoPtr->queue)) {
		protoPtr->stats.drops++;
	    } else {
		bcopy(packetPtr,protoPtr->queue.packet[protoPtr->queue.tail],
		      size);
		protoPtr->queue.size[protoPtr->queue.tail] = size;
		protoPtr->queue.tail = NextTail(protoPtr->queue);
		if (protoPtr->kernel) {
		    /*
		     * Indirect to a process to pull the packet off of
		     * the input queue.  We can't call the protocol handlers
		     * directly because they use malloc and free.  Note
		     * that we pass the protoPtr to the packet handler
		     * so it can call DevNet_FsRead to get the packet.
		     */
		    Proc_CallFunc(protoPtr->inputProc, (ClientData)protoPtr, 0);
		} else {
		    Fs_DevNotifyReader(protoPtr->fsReadyToken);
		}
	    }
	    break;
	}
    }
    MASTER_UNLOCK(&protoMutex);
}

/*
 *----------------------------------------------------------------------
 *
 * DevNet_FsRead --
 *
 *	Read a packet for an ethernet protocol.  This returns the first
 *	packet from the protocol's input queue, if any.  This returns
 *	data from at most 1 network packet.  If the caller's buffer is
 *	too short, the packet is truncated.
 *
 * Results:
 *	SUCCESS		- if a packet was found in the queue.  
 *	FS_WOULD_BLOCK	- no packets found.
 *
 * Side effects:
 *	Removes the first packet from the queue and copies it into
 *	the receiver's buffer.
 *
 *----------------------------------------------------------------------
 */

/*ARGSUSED*/
ReturnStatus
DevNet_FsRead(devicePtr, readPtr, replyPtr)
    Fs_Device	*devicePtr;
    Fs_IOParam	*readPtr;	/* Read parameter block */
    Fs_IOReply	*replyPtr;	/* Return length and signal */ 
{
    ReturnStatus status;
    register ProtocolState *protoPtr;
    register Address packetPtr;
    register int size;

    protoPtr = (ProtocolState *)devicePtr->data;

    MASTER_LOCK(&protoMutex);
    if (QueueEmpty(protoPtr->queue)) {
	size = 0;
	status = FS_WOULD_BLOCK;
	if (devNetEtherDebug) {
	    printf("DevNet_FsRead: empty queue, proto 0x%x\n",
				protoPtr->protocol);
	}
    } else {
	packetPtr = protoPtr->queue.packet[protoPtr->queue.head];
	size = protoPtr->queue.size[protoPtr->queue.head];

	protoPtr->queue.head = NextHead(protoPtr->queue);

	if (size > readPtr->length) {
	    size = readPtr->length;
	}
	bcopy(packetPtr, readPtr->buffer, size);

	status = SUCCESS;
	if (devNetEtherDebug) {
	    printf("DevNet_FsRead: Found packet proto 0x%x, size %d\n",
				protoPtr->protocol, size);
	}
    }

    replyPtr->length = size;
    MASTER_UNLOCK(&protoMutex);
    return(status);
}


/*
 *----------------------------------------------------------------------
 *
 * DevNet_FsWrite --
 *
 *	Pass a packet off to the network driver for output.  The driver
 *	maintains a transmit queue so we don't have to.  The protocol
 *	in the output packet header is verified to be the one corresponding
 *	to the device file.
 *
 * Results:
 *	SUCCESS		- the packet was transmitted.
 *	SYS_INVALID_ARG	- packet is too small or the protocol in the
 *			  buffer doesn't match the one for the device.
 *
 * Side effects:
 *	Initiates transmission of the packet.
 *
 *----------------------------------------------------------------------
 */

/*ARGSUSED*/
ReturnStatus
DevNet_FsWrite(devicePtr, writePtr, replyPtr)
    Fs_Device	*devicePtr;
    Fs_IOParam	*writePtr;	/* Standard write parameter block */
    Fs_IOReply	*replyPtr;	/* Return length and signal */
{
    register Net_EtherHdr *etherHdrPtr;
    register ProtocolState *protoPtr;
    Net_ScatterGather ioVector;

    if (writePtr->length < sizeof(Net_EtherHdr) ||
	writePtr->length > NET_ETHER_MAX_BYTES) {
	return(SYS_INVALID_ARG);
    }
    protoPtr = (ProtocolState *)devicePtr->data;
    etherHdrPtr = (Net_EtherHdr *)writePtr->buffer;

    /*
     * Verify the protocol type in the header.  The low level driver
     * will fill in the source address for us.
     */
    if (NET_ETHER_HDR_TYPE(*etherHdrPtr) != protoPtr->protocol) {
	return(SYS_INVALID_ARG);
    }

    ioVector.bufAddr = (Address)((int)writePtr->buffer + sizeof(Net_EtherHdr));
    ioVector.length  = writePtr->length - sizeof(Net_EtherHdr);

    Net_EtherOutputSync(etherHdrPtr, &ioVector, 1);

    replyPtr->length = writePtr->length;
    return(SUCCESS);
}


/*
 *----------------------------------------------------------------------
 *
 * DevNet_FsClose --
 *
 *	Close the device.
 *
 * Results:
 *	SUCCESS
 *
 * Side effects:
 *	The protocol state is marked as closed, and any packets on the
 *	input queue are discarded.
 *
 *----------------------------------------------------------------------
 */
/*ARGSUSED*/
ReturnStatus
DevNet_FsClose(devicePtr, useFlags, openCount, writerCount)
    Fs_Device	*devicePtr;	/* Device info. */
    int		useFlags;	/* FS_READ | FS_WRITE */
    int		openCount;	/* Number of times device still open. */
    int		writerCount;	/* Number of writers still on the device. */
{
    ProtocolState *protoPtr;

    MASTER_LOCK(&protoMutex);

    protoPtr = (ProtocolState *)devicePtr->data;
    protoPtr->open = FALSE;
    protoPtr->fsReadyToken = (Fs_NotifyToken)NIL;
    /*
     * Nuke the queue here?
     */

    MASTER_UNLOCK(&protoMutex);
    return(SUCCESS);
}


/*
 *----------------------------------------------------------------------
 *
 * DevNet_FsSelect --
 *
 *	Perform device-specific select functions on the device.
 *	Always indicates that the device is writable. Indicates the
 *	device is readable if the queue is not empty.
 *
 * Results:
 *	SUCCESS		- the operation was successful.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

/*ARGSUSED*/
ReturnStatus
DevNet_FsSelect(devicePtr, readPtr, writePtr, exceptPtr)
    Fs_Device	*devicePtr;
    int			*readPtr;
    int			*writePtr;
    int			*exceptPtr;
{
    register ProtocolState *protoPtr;

    MASTER_LOCK(&protoMutex);
    protoPtr = (ProtocolState *)devicePtr->data;

    if (*readPtr) {
	if (QueueEmpty(protoPtr->queue)) {
	    *readPtr = 0;
	}
    }
    *exceptPtr = 0;
    MASTER_UNLOCK(&protoMutex);
    return(SUCCESS);
}


/*
 *----------------------------------------------------------------------
 *
 * DevNet_FsIOControl --
 *
 *	Not implemented yet.
 *
 * Results:
 *	SUCCESS		- the operation was successful.
 *	SYS_INVALID_ARG - bad command, or wrong buffer size.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

/*ARGSUSED*/
ReturnStatus
DevNet_FsIOControl(devicePtr, ioctlPtr, replyPtr)
    Fs_Device	*devicePtr;
    Fs_IOCParam *ioctlPtr;
    Fs_IOReply *replyPtr;
{
    return(FAILURE);
}
