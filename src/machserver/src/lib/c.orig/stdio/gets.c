/* 
 * gets.c --
 *
 *	Source code for the "gets" library procedure.
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
static char rcsid[] = "$Header: gets.c,v 1.1 88/06/10 16:23:54 ouster Exp $ SPRITE (Berkeley)";
#endif not lint

#include "stdio.h"

/*
 *----------------------------------------------------------------------
 *
 * gets --
 *
 *	Read a line from stdin.
 *
 * Results:
 *	Characters are read from stdin and placed at buf until a
 *	newline is encountered or an end of file or error is encountered.
 *	The newline is read and discarded, and the string at buf is left
 *	null-terminated.  The return value is a pointer to buf if
 *	all went well, or NULL if an end of file or error was encountered.
 *
 * Side effects:
 *	Characters are removed from stream.
 *
 *----------------------------------------------------------------------
 */

char *
gets(bufferPtr)
    char *bufferPtr;		/* Where to place characters. */
{
    register char *destPtr = bufferPtr;
    register int c;
    register FILE *stream = stdin;

    while (1) {
	c = getc(stream);
	if (c == EOF) {
	    *destPtr = 0;
	    return NULL;
	}
	if (c == '\n') {
	    break;
	}
	*destPtr = c;
	destPtr++;
    }
    *destPtr = 0;
    return bufferPtr;
}
