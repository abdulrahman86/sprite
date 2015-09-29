/* 
 * Host_ByInetAddr.c --
 *
 *	Source code for the Host_ByInetAddr library procedure.
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
static char rcsid[] = "$Header: /sprite/src/lib/c/host/RCS/Host_ByInetAddr.c,v 1.1 88/06/30 11:06:43 ouster Exp Locker: mendel $ SPRITE (Berkeley)";
#endif not lint

#include <stdio.h>
#include <host.h>
#include <hostInt.h>


/*
 *-----------------------------------------------------------------------
 *
 * Host_ByInetAddr --
 *
 *	Find information about the host with the given internet address.
 *
 * Results:
 *	A Host_Entry structure, or NULL if no host exists in the database
 *	with the given internet address.  The Host_Entry is statically
 *	allocated, and may be modified on the next call to any Host_
 *	procedure.
 *
 * Side Effects:
 *	The host database file is opened if it wasn't already.
 *
 *-----------------------------------------------------------------------
 */

Host_Entry *
Host_ByInetAddr(inetAddr)
    register struct in_addr inetAddr;	/* Address to match. */
{
    register Host_Entry	    	*entry;

    if (Host_Start() == 0) {
	while (1) {
	    entry = Host_Next();
	    if (entry == (Host_Entry *) NULL) {
		return (Host_Entry *) NULL;
	    }
	    if (entry->inetAddr.s_addr == inetAddr.s_addr) {
		return (entry);
	    }
	}
    }
    return ((Host_Entry *)NULL);
}
