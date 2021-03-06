#ifndef lint
static char rcsid[] = "$Id: commit.c,v 1.4 91/09/10 16:11:21 jhh Exp $";
#endif !lint

/*
 *    Copyright (c) 1989, Brian Berliner
 *
 *    You may distribute under the terms of the GNU General Public License
 *    as specified in the README file that comes with the CVS 1.0 kit.
 *
 * Commit Files
 *
 *	"commit" commits the present version to the RCS repository, AFTER
 *	having done a test on conflicts.  The call is:
 *		cvs commit [options] files...
 *
 *	"commit" accepts the following options:
 *		-f		Force a commit, even if the RCS $Id string
 *				is not found
 *		-n		Causes "commit" to *not* run any commit prog
 *		-a		Commits all files in the current directory
 *				that have been modified.
 *		-m 'message'	Does not start up the editor for the
 *				log message; just gleans it from the
 *				'message' argument.
 *		-r Revision	Allows committing to a particular *numeric*
 *				revision number.
 *
 *	Note that "commit" does not do a recursive commit.  You must do
 *	"commit" in each directory where there are files that you'd
 *	like to commit.
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include "cvs.h"

static int force_commit_no_rcsid = 0;

extern int run_module_prog;

commit(argc, argv)
    int argc;
    char *argv[];
{
    int commit_all = 0, err = 0;
    char *rev = "";			/* not NULL! */
    char line[MAXLINELEN], message[MAXMESGLEN];
    int c;

    if (argc == -1)
	commit_usage();
    /*
     * For log purposes, do not allow "root" to commit files
     */
    if (geteuid() == 0)
	error(0, "cannot commit files as 'root'");
    optind = 1;
    while ((c = getopt(argc, argv, "fnam:r:qQ")) != -1) {
	switch (c) {
	case 'f':
	    force_commit_no_rcsid = 1;
	    break;
	case 'n':
	    run_module_prog = 0;
	    break;
	case 'a':
	    commit_all = 1;
	    break;
	case 'm':
	    use_editor = FALSE;
	    if (strlen(optarg) >= sizeof(message)) {
		warn(0, "warning: message too long; truncated!");
		(void) strncpy(message, optarg, sizeof(message));
		message[sizeof(message) - 1] = '\0';
	    } else
		(void) strcpy(message, optarg);
	    break;
	case 'r':
	    if (!isdigit(optarg[0]))
		error(0, "specified revision %s must be numeric!", optarg);
	    rev = optarg;
	    break;
	case 'Q':
	    really_quiet = 1;
	    /* FALL THROUGH */
	case 'q':
	    quiet = 1;
	    break;
	case '?':
	default:
	    commit_usage();
	    break;
	}
    }
    argc -= optind;
    argv += optind;
    if (!commit_all && argc == 0)
	error(0, "must specify the files you'd like to check-in");
    if (commit_all && argc != 0)
	error(0, "cannot specify files with the -a option");
    Name_Repository();
    Writer_Lock();
    if (commit_all) {
	Find_Names(&fileargc, fileargv, ALL);
	argc = fileargc;
	argv = fileargv;
    }
    if (rev[0] != '\0') {
	register int i;
	FILE *fptty;

#ifdef sprite
	fptty = open_file(getenv("TTY"), "r");
#else
	fptty = open_file("/dev/tty", "r");
#endif
	printf("WARNING:\n");
	printf("\tCommitting with a specific revision number\n");
	printf("\tbypasses all consistency checks.  Are you abosulutely\n");
	printf("\tsure you want to continue (y/n) [n] ? ");
	(void) fflush(stdout);
	if (fgets(line, sizeof(line), fptty) == NULL ||
	    (line[0] != 'y' && line[0] != 'Y')) {
	    error(0, "commit of revision %s aborted", rev);
	}
	(void) fclose(fptty);
	/*
	 * When committing with a specific revision number, we simply
	 * fudge the lists that Collect_Sets() would have created for
	 * us.  This is all so gross, but sometimes useful.
	 */
	Clist[0] = Glist[0] = Mlist[0] = Olist[0] = Dlist[0] = '\0';
	Alist[0] = Rlist[0] = Wlist[0] = Llist[0] = Blist[0] = '\0';
	for (i = 0; i < argc; i++) {
	    (void) strcat(Mlist, " ");
	    (void) strcat(Mlist, argv[i]);
	}
    } else {
	err += Collect_Sets(argc, argv);
    }
    if (err == 0) {
	err += commit_process_lists(message, rev);
	if (err == 0 && run_module_prog) {
	    char *cp;
	    FILE *fp;

	    /*
	     * It is not an error if Checkin.prog does not exist.
	     */
	    if ((fp = fopen(CVSADM_CIPROG, "r")) != NULL) {
		if (fgets(line, sizeof(line), fp) != NULL) {
		    if ((cp = rindex(line, '\n')) != NULL)
			*cp = '\0';
		    (void) sprintf(prog, "%s %s", line, Repository);
		    printf("%s %s: Executing '%s'\n", progname, command, prog);
		    (void) system(prog);
		}
		(void) fclose(fp);
	    }
	}
	Update_Logfile(Repository, message);
    }
    Lock_Cleanup(0);
    exit(err);
}

