/*
 * netLEMachInt.h --
 *
 *	Internal definitions for the sun4c LANCE controller.
 *
 * Copyright 1990 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /cdrom/src/kernel/Cvsroot/kernel/net/sun4c.md/netLEMachInt.h,v 1.2 91/11/15 20:58:12 kupfer Exp $ SPRITE (Berkeley)
 */

#ifndef _NETLEMACHINT
#define _NETLEMACHINT

#include <netInt.h>

/*
 * Register address port and register data port definition.
 */

typedef struct NetLEMach_Reg {
	volatile unsigned short	dataPort;	/* RDP */
        volatile unsigned short	addrPort[1];	/* RAP */
} NetLEMach_Reg;

#define NET_LE_REG_SIZE	4

/*
 * On the sun4c we don't have to copy packets because they are mapped
 * into DVMA space.
 */

#define NET_LE_COPY_PACKET FALSE

/*
 * Macros for converting chip to cpu and cpu to chip address.
 * We always deal with chip addresses in two parts, the lower 16 bits
 * and the upper 8 bits.
 */
#define	NET_LE_FROM_CHIP_ADDR(statePtr, high,low)	\
		((Address) (0xff000000 + ((high) << 16) + (low)))

#define	NET_LE_TO_CHIP_ADDR_LOW(a) ( ((unsigned int) (a)) & 0xffff)
#define	NET_LE_TO_CHIP_ADDR_HIGH(a) ( (((unsigned int) (a)) >> 16) & 0xff)

/* 
 * Routine to allocate a network buffer.
 */
#define BufAlloc(statePtr, numBytes) \
    (char *) VmMach_NetMemAlloc(numBytes);

#endif /* _NETLEMACHINT */
