/* 
 * Mig_GetPdevName.c --
 *
 *	This file contains the Mig_GetPdevName procedure, which
 *	returns the string corresponding to the pseudo-device
 *	used for either the global server or the local host.
 *
 * Copyright 1990 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /sprite/src/lib/c/mig/RCS/Mig_GetPdevName.c,v 2.0 90/03/10 13:13:02 douglis Stable $ SPRITE (Berkeley)";
#endif not lint

#include <fs.h>
#include <stdio.h>
#include <mig.h>

extern char *strcpy();

/*
 * Define the pseudo-devices used to communicate with the local
 * and global migration daemons.  LOCAL_PDEV is used to form
 * a string based on the hostname.
 */
#ifndef GLOBAL_PDEV
#define GLOBAL_PDEV "/sprite/admin/migd/pdev"
#endif

#ifndef LOCAL_PDEV
#define LOCAL_PDEV "/hosts/%s/migd.pdev"
#endif

static char *globalPdev = GLOBAL_PDEV;
static char *localPdev  = NULL;


/*
 *----------------------------------------------------------------------
 *
 * Mig_GetPdevName --
 *
 *	Return the name of the specified pseudo-device.  If global
 *	is non-zero, then return the name of the global pdev, else
 *	the one for this host.
 *
 * Results:
 *	A pointer to the name is returned. If an error is returned
 *	when getting the host name, NULL is returned.
 *
 * Side effects:
 *	Memory is allocated to hold the file name.
 *
 *----------------------------------------------------------------------
 */
char *
Mig_GetPdevName(global)
    int global;			/* Whether to return the global pdev name. */
{
    char hostName[FS_MAX_NAME_LENGTH];
    char fileName[FS_MAX_NAME_LENGTH];
    int status;
    extern char *malloc();

    if (global) {
	return(globalPdev);
    } 

    if (!localPdev) {
	status = gethostname(hostName, FS_MAX_NAME_LENGTH);
	if (status != 0) {
	    return((char *) NULL);
	}
	(void) sprintf(fileName, LOCAL_PDEV, hostName);
	localPdev = malloc((unsigned) (strlen(fileName) + 1));
	if (localPdev == (char *) NULL) {
	    return(localPdev);
	}
	(void) strcpy(localPdev, fileName);
    }
    return(localPdev);
}
