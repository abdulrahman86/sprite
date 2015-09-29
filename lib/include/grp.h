/*	grp.h	4.1	83/05/03	*/

#ifndef _GRP
#define _GRP

struct	group { /* see getgrent(3) */
	char	*gr_name;
	char	*gr_passwd;
	int	gr_gid;
	char	**gr_mem;
};

struct group *getgrent(), *getgrgid(), *getgrnam();

#endif /* _GRP */
