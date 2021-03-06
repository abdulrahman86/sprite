/* 
 * Net_AddrCmp.c --
 *
 *	Routines to compare network addresses for equality..
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
static char rcsid[] = "$Header: /sprite/src/lib/net/RCS/Net_AddrCmp.c,v 1.1 92/03/27 16:17:09 voelker Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include "sprite.h"
#include "net.h"


/*
 *----------------------------------------------------------------------
 *
 * Net_AddrCmp --
 *
 *	Compares two network addresses for equality.  
 *	
 *
 * Results:
 *	0 if the addresses are equal, 1 otherwise
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Net_AddrCmp(aPtr, bPtr)
    Net_Address		*aPtr;	/* First address. */
    Net_Address		*bPtr;	/* Second address. */
{
    int result;
    if (aPtr->type != bPtr->type) {
	return 1;					
    }
    switch(aPtr->type) {	
	case NET_ADDRESS_ETHER: 	
	    result = Net_EtherAddrCmp(aPtr->address.ether, bPtr->address.ether);
	    break;						
	case NET_ADDRESS_ULTRA:					
	    result = Net_UltraAddrCmp(aPtr->address.ultra, bPtr->address.ultra);
	    break;						
	case NET_ADDRESS_FDDI:					
	    result = Net_FDDIAddrCmp(aPtr->address.fddi, bPtr->address.fddi);
	    break;
	case NET_ADDRESS_INET:
	    result = Net_InetAddrCmp(aPtr->address.inet, bPtr->address.inet);
	    break;
    }
    return result;
}

