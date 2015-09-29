/* 
 * spurPrint.c --
 *
 *	Contains the machine specific routine for printing the size if the
 *	machine is a spur.
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
static char rcsid[] = "$Header: /a/newcmds/size/RCS/printSpur.c,v 1.1 89/05/16 23:52:25 jhh Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include <spur.md/sys/exec.h>
#include <spur.md/a.out.h>
#include "size.h"


/*
 *----------------------------------------------------------------------
 *
 * PrintSpur --
 *
 *	Prints out the size information for a spur a.out file.
 *
 * Results:
 *	SUCCESS if size information was printed, FAILURE otherwise.
 *
 * Side effects:
 *	Stuff is printed.
 *
 *----------------------------------------------------------------------
 */

ReturnStatus
PrintSpur(fp, printName, fileName, printHeadings, bufferSize, buffer)
    FILE	*fp;		/* file that header was read from */
    Boolean	printName;	/* TRUE => print name of file */
    char	*fileName;	/* name of file */
    Boolean	printHeadings;	/* TRUE => print column headings */
    int		bufferSize;	/* size of buffer */
    char	*buffer;	/* buffer containing header */
{

    struct exec		*header;
    char		swappedHeader[sizeof(*header) * 2];
    int			swappedSize = sizeof(swappedHeader);
    int			status;

    if (bufferSize < sizeof(struct exec)) {
	return FAILURE;
    }
    header = (struct exec *) buffer;
    if (!N_BADMAG(*header)) {
	goto doPrint;
    }
    if (FMT_SPUR_FORMAT != hostFmt) {
	status = Fmt_Convert("{w12}", FMT_SPUR_FORMAT, &bufferSize, buffer,
			hostFmt, &swappedSize, swappedHeader);
	if (status) {
	    fprintf(stderr, "Fmt_Convert returned %d.\n", status);
	    return FAILURE;
	}
	header = (struct exec *) swappedHeader;
	if (!N_BADMAG(*header)) {
	    goto doPrint;
	}
    }
    if (FMT_68K_FORMAT != hostFmt) {
	status = Fmt_Convert("{w12}", FMT_68K_FORMAT, &bufferSize, buffer,
			hostFmt, &swappedSize, swappedHeader);
	if (status) {
	    fprintf(stderr, "Fmt_Convert returned %d.\n", status);
	    return FAILURE;
	}
	header = (struct exec *) swappedHeader;
	printf("spur magic 0x%x\n", header->a_magic);
	if (!N_BADMAG(*header)) {
	    goto doPrint;
	}
    }
    return FAILURE;
doPrint:
    if (printHeadings) {
	printf("%-7s %-7s %-7s %-7s %-7s %-7s %-7s\n", 
		"text", "data", "bss", "sdata", "sbss", "dec", "hex");
    }
    printf("%-7d %-7d %-7d %-7d %-7d %-7d %-7x",
		   header->a_text, header->a_data, header->a_bss,
		   header->a_sdata, header->a_sbss,
		   header->a_text + header->a_data + header->a_bss +
		   header->a_sbss + header->a_sdata,
		   header->a_text + header->a_data + header->a_bss +
		   header->a_sbss + header->a_sdata);
    if (printName) {
	printf("\t%s\n", fileName);
    } else {
	printf("\n");
    }
    return SUCCESS;
}