/*
 * Process all the lists, returning the number of errors found.
 */
static
commit_process_lists(message, rev)
    char *message;
    char *rev;
{
    char line[MAXLISTLEN], fname[MAXPATHLEN], revision[50];
    FILE *fp;
    char *cp;
    int first, err = 0;

    /*
     * Doesn't make much sense to commit a directory...
     */
    if (Dlist[0])
	warn(0, "committing directories ignored -%s", Dlist);
    /*
     * Is everything up-to-date?
     * Only if Glist, Olist, and Wlist are all NULL!
     */
    if (Glist[0] || Olist[0] || Wlist[0]) {
	(void) fprintf(stderr, "%s: the following files are not ", progname);
	(void) fprintf(stderr,
		       "up to date; use '%s update' first:\n", progname);
	if (Glist[0] != '\0')
	    (void) fprintf(stderr, "\t%s\n", Glist);
	if (Olist[0] != '\0')
	    (void) fprintf(stderr, "\t%s\n", Olist);
	if (Wlist[0] != '\0')
	    (void) fprintf(stderr, "\t%s\n", Wlist);
	Lock_Cleanup(0);
	exit(1);
    }
    /*
     * Is there anything to do in the first place?
     */
    if (Mlist[0] == '\0' && Rlist[0] == '\0' && Alist[0] == '\0') {
	if (!quiet) {
	    error(0, "there is nothing to commit!");
	} else {
	    Lock_Cleanup(0);
	    exit(1);
	}
    }
    /*
     * First we make sure that the file has an RCS $Id string in it
     * and if it does not, the user is prompted for verification to continue.
     */
    if (force_commit_no_rcsid == 0) {
	(void) strcpy(line, Mlist);
	(void) strcat(line, Alist);
	for (first = 1, cp = strtok(line, " \t"); cp;
	    cp = strtok((char *)NULL, " \t")) {
	    (void) sprintf(prog, "%s -s %s %s", GREP, RCSID_PAT, cp);
	    if (system(prog) != 0) {
		if (first) {
		    printf("%s %s: WARNING!\n", progname, command);
#ifndef sprite
		    printf("\tThe following file(s) do not contain an RCS $Id keyword:\n");
#else
		    printf(
    "\tThe following file(s) do not contain an RCS $Id or $Header keyword:\n");
#endif
		    first = 0;
		}
		printf("\t\t%s\n", cp);
	    }
	}
	if (first == 0) {
#ifdef sprite
	    FILE *fptty = open_file(getenv("TTY"), "r");
#else
	    FILE *fptty = open_file("/dev/tty", "r");
#endif
	    printf("\tAre you sure you want to continue (y/n) [n] ? ");
	    (void) fflush(stdout);
	    if (fgets(line, sizeof(line), fptty) == NULL ||
		(line[0] != 'y' && line[0] != 'Y')) {
		error(0, "commit aborted");
	    }
	    (void) fclose(fptty);
	}
    }
    if (use_editor)
	do_editor(message);
    /*
     * Mlist is the "modified, needs committing" list
     */
    (void) strcpy(line, Mlist);
    for (cp = strtok(line, " \t"); cp; cp = strtok((char *)NULL, " \t")) {
	(void) strcpy(User, cp);
	(void) sprintf(Rcs, "%s/%s%s", Repository, User, RCSEXT);
	if (lock_RCS(rev) != 0)
	    err++;
    }
    /*
     * Rlist is the "to be removed" list
     */
    (void) strcpy(line, Rlist);
    for (cp = strtok(line, " \t"); cp; cp = strtok((char *)NULL, " \t")) {
	(void) strcpy(User, cp);
	(void) sprintf(Rcs, "%s/%s%s", Repository, User, RCSEXT);
	if (lock_RCS(rev) != 0)
	    err++;
    }
    /*
     * Alist is the "to be added" list
     */
    (void) strcpy(line, Alist);
    for (cp = strtok(line, " \t"); cp; cp = strtok((char *)NULL, " \t")) {
	(void) strcpy(User, cp);
	(void) sprintf(Rcs, "%s/%s%s", Repository, User, RCSEXT);
	(void) sprintf(prog, "%s/%s -i -t%s/%s%s", Rcsbin, RCS, CVSADM,
		       User, CVSEXT_LOG);
	(void) sprintf(fname, "%s/%s%s", CVSADM, User, CVSEXT_OPT);
	fp = open_file(fname, "r");
	while (fgets(fname, sizeof(fname), fp) != NULL) {
	    if ((cp = rindex(fname, '\n')) != NULL)
		*cp = '\0';
	    (void) strcat(prog, " ");
	    (void) strcat(prog, fname);
	}
	(void) fclose(fp);
	(void) strcat(prog, " ");
	(void) strcat(prog, Rcs);
	if (system(prog) == 0) {
	    fix_rcs_modes(Rcs, User);
	} else {
	    warn(0, "could not create %s", Rcs);
	    err++;
	}
    }
    /*
     * If something failed, release all locks and restore the default
     * branches
     */
    if (err) {
	int didllist = 0;
	char *branch;

	for (cp = strtok(Llist, " \t"); cp; cp = strtok((char *)NULL, " \t")) {
	    didllist = 1;
	    (void) strcpy(User, cp);
	    (void) sprintf(Rcs, "%s/%s%s", Repository, User, RCSEXT);
	    (void) sprintf(prog, "%s/%s -q -u %s", Rcsbin, RCS, Rcs);
	    if (system(prog) != 0)
		warn(0, "could not UNlock %s", Rcs);
	}
	if (didllist) {
	    for (cp=strtok(Blist, " \t"); cp; cp=strtok((char *)NULL, " \t")) {
		if ((branch = rindex(cp, ':')) == NULL)
		    continue;
		*branch++ = '\0';
		(void) strcpy(User, cp);
		(void) sprintf(Rcs, "%s/%s%s", Repository, User, RCSEXT);
		(void) sprintf(prog, "%s/%s -q -b%s %s", Rcsbin, RCS,
			       branch, Rcs);
		if (system(prog) != 0)
		    warn(0, "could not restore branch %s to %s", branch, Rcs);
	    }
	}
	for (cp = strtok(Alist, " \t"); cp; cp = strtok((char *)NULL, " \t")) {
	    (void) strcpy(User, cp);
	    (void) sprintf(Rcs, "%s/%s%s", Repository, User, RCSEXT);
	    (void) unlink(Rcs);
	}
	Lock_Cleanup(0);
	exit(1);
    }
    /*
     * Got them all, now go ahead;
     * First, add the files in the Alist
     */
    if (Alist[0] != '\0') {
	int maxrev, rev;

	/* scan the entries file looking for the max revision number */
	fp = open_file(CVSADM_ENT, "r");
	maxrev = 0;
	while (fgets(line, sizeof(line), fp) != NULL) {
	    rev = atoi(line);
	    if (rev > maxrev)
		maxrev = rev;
	}
	if (maxrev == 0)
	    maxrev = 1;
	(void) fclose(fp);
	(void) sprintf(revision, "-r%d", maxrev);
	(void) strcpy(line, Alist);
	for (cp = strtok(line, " \t"); cp; cp = strtok((char *)NULL, " \t")) {
	    (void) strcpy(User, cp);
	    if (Checkin(revision, message) != 0)
		err++;
	    (void) sprintf(fname, "%s/%s%s", CVSADM, User, CVSEXT_OPT);
	    (void) unlink(fname);
	    (void) sprintf(fname, "%s/%s%s", CVSADM, User, CVSEXT_LOG);
	    (void) unlink(fname);
	}
    }
    /*
     * Everyone else uses the head as it is set in the RCS file,
     * or the revision that was specified on the command line.
     */
    if (rev[0] != '\0')
	(void) sprintf(revision, "-r%s", rev);
    else
	revision[0] = '\0';
    /*
     * Commit the user modified files in Mlist
     */
    (void) strcpy(line, Mlist);
    for (cp = strtok(line, " \t"); cp; cp = strtok((char *)NULL, " \t")) {
	(void) strcpy(User, cp);
	if (Checkin(revision, message) != 0)
	    err++;
    }
    /*
     * And remove the RCS files in Rlist, by placing it in the Attic
     */
    (void) strcpy(line, Rlist);
    for (cp = strtok(line, " \t"); cp; cp = strtok((char *)NULL, " \t")) {
	int omask;

	(void) strcpy(User, cp);
	(void) sprintf(Rcs, "%s/%s%s", Repository, User, RCSEXT);
	(void) sprintf(fname, "%s/%s", Repository, CVSATTIC);
	omask = umask(2);
	(void) mkdir(fname, 0777);
	(void) umask(omask);
	(void) sprintf(fname, "%s/%s/%s%s", Repository, CVSATTIC,
		       User, RCSEXT);
	(void) sprintf(prog, "%s/%s -u -q %s", Rcsbin, RCS, Rcs);
	if ((system(prog) == 0 && rename(Rcs, fname) != -1) ||
	    (!isreadable(Rcs) && isreadable(fname)))
	    Scratch_Entry(User);
	else
	    err++;
    }
    return (err);
}

