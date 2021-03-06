/*
 * Warning:  this file was borrowed from 4.3 BSD and appears to be a
 * derivative of AT&T code.  Do not distribute it to unlicensed
 * parties.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getgrgid.c	5.2 (Berkeley) 3/9/86";
#endif LIBC_SCCS and not lint

#include <grp.h>

struct group *
getgrgid(gid)
register gid;
{
	register struct group *p;
	struct group *getgrent();

	setgrent();
	while( (p = getgrent()) && p->gr_gid != gid );
	endgrent();
	return(p);
}
