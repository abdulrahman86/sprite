/*
 * sigMach.h --
 *
 *     Machine dependent data structures and procedure headers exported by the
 *     the signal module.  These are are for the 68010 hardware.
 *
 * Copyright (C) 1985, 1988 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /sprite/src/lib/include/sun3.md/RCS/sigMach.h,v 1.2 89/06/16 13:17:27 rab Exp $ SPRITE (Berkeley)
 */

#ifndef _SIGMACH
#define _SIGMACH

/*
 * The different machine dependent codes for an illegal instruction signal
 */

#define	SIG_TRAPV		4
#define	SIG_CHK			5
#define	SIG_EMU1010		6
#define	SIG_EMU1111		7


#ifdef sun3

/*
 * Machine dependent codes for mc68881 floating point exception signals
 */

#define SIG_FP_UNORDERED_COND  48
#define SIG_FP_INEXACT_RESULT  49
#define SIG_FP_ZERO_DIV        50
#define SIG_FP_UNDERFLOW       51
#define SIG_FP_OPERAND_ERROR   52
#define SIG_FP_OVERFLOW        53
#define SIG_FP_NAN             54
#endif

#endif /* _SIGMACH */