/*
 * Attempt to place a lock on the RCS file; returns 0 if it could and
 * 1 if it couldn't.  If the RCS file currently has a branch as the head,
 * we must move the head back to the trunk before locking the file, and
 * be sure to put the branch back as the head if there are any errors.
 */
static
lock_RCS(rev)
    char *rev;
{
    char branch[50];
    int err = 0;

    branch[0] = '\0';
    /*
     * For a specified, numeric revision of the form "1" or "1.1",
     * (or when no revision is specified ""), definitely move the
     * branch to the trunk before locking the RCS file.
     *
     * The assumption is that if there is more than one revision
     * on the trunk, the head points to the trunk, not a branch...
     * and as such, it's not necessary to move the head in this case.
     */
    if (numdots(rev) < 2) {
	branch_number(Rcs, branch);
	if (branch[0] != '\0') {
	    (void) sprintf(prog, "%s/%s -q -b %s", Rcsbin, RCS, Rcs);
	    if (system(prog) != 0) {
		warn(0, "cannot change branch to default for %s", Rcs);
		return (1);
	    }
	}
	(void) sprintf(prog, "%s/%s -q -l %s", Rcsbin, RCS, Rcs);
	err = system(prog);
    } else {
	(void) sprintf(prog, "%s/%s -q -l%s %s 2>%s",
		       Rcsbin, RCS, rev, Rcs, DEVNULL);
	(void) system(prog);
    }
    if (err == 0) {
	(void) strcat(Llist, " ");
	(void) strcat(Llist, User);
	(void) strcat(Blist, " ");
	(void) strcat(Blist, User);
	if (branch[0] != '\0') {
	    (void) strcat(Blist, ":");
	    (void) strcat(Blist, branch);
	}
	return (0);
    }
    if (branch[0] != '\0') {
	(void) sprintf(prog, "%s/%s -q -b%s %s", Rcsbin, RCS, branch, Rcs);
	if (system(prog) != 0)
	    warn(0, "cannot restore branch to %s for %s", branch, Rcs);
    }
    return (1);
}

