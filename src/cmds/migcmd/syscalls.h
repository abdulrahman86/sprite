/*
 * syscalls.h --
 *
 *	Declaration of the structure used by some user programs to print
 *	information about system call use.
 *
 * Copyright 1988 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Syscalls: proto.h,v 1.2 88/03/11 08:39:40 ouster Exp $ SPRITE (Berkeley)
 */

#ifndef _SYSCALLS
#define _SYSCALLS

/*
 * List of system calls to print out, corresponding to call number.
 * Whether or not they're local should be taken from some other
 * file in the kernel rather than repeated here....
 */

typedef struct {
    char *name;
    int local;
} SysCallInfo;

extern SysCallInfo sysCallArray[];

/*
 * sizeof won't work in a different file from where the array is allocated,
 * so we declare a variable for it in the same file.
 */
extern int sysCallArraySize;

#endif _SYSCALLS

