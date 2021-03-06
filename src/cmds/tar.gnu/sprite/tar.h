/*

	Copyright (C) 1988 Free Software Foundation

GNU tar is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GNU tar General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GNU tar,
but only under the conditions described in the GNU tar General Public
License.  A copy of this license is supposed to have been given to you
along with GNU tar so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GNU tar, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

/*
 * Header file for tar (tape archive) program.
 *
 * @(#)tar.h 1.24 87/11/06
 *
 * Created 25 August 1985 by John Gilmore, ihnp4!hoptoad!gnu.
 */

#ifdef sprite
#define DEFBLOCKING 8
#define ALLOW_NO_RECURSE
#define ALLOW_LONG_NAMES
#endif

/*
 * Kludge for handling systems that can't cope with multiple
 * external definitions of a variable.  In ONE routine (tar.c),
 * we #define TAR_EXTERN to null; here, we set it to "extern" if
 * it is not already set.
 */
#ifndef TAR_EXTERN
#define TAR_EXTERN extern
#endif

#ifdef USG
typedef int size_t;
#endif

/*
 * Header block on tape.
 *
 * I'm going to use traditional DP naming conventions here.
 * A "block" is a big chunk of stuff that we do I/O on.
 * A "record" is a piece of info that we care about.
 * Typically many "record"s fit into a "block".
 */
#define	RECORDSIZE	512
#define	NAMSIZ		100
#define	TUNMLEN		32
#define	TGNMLEN		32
#define SPARSE_EXT_HDR  21
#define SPARSE_IN_HDR	4

struct sparse {
	char offset[12];
	char numbytes[12];
};

struct sp_array {
	int offset;
	int numbytes;
};

#define HDR_MAGIC 0xa3

union record {
	char		charptr[RECORDSIZE];
	struct header {
		char	name[NAMSIZ];
		char	mode[8];
		char	uid[8];
		char	gid[8];
		char	size[12];
		char	mtime[12];
		char	chksum[8];
		char	linkflag;
		char	linkname[NAMSIZ];
		char	magic[8];
		char	uname[TUNMLEN];
		char	gname[TGNMLEN];
		char	devmajor[8];
		char	devminor[8];
		/* these following fields were added by JF for gnu */
		/* and are NOT standard */
		char	atime[12];
		char	ctime[12];
		char	offset[12];
		char	longnames[4];
		struct	sparse sp[SPARSE_IN_HDR];
		char	isextended;
		char	realsize[12];	     /* true size of the sparse file */
		unsigned char hdr_magic;
/*		char	ending_blanks[12];*/ /* number of nulls at the
						   end of the file, if any */
	} header;
	struct extended_header {
		unsigned char xh_isextended; /* non-zero -> continued in
					      * next record */
		unsigned char xh_type;
		unsigned char xh_magic;
		union {
		    struct sparse _xh_sp[21];
		    /* 
		     * The buffer is only 508 characters so that the string 
		     * stored in it will be null-terminated
		     */
		    char _xh_namebuf[508];
		} u;
	} ext_hdr;
};

#define xh_sp           u._xh_sp
#define xh_namebuf      u._xh_namebuf

/* 
 * Non-ASCII magic number to distinguish a header with GNU extensions from 
 * a Posix header with a prefixed name.  (A Posix header would have either 
 * an ASCII character or a null.) (XXX what about 8-bit character sets?)
 */
#define XH_MAGIC        0xab

/* Flags telling what types of extended header records follow. */
#define XH_FILENAME     1
#define XH_LINKNAME     2
#define XH_SPARSE_FILE  4

/* The checksum field is filled with this while the checksum is computed. */
#define	CHKBLANKS	"        "	/* 8 blanks, no null */

/* The magic field is filled with this if uname and gname are valid. */
#define	TMAGIC		"ustar  "	/* 7 chars and a null */