/*
 * A special function used only by lock_RCS() to determine if the current
 * head is pointed at a branch.  Returns the result in "branch" as a null
 * string if the trunk is the head, or as the branch number if the branch
 * is the head.
 */
static
branch_number(rcs, branch)
    char *rcs;
    char *branch;
{
    char line[MAXLINELEN];
    FILE *fp;
    char *cp;

    branch[0] = '\0';			/* Assume trunk is head */
    fp = open_file(rcs, "r");
    if (fgets(line, sizeof(line), fp) == NULL) {
	(void) fclose(fp);
	return;
    }
    if (fgets(line, sizeof(line), fp) == NULL) {
	(void) fclose(fp);
	return;
    }
    (void) fclose(fp);
    if (strncmp(line, RCSBRANCH, sizeof(RCSBRANCH) - 1) != 0 ||
	!isspace(line[sizeof(RCSBRANCH) - 1]) ||
	(cp = rindex(line, ';')) == NULL)
	return;
    *cp = '\0';				/* strip the ';' */
    if ((cp = rindex(line, ' ')) == NULL &&
	(cp = rindex(line, '\t')) == NULL)
	return;
    cp++;
    if (*cp == NULL)
	return;
    (void) strcpy(branch, cp);
}

/*
 * Puts a standard header on the output which is either being prepared for
 * an editor session, or being sent to a logfile program.  The modified, added,
 * and removed files are included (if any) and formatted to look pretty.
 */
