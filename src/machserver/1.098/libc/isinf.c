/* 
 * isinf.c --
 *
 *	Machine-dependent procedure to determine whether a double is 
 *	infinity.
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
static char rcsid[] = "$Header: /sprite/src/lib/c/etc/RCS/isinf.c,v 1.1 90/11/02 07:44:01 rab Exp Locker: rab $ SPRITE (Berkeley)";
#endif /* not lint */

#include <math.h>
#include <machparam.h>

#if BYTE_ORDER==BIG_ENDIAN     
#define MSW 0
#define LSW 1
#endif
#if BYTE_ORDER==LITTLE_ENDIAN
#define MSW 1
#define LSW 0
#endif


/*
 *----------------------------------------------------------------------
 *
 * isnan --
 *
 *	Return whether a double is equivalent to infinity.
 *
 * Results:
 *	1 if the number is infinity, 0 otherwise.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
isinf(value)
    double value;
{
    union {
	double d;
	long l[2];
    } u;

    /*
     * Put the value into a union so we can check out the bits.
     */
    u.d = value;


    /*
     * An IEEE Std 754 double precision floating point number
     * has the following format:
     *
     *      1  bit       -- sign of Mantissa
     *      11 bits      -- exponent
     *      52 bits      -- Mantissa
     *
     * If the exponent has all bits set, the value is not a 
     * real number.
     *
     * If the Mantissa is zero then the value is infinity, which
     * is the result of division by zero, or overflow.
     *
     * If the Mantissa is non-zero the value is not a number (NaN).
     * NaN can be generated by dividing zero by itself, taking the
     * logarithm of a negative number, etc.
     */

    /*
     * check the exponent
     */
    if ((u.l[MSW] & 0x7ff00000) == 0x7ff00000) {
	/*
	 * See if the Mantissa is zero.
	 */
	if ((u.l[MSW] & ~0xfff00000) == 0 && u.l[LSW] == 0) {
	    /*
	     * Infinity.
	     */
	    return (1);
	} else {
	    /*
	     * NaN.
	     */
	    return(0);
	}
    } else {
	/*
	 * Normal.
	 */
	return (0);
    }
}

