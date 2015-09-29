/* 
 * Host_ByName.c --
 *
 *	Source code for the Host_ByName library procedure.
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
static char rcsid[] = "$Header: Host_ByName.c,v 1.1 88/06/30 11:06:43 ouster Exp $ SPRITE (Berkeley)";
#endif not lint

#include <stdio.h>
#include <host.h>
#include <hostInt.h>
#include <string.h>


/*
 *-----------------------------------------------------------------------
 *
 * Host_ByName --
 *
 *	Return information about a host based on its name. The name may
 *	be either the official name of a host or an alias for it.
 *
 * Results:
 *	A Host_Entry pointer describing the host, or NULL if no such host
 *	exists in the current database.  The Host_Entry is statically
 *	allocated, and may be modified on the next call to any Host_
 *	procedure.
 *
 * Side Effects:
 *	The host file is opened if it wasn't already.
 *
 *-----------------------------------------------------------------------
 */

Host_Entry *
Host_ByName(name)
    register char 	*name;      /* Name of host to find */
{
    register Host_Entry	*entry;	    /* Current entry */
    register char 	**cpp;	    /* Pointer to alias table */

    if (Host_Start() != 0) {
	return ((Host_Entry *)NULL);
    }
    
    while (1) {
	entry = Host_Next();
	if (entry != (Host_Entry *)NULL) {
	    if (strcmp(entry->name, name) == 0) {
		return (entry);
	    } else {
		for (cpp = entry->aliases; *cpp != (char *)NULL; cpp++) {
		    if (strcmp(*cpp, name) == 0) {
			return (entry);
		    }
		}
	    }
	} else {
	    return (Host_Entry *) NULL;
	}
    }
}