static
setup_tmpfile(fp, prefix)
    FILE *fp;
    char *prefix;
{
    if (Mlist[0] != '\0') {
	(void) fprintf(fp, "%sModified Files:\n", prefix);
	fmt(fp, Mlist, prefix);
    }
    if (Alist[0] != '\0') {
	(void) fprintf(fp, "%sAdded Files:\n", prefix);
	fmt(fp, Alist, prefix);
    }
    if (Rlist[0] != '\0') {
	(void) fprintf(fp, "%sRemoved Files:\n", prefix);
	fmt(fp, Rlist, prefix);
    }
}

/*
 * Breaks the files list into reasonable sized lines to avoid line
 * wrap...  all in the name of pretty output.
 */
static
fmt(fp, instring, prefix)
    FILE *fp;
    char *instring;
    char *prefix;
{
    char line[MAXLINELEN];
    char *cp;
    int col;

    (void) strcpy(line, instring);	/* since strtok() is destructive */
    (void) fprintf(fp, "%s\t", prefix);
    col = 8;				/* assumes that prefix is < 8 chars */
    for (cp = strtok(line, " \t"); cp; cp = strtok((char *)NULL, " \t")) {
	if ((col + strlen(cp)) > 70) {
	    (void) fprintf(fp, "\n%s\t", prefix);
	    col = 8;
	}
	(void) fprintf(fp, "%s ", cp);
	col += strlen(cp) + 1;
    }
    (void) fprintf(fp, "\n%s\n", prefix);
}

/*
 * Builds a temporary file using setup_tmpfile() and invokes the user's
 * editor on the file.  The header garbage in the resultant file is then
 * stripped and the log message is stored in the "message" argument.
 */
static
do_editor(message)
    char *message;
{
    FILE *fp;
    char line[MAXLINELEN], fname[MAXPATHLEN];
    int fd;

    message[0] = '\0';
    (void) strcpy(fname, CVSTEMP);
    if ((fd = mkstemp(fname)) < 0)
	error(0, "cannot create temporary file %s", fname);
    if ((fp = fdopen(fd, "w+")) == NULL)
	error(0, "cannot create FILE * to %s", fname);
    setup_tmpfile(fp, CVSEDITPREFIX);
    (void) fprintf(fp, "%sEnter Log.  Lines beginning with '%s' are removed automatically\n",
		   CVSEDITPREFIX, CVSEDITPREFIX);
    (void) fprintf(fp, "%s----------------------------------------------------------------------\n", CVSEDITPREFIX);
    (void) fclose(fp);
    (void) sprintf(prog, "%s %s", Editor, fname);
    if (system(prog) != 0)
	warn(0, "warning: editor session failed");
    fp = open_file(fname, "r");
    while (fgets(line, sizeof(line), fp) != NULL) {
	if (strncmp(line, CVSEDITPREFIX, sizeof(CVSEDITPREFIX)-1) == 0)
	    continue;
	if ((strlen(message) + strlen(line)) >= MAXMESGLEN) {
	    warn(0, "warning: log message truncated!");
	    break;
	}
	(void) strcat(message, line);
    }
    (void) fclose(fp);
    (void) unlink(fname);
}

/*
 * Uses setup_tmpfile() to pass the updated message on directly to
 * any logfile programs that have a regular expression match for the
 * checked in directory in the source repository.  The log information
 * is fed into the specified program as standard input.
 */