/* The linkflag defines the type of file */
#define	LF_OLDNORMAL	'\0'		/* Normal disk file, Unix compat */
#define	LF_NORMAL	'0'		/* Normal disk file */
#define	LF_LINK		'1'		/* Link to previously dumped file */
#define	LF_SYMLINK	'2'		/* Symbolic link */
#define	LF_CHR		'3'		/* Character special file */
#define	LF_BLK		'4'		/* Block special file */
#define	LF_DIR		'5'		/* Directory */
#define	LF_FIFO		'6'		/* FIFO special file */
#define	LF_CONTIG	'7'		/* Contiguous file */
/* Further link types may be defined later. */
#ifdef sprite
#define LF_RMTLINK	'8'		/* Remote link, kind of symbolic link */
#define LF_PSEUDODEV	'9'		/* Pseudo-device (for server process) */
#endif /* sprite */

#define LF_DUMPDIR	'D'		/* This is a dir entry that contains
					   the names of files that were in
					   the dir at the time the dump
					   was made */
#define LF_MULTIVOL	'M'		/* This is the continuation
					   of a file that began on another
					   volume */
#define LF_SPARSE	'S'		/* This is for sparse files */
#define LF_VOLHDR	'V'		/* This file is a tape/volume header */
					/* Ignore it on extraction */

/*
 * Exit codes from the "tar" program
 */
#define	EX_SUCCESS	0		/* success! */
#define	EX_ARGSBAD	1		/* invalid args */
#define	EX_BADFILE	2		/* invalid filename */
#define	EX_BADARCH	3		/* bad archive */
#define	EX_SYSTEM	4		/* system gave unexpected error */


/*
 * Global variables
 */
TAR_EXTERN union record	*ar_block;	/* Start of block of archive */
TAR_EXTERN union record	*ar_record;	/* Current record of archive */
TAR_EXTERN union record	*ar_last;	/* Last+1 record of archive block */
TAR_EXTERN char		ar_reading;	/* 0 writing, !0 reading archive */
TAR_EXTERN int		blocking;	/* Size of each block, in records */
TAR_EXTERN int		blocksize;	/* Size of each block, in bytes */
TAR_EXTERN char		*ar_file;	/* File containing archive */
TAR_EXTERN char		*info_script;	/* Script to run at end of each tape change */
TAR_EXTERN char		*name_file;	/* File containing names to work on */
TAR_EXTERN char		*tar;		/* Name of this program */
TAR_EXTERN struct sp_array *sparsearray;/* Pointer to the start of the scratch space */
TAR_EXTERN int		sp_array_size;	/* Initial size of the sparsearray */
TAR_EXTERN char		*current_filename; /* file name for current header */
TAR_EXTERN char		*current_linkname; /* link name for current header */

/*
 * Flags from the command line
 */
TAR_EXTERN int cmd_mode;
#define CMD_NONE	0
#define CMD_CAT		1		/* -A */
#define CMD_CREATE	2		/* -c */
#define CMD_DIFF	3		/* -d */
#define CMD_APPEND	4		/* -r */
#define CMD_LIST	5		/* -t */
#define CMD_UPDATE	6		/* -u */
#define CMD_EXTRACT	7		/* -x */
#define CMD_DELETE	8		/* -D */

					/* -[0-9][lmh] */
			/* CMD_CAT	   -A */
					/* -b */
TAR_EXTERN int	f_reblock;		/* -B */
			/* CMD_CREATE	   -c */
					/* -C */
			/* CMD_DIFF	   -d */
/* TAR_EXTERN char	f_dironly;	/* -D */
TAR_EXTERN int	f_debug;        	/* -e */
					/* -f */
TAR_EXTERN int	f_run_script_at_end;	/* -F */
TAR_EXTERN int 	f_gnudump;		/* -G */
TAR_EXTERN int	f_follow_links;		/* -h */
TAR_EXTERN int	f_ignorez;		/* -i */
			/* CMD_DELETE	   -J */
