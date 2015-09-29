/*
 * jgetInt.h --
 *
 *	Declarations for use by the jls utility for tapestry archive system
 *
 * Copyright 1991 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that this copyright
 * notice appears in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /sprite/lib/forms/RCS/jgetInt.h,v 1.0 91/02/09 13:24:52 mottsmth Exp $ SPRITE (Berkeley)
 */

#ifndef _JGETINT
#define _JGETINT

#define MAXARG 100

#define DEF_RANGE ""
#define DEF_ASOF ""
#define DEF_SINCE ""
#define DEF_ABS ""
#define DEF_OWNER ""
#define DEF_GROUP ""
#define DEF_MAIL ""
#define DEF_RECURSE INT_MAX
#define DEF_MODDATE 0
#define DEF_ALL 0
#define DEF_FIRSTVER -1
#define DEF_LASTVER -1
#define DEF_FIRSTDATE -1
#define DEF_LASTDATE -1
#define DEF_CLOBBER 0
#define DEF_VERBOSE 0
#define DEF_PERM 0600
#define DEF_PERMDIR 0700
#define DEF_TARGET ""
#define DEF_FILTER 0
#define DEF_TAR 0
#define DEF_TARBUF 10240

typedef struct parmTag {
    char *server;
    int port;
    char *arch;
    char *range;
    char *asof;
    char *since;
    char *abs;
    char *owner;
    char *group;
    char *mail;
    int recurse;
    int modDate;
    int all;
    int firstVer;
    int lastVer;
    int firstDate;
    int lastDate;
    int clobber;
    int verbose;
    char *target;
    int filter;
    int tar;
    int tarBuf;
} Parms;

#endif /* _JGETINT */