Update_Logfile(repository, message)
    char *repository;
    char *message;
{
    FILE *fp_info;
    char logfile[MAXPATHLEN], title[MAXLISTLEN+MAXPATHLEN], line[MAXLINELEN];
    char path[MAXPATHLEN], default_filter[MAXLINELEN];
    char *exp, *filter, *cp, *short_repository;
    int filter_run, line_number;

    if (CVSroot == NULL) {
	warn(0, "CVSROOT variable not set; no log message will be sent");
	return;
    }
    (void) sprintf(logfile, "%s/%s", CVSroot, CVSROOTADM_LOGINFO);
    if ((fp_info = fopen(logfile, "r")) == NULL) {
	warn(0, "warning: cannot open %s", logfile);
	return;
    }
    if (CVSroot != NULL)
	(void) sprintf(path, "%s/", CVSroot);
    else
	(void) strcpy(path, REPOS_STRIP);
    if (strncmp(repository, path, strlen(path)) == 0)
	short_repository = repository + strlen(path);
    else
	short_repository = repository;
    (void) sprintf(title, "'%s%s'", short_repository, Llist);
    default_filter[0] = '\0';
    filter_run = line_number = 0;
    while (fgets(line, sizeof(line), fp_info) != NULL) {
	line_number++;
	if (line[0] == '#')
	    continue;
	for (cp = line; *cp && isspace(*cp); cp++)
	    ;
	if (*cp == '\0')
	    continue;			/* blank line */
	for (exp = cp; *cp && !isspace(*cp); cp++)
	    ;
	if (*cp != '\0')
	    *cp++ = '\0';
	while (*cp && isspace(*cp))
	    cp++;
	if (*cp == '\0') {
	    warn(0, "syntax error at line %d file %s; ignored",
		 line_number, logfile);
	    continue;
	}
	filter = cp;
	if ((cp = rindex(filter, '\n')) != NULL)
	    *cp = '\0';			/* strip the newline */
	/*
	 * At this point, exp points to the regular expression, and
	 * filter points to the program to exec.  Evaluate the regular
	 * expression against short_repository and exec the filter
	 * if it matches.
	 */
	if (strcmp(exp, "DEFAULT") == 0) {
	    (void) strcpy(default_filter, filter);
	    continue;
	}
	/*
	 * For a regular expression of "ALL", send the log message
	 * to the requested filter *without* noting that a filter was run.
	 * This allows the "DEFAULT" regular expression to be more
	 * meaningful with all updates going to a master log file.
	 */
	if (strcmp(exp, "ALL") == 0) {
	    (void) logfile_write(repository, filter, title, message);
	    continue;
	}
	if ((cp = re_comp(exp)) != NULL) {
	    warn(0, "bad regular expression at line %d file %s: %s",
		 line_number, logfile, cp);
	    continue;
	}
	if (re_exec(short_repository) == 0)
	    continue;			/* no match */
	if (logfile_write(repository, filter, title, message) == 0)
	    filter_run = 1;
    }
    if (filter_run == 0 && default_filter[0] != '\0')
	(void) logfile_write(repository, default_filter, title, message);
}

/*
 * Since some systems don't define this...
 */
#ifndef MAXHOSTNAMELEN
#define	MAXHOSTNAMELEN	64
#endif !MAXHOSTNAMELEN

/*
 * Writes some stuff to the logfile "filter" and returns the status of the
 * filter program.
 */
static
logfile_write(repository, filter, title, message)
    char *repository;
    char *filter;
    char *title;
    char *message;
{
    char cwd[MAXPATHLEN], host[MAXHOSTNAMELEN];
    FILE *fp;
    char *cp;

    /*
     * A maximum of 6 %s arguments are supported in the filter
     */
    (void) sprintf(prog, filter, title, title, title, title, title, title);
    if ((fp = popen(prog, "w")) == NULL) {
	warn(0, "cannot write entry to log filter: %s", prog);
	return (1);
    }
    if (gethostname(host, sizeof(host)) < 0)
	(void) strcpy(host, "(unknown)");
    (void) fprintf(fp, "Update of %s\n", repository);
    (void) fprintf(fp, "In directory %s:%s\n\n", host,
		   (cp = getwd(cwd)) ? cp : cwd);
    setup_tmpfile(fp, "");
    (void) fprintf(fp, "Log Message:\n%s\n", message);
    return (pclose(fp));
}

/*
 * Called when "add"ing files to the RCS respository, as it is necessary
 * to preserve the file modes in the same fashion that RCS does.  This would
 * be automatic except that we are placing the RCS ,v file very far away from
 * the user file, and I can't seem to convince RCS of the location of the
 * user file.  So we munge it here, after the ,v file has been successfully
 * initialized with "rcs -i".
 */
static
fix_rcs_modes(rcs, user)
    char *rcs;
    char *user;
{
    struct stat sb;

    if (stat(user, &sb) != -1) {
	(void) chmod(rcs, (int) sb.st_mode & ~0222);
    }
}

static
commit_usage()
{
    (void) fprintf(stderr,
	"%s %s [-fn] [-a] [-m 'message'] [-r revision] [files...]\n",
		   progname, command);
    exit(1);
}