TAR_EXTERN int	f_keep;			/* -k */
TAR_EXTERN int	f_startfile;		/* -K */
TAR_EXTERN int	f_local_filesys;	/* -l */
TAR_EXTERN int	f_long_names;		/* -L */
TAR_EXTERN int	f_modified;		/* -m */
TAR_EXTERN int 	f_multivol;		/* -M */
TAR_EXTERN int  f_no_recurse;           /* -n */
TAR_EXTERN int	f_new_files;		/* -N */
TAR_EXTERN int	f_oldarch;		/* -o */
TAR_EXTERN int  f_exstdout;		/* -O */
TAR_EXTERN int	f_use_protection;	/* -p */
TAR_EXTERN int  f_absolute_paths;	/* -P */
TAR_EXTERN int	f_sayblock;		/* -R */
TAR_EXTERN int	f_sorted_names;		/* -s */
TAR_EXTERN int	f_sparse_files;		/* -S  ... JK */
TAR_EXTERN int	f_namefile;		/* -T */
			/* CMD_UPDATE	   -u */
TAR_EXTERN int	f_verbose;		/* -v */
TAR_EXTERN char *f_volhdr;		/* -V */
TAR_EXTERN int  f_confirm;		/* -w */
TAR_EXTERN int  f_verify;		/* -W */
			/* CMD_EXTRACT     -x */
TAR_EXTERN int  f_exclude;		/* -X */
TAR_EXTERN int 	f_compress;		/* -z */
					/* -Z */

/*
 * We now default to Unix Standard format rather than 4.2BSD tar format.
 * The code can actually produce all three:
 *	f_standard	ANSI standard
 *	f_oldarch	V7
 *	neither		4.2BSD
 * but we don't bother, since 4.2BSD can read ANSI standard format anyway.
 * The only advantage to the "neither" option is that we can cmp our
 * output to the output of 4.2BSD tar, for debugging.
 */
#define		f_standard		(!f_oldarch)

/*
 * Structure for keeping track of filenames and lists thereof.
 */
struct name {
	struct name	*next;
	short		length;		/* cached strlen(name) */
	char		found;		/* A matching file has been found */
	char		firstch;	/* First char is literally matched */
	char		regexp;		/* This name is a regexp, not literal */
	char		*change_dir;	/* JF set with the -C option */
	char		*dir_contents;	/* JF for f_gnudump */
	char		name[NAMSIZ+1];
};

TAR_EXTERN struct name	*namelist;	/* Points to first name in list */
TAR_EXTERN struct name	*namelast;	/* Points to last name in list */

TAR_EXTERN int		archive;	/* File descriptor for archive file */
TAR_EXTERN int		errors;		/* # of files in error */

/*
 *
 * Due to the next struct declaration, each routine that includes
 * "tar.h" must also include <sys/types.h>.  I tried to make it automatic,
 * but System V has no defines in <sys/types.h>, so there is no way of
 * knowing when it has been included.  In addition, it cannot be included
 * twice, but must be included exactly once.  Argghh!
 *
 * Thanks, typedef.  Thanks, USG.
 */
struct link {
	struct link	*next;
	dev_t		dev;
	ino_t		ino;
	short		linkcount;
	char		name[NAMSIZ+1];
};

TAR_EXTERN struct link	*linklist;	/* Points to first link in list */


/*
 * Error recovery stuff
 */
TAR_EXTERN char		read_error_flag;


/*
 * Declarations of functions available to the world.
 */
union record *findrec();
void userec();
union record *endofrecs();
void anno();
char *extended_header_type_name();
void get_names();

/* Do not prototype these for BSD--see port.c [DOPRNT_MSG].  */
#if defined(__STDC__) && (!defined(BSD42) || defined(sprite))
void msg(char *, ...);
void msg_perror(char *, ...);
#else
void msg();
void msg_perror();
#endif
/* #define	 annorec(stream, msg)	anno(stream, msg, 0)	/* Cur rec */
/* #define	annofile(stream, msg)	anno(stream, msg, 1)	/* Saved rec */
