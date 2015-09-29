/*
 * Copyright (c) 1983, 1988 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1983, 1989 Regents of the University of California.\n\
 All rights reserved.\n";
#endif not lint

#ifndef lint
static char sccsid[] = "@(#)rlogind.c	5.15 (Berkeley) 5/23/88";
static char rcsid[] = "$Header: /sprite/src/daemons/rlogind/RCS/rlogind.c,v 1.9 92/04/22 14:28:01 kupfer Exp $ SPRITE (Berkeley)";
#endif not lint

/*
 * remote login server:
 *	remuser\0
 *	locuser\0
 *	terminal info\0
 *	data
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/file.h>

#include <netinet/in.h>

#include <dev/tty.h>
#include <errno.h>
#include <fs.h>
#include <pwd.h>
#include <signal.h>
#include <sgtty.h>
#include <stdio.h>
#include <netdb.h>
#include <syslog.h>
#include <string.h>
#include <td.h>
#include <rloginPseudoDev.h>
#include <ulog.h>

# ifndef TIOCPKT_WINDOW
# define TIOCPKT_WINDOW 0x80
# endif TIOCPKT_WINDOW

extern	int errno;
int	reapchild();
extern struct	passwd *getpwnam();
extern char	*malloc();
extern char	*getenv();

/*
 * Empty environment for child processes.
 */
char *newEnviron[1];
extern char **environ;

/*
 * Initial strings received over the network connection:
 */

#define NMAX 32
static char rusername[NMAX+1];	
static char lusername[NMAX+1];
static char termType[64];

/*
 * Information about the tty-driven pseudo-device (the end of things
 * that connects to the shell).
 */

static char *pdevName;			/* Name of pseudo-device file. */
static char *portStr;			/* Pointer to the unique number
					 * contained in pdevName (used for
					 * setting up finger info. */
static Td_Pdev pdev;			/* Token for pseudo-device for
					 * terminal. */
static Td_Terminal term;		/* Token for terminal emulator. */
static int readyForNet;			/* Non-zero means there may be
					 * chars. in the output buffer for
					 * term, waiting to be shipped over
					 * the network. */

/*
 * Information used to buffer characters between the terminal and the
 * network.
 */

#define BUFFER_SIZE 1000
static char netBuffer[BUFFER_SIZE];	/* Holds chars waiting to go out over
					 * network. */
static char *nextNetChar;		/* Pointer to first char. in netBuf
					 * that hasn't already been sent over
					 * the net. */
static int netCount = 0;		/* Numbers of chars, starting at
					 * nextNetChar, that still need to
					 * go out over the wire. */
static int netID;			/* Stream ID for network file. */
static Boolean selectOnOutput;		/* TRUE means we're waiting for the
					 * network to become ready for output.
					 */
static int weirdFlowChars = 0;		/* Non-zero means the flow-control
					 * characters aren't ^S and ^Q, so
					 * we've told the other end of the
					 * connection not to handle them
					 * locally. */

/*
 * Miscellaneous variables.
 */

static Boolean timeToQuit = FALSE;	/* TRUE means we should close down
					 * the connection. */
static int childPID;			/* Login process's PID. */

char	magic[2] = { 0377, 0377 };
char	oobdata[] = {TIOCPKT_WINDOW};

/*
 * Forward declarations to procedures defined later in this file:
 */

static int	ttyControlProc();
static void	netProc();

/*ARGSUSED*/
main(argc, argv)
	int argc;
	char **argv;
{
	int on = 1, fromlen;
	struct sockaddr_in from;

	openlog("rlogind", LOG_PID | LOG_AUTH, LOG_AUTH);
	fromlen = sizeof (from);
	if (getpeername(0, &from, &fromlen) < 0) {
		fprintf(stderr, "%s: ", argv[0]);
		perror("getpeername");
		_exit(1);
	}
	if (setsockopt(0, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof (on)) < 0) {
		syslog(LOG_WARNING, "setsockopt (SO_KEEPALIVE): %m");
	}
	doit(0, &from);
}

int	child;
int	cleanup();
int	netf;
char	*line;
extern	char	*inet_ntoa();

struct winsize win = { 0, 0, 0, 0 };


doit(f, fromp)
	int f;
	struct sockaddr_in *fromp;
{
	int i, p, t, pid, on = 1;
	register struct hostent *hp;
	struct hostent hostent;
	char c, *cPtr;
	int baud, dummy1, dummy2;
	struct sgttyb sgttyb;
	char *mach;

	alarm(60);
	read(f, &c, 1);
	if (c != 0)
		exit(1);

	/*
	 *--------------------------------------
	 * Read user/term info from the network.
	 *--------------------------------------
	 */

	getstr(f, rusername, sizeof(rusername), "remuser");
	getstr(f, lusername, sizeof(lusername), "locuser");
	getstr(f, termType, sizeof(termType), "Terminal type");
	alarm(0);
	fromp->sin_port = ntohs((u_short)fromp->sin_port);
	hp = gethostbyaddr(&fromp->sin_addr, sizeof (struct in_addr),
		fromp->sin_family);
	if (hp == 0) {
		/*
		 * Only the name is used below.
		 */
		hp = &hostent;
		hp->h_name = inet_ntoa(fromp->sin_addr);
	}
	if (fromp->sin_family != AF_INET ||
	    fromp->sin_port >= IPPORT_RESERVED ||
	    fromp->sin_port < IPPORT_RESERVED/2)
		fatal(f, "Permission denied");
	write(f, "", 1);

	/*
	 *------------------------------------------------------
	 * Terminal info = "name/baud".  Separate the two and
	 * lookup the baud rate for later use.  Note:  the baud
	 * rate is needed;  if not set, then programs like vi
	 * won't work right for rlogin-ed dialups, for example.
	 *------------------------------------------------------
	 */

	baud = B9600;
	cPtr = index (termType, '/');
	if (cPtr != 0) {
	    int i, number;
	    static char *strings[] = {
		"0", "50", "75", "110", "134", "150", "200", "300", "600",
		"1200", "1800", "2400", "4800", "9600", "EXTA", "EXTB", 0
	    };
	    static int values[] = {
		B0, B50, B75, B110, B134, B150, B200, B300, B600,
		B1200, B1800, B2400, B4800, B9600, EXTA, EXTB
	    };

	    *cPtr = 0;
	    cPtr++;
	    for (i = 0, baud = B9600; strings[i] != 0; i++) {
		if (strcmp(strings[i], cPtr) == 0) {
		    baud = values[i];
		    break;
		}
	    }
	}

	/*
	 *--------------------------------------
	 * Create the terminal pseudo-device.
	 *--------------------------------------
	 */

	pdev = Td_CreatePdev(RLOGIN_PDEV_NAME, &pdevName, &term,
		ttyControlProc, (ClientData) NULL);
	if (pdev == NULL) {
	    fatal(f, "Couldn't open pseudo-terminal");
	}
	i = sizeof(sgttyb);
	if (Td_ControlCooked(term, IOC_TTY_GETP, FMT_MY_FORMAT, 0, (char *)NULL,
		&i, (char *) &sgttyb, &dummy1, &dummy2) != 0) {
	    setSpeedError:
	    fatal(f, "Couldn't set terminal speed");
	}
	i = 0;
	sgttyb.sg_ispeed = sgttyb.sg_ospeed = baud;
	if (Td_ControlCooked(term, IOC_TTY_SETN, FMT_MY_FORMAT, sizeof(sgttyb),
		(char *) &sgttyb, &i, (char *) NULL, &dummy1, &dummy2) != 0) {
	    goto setSpeedError;
	}

	/*
	 * The pdevName is mumbleN where N is a number.  Hence, the port
	 * number, as a string, may be found as part of the pdevName after
	 * the mumble part.
	 */

	portStr = strstr(pdevName, RLOGIN_PDEV_NAME);
	if (portStr == NULL) {
	    syslog(LOG_WARNING, "bad name for pseudo-device: %s", pdevName);
	} else {
	    portStr += strlen(RLOGIN_PDEV_NAME);
	}

	/*
	 *-------------------------------------
	 * Spawn the login process.
	 *-------------------------------------
	 */

	childPID = fork();
	if (childPID < 0)
		fatalperror(f, "", errno);
	if (childPID == 0) {
		int i;
		static char *args[] =
			{"login", "-h", 0, "-R", 0, "-P", 0, 0, 0};

		t = open(pdevName, 2);
		if (t < 0)
			fatalperror(f, pdevName, errno);
		for (i = 0; i < t; i++)
			(void) close(i);
		(void) dup2(t, 0); (void) dup2(t, 1); (void) dup2(t, 2);
		(void) close(t);
		
		/*
		 * Putting together an argument list like this leaves
		 * a little to be desired.  Copy in the user name,
		 * remote host, and port number.  Check the port number
		 * to make sure it is legit; if not, don't use the user
		 * log at all (a warning message would already have been
		 * sent to the syslog).
		 */
		args[2] = hp->h_name;
		args[4] = rusername;
		if (portStr != NULL) {
		    args[6] = portStr;
		    args[7] = lusername;
		} else {
		    args[5] = "-l";
		    args[6] = lusername;
		}

		/*
		 * Set up a new, otherwise empty, environment.
		 * We need to grab $MACHINE before resetting environ, though.
		 */
		mach = getenv("MACHINE");
		
		environ = newEnviron;
		setenv("TTY", pdevName);
		setenv("TERM", termType);
		setenv("RHOST", hp->h_name);
		setenv("MACHINE", mach);
		if (rusername) {
		    setenv("RUSER", rusername);
		}
		setenv ("SPRITE_OS", "yes");
		(void) execv("/sprite/cmds.$MACHINE/login", args);
		fatalperror(2, "/sprite/cmds.$MACHINE/login",errno);
		/*NOTREACHED*/
	}
	(void) ioctl(f, FIONBIO, (char *) &on);
	(void) signal(SIGTSTP, SIG_IGN);
	(void) signal(SIGCHLD, cleanup);
	(void) setpgrp(0, 0);
	/*
	 * Must ignore SIGTTOU, otherwise we'll stop
	 * when we try and set slave pty's window shape
	 * (our controlling tty is the master pty).
	 */
	(void) signal(SIGTTOU, SIG_IGN);
	send(f, oobdata, 1, MSG_OOB);	/* indicate new rlogin */

	/*
	 *-------------------------------------
	 * Process events.
	 *-------------------------------------
	 */

	netID = f;
	selectOnOutput = FALSE;
	Fs_EventHandlerCreate(f, FS_READABLE, netProc, (ClientData) NULL);
	while (!timeToQuit) {
	    Fs_Dispatch();
	}

	signal(SIGCHLD, SIG_IGN);
	cleanup();
}

/*
 * Handle a "control" request (signaled by magic being present)
 * in the data stream.  For now, we are only willing to handle
 * window size changes.
 */
control(cp, n)
	char *cp;
	int n;
{
	struct winsize w;
	int dummy1, dummy2, count;

	if (n < 4+sizeof (w) || cp[2] != 's' || cp[3] != 's')
		return (0);
	oobdata[0] &= ~TIOCPKT_WINDOW;	/* we know he heard */
	bcopy(cp+4, (char *)&w, sizeof(w));
	w.ws_row = ntohs(w.ws_row);
	w.ws_col = ntohs(w.ws_col);
	w.ws_xpixel = ntohs(w.ws_xpixel);
	w.ws_ypixel = ntohs(w.ws_ypixel);
	count = 0;
	(void) Td_ControlCooked(term, IOC_TTY_SET_WINDOW_SIZE, FMT_MY_FORMAT,
		sizeof(w), (char *)&w, &count, (char *) NULL, &dummy1, &dummy2);
	return (4+sizeof (w));
}

/*
 * The procedure below is called when the network stream becomes ready
 * for reading or writing (as indicated by eventMask).  It passes characters
 * back and forth between the network and the pseudo-device.
 */

	/* ARGSUSED */
static void
netProc(clientData, streamID, eventMask)
    ClientData clientData;		/* Not used. */
    int streamID;			/* ID of network stream. */
    int eventMask;			/* What can be done to the network:
					 * some combination of FS_READABLE
					 * and FS_WRITABLE. */
{
    char buffer[BUFFER_SIZE+1];
    int count;

    if (eventMask & FS_READABLE) {
	count = read(netID, buffer, BUFFER_SIZE);
	if (count >= 0) {
	    register char *cp;

	    if (count == 0) {
		timeToQuit = TRUE;
		return;
	    }

	    /*
	     * Scan over input data looking for control requests
	     * (which are preceded by "magic" characters).  Send normal
	     * data to the terminal driver, and control info to a
	     * special procedure for handling.  By the way, the code below
	     * is gross, since it won't work if the control information
	     * happens to span a buffer boundary (but if it's good enough
	     * for UNIX, then I suppose it's good enough for Sprite).
	     */

	    for (cp = buffer; cp < buffer+count; cp++) {
		int	csize;
		if (cp[0] != magic[0] || (cp[1] != magic[1])) {
		    Td_PutRaw(term, 1, cp);
		    continue;
		}
		csize = control(cp, buffer+count-cp);
		if (csize > 0) {
		    cp += csize - 1;
		}
	    }
	} else if (errno != EWOULDBLOCK) {
	    fatalperror(netID, "reading from net",errno);
	}
    }

    if (eventMask & FS_WRITABLE) {

	/* 
	 * Get characters from the terminal output buffer.
	 */

	if (netCount == 0) {
	    count = Td_GetRaw(term, BUFFER_SIZE, netBuffer);
	    if (count == 0) {
		selectOnOutput = FALSE;
		Fs_EventHandlerDestroy(netID);
		Fs_EventHandlerCreate(netID, FS_READABLE, netProc,
			(ClientData) 0);
		return;
	    }
	    netCount = count;
	    nextNetChar = netBuffer;
	}

	/*
	 * Send characters to the network.
	 */

	count = write(netID, nextNetChar, netCount);
	if (count >= 0) {
	    netCount -= count;
	    nextNetChar += count;
	} else if (errno != EWOULDBLOCK) {
	    fatalperror(netID, "writing to net", errno);
	}
    }
}

/*
 * The following procedure is invoked by the terminal driver as
 * the "raw control procedure" for the terminal.  It is called, for
 * example, when output characters become ready or when the flow
 * control characters change.
 */

	/* ARGSUSED */
static int
ttyControlProc(clientData, command, inSize, inBuffer, outSize, outBuffer)
    ClientData clientData;	/* Not used. */
    int command;		/* Identifies control operation being
				 * invoked, e.g. TD_COOKED_SIGNAL. */
    int inSize;			/* Number of bytes of input data available
				 * to us. */
    char *inBuffer;		/* Pointer to input data. */
    int outSize;		/* Maximum number of bytes of output data
				 * we can return to caller. */
    char *outBuffer;		/* Area in which to store output data for
				 * caller. */
{
    Td_FlowChars *flowPtr;
    char control;

    switch (command) {
	case TD_RAW_OUTPUT_READY:
	    if (!selectOnOutput) {
		selectOnOutput = TRUE;
		Fs_EventHandlerDestroy(netID);
		Fs_EventHandlerCreate(netID, FS_READABLE|FS_WRITABLE,
			netProc, (ClientData) 0);
	    }
	    break;

	case TD_RAW_FLUSH_OUTPUT:
	    netCount = 0;
	    control = oobdata[0] | TIOCPKT_FLUSHWRITE;
	    send(netID, &control, 1, MSG_OOB);
	    break;

	case TD_RAW_FLOW_CHARS:
	    flowPtr = (Td_FlowChars *) inBuffer;
	    if ((flowPtr->stop == CTRL('s'))
		    && (flowPtr->start == CTRL('q'))) {
		if (weirdFlowChars) {
		    control = oobdata[0] | TIOCPKT_DOSTOP;
		    send(netID, &control, 1, MSG_OOB);
		    weirdFlowChars = 0;
		}
	    } else {
		if (!weirdFlowChars) {
		    control = oobdata[0] | TIOCPKT_NOSTOP;
		    send(netID, &control, 1, MSG_OOB);
		    weirdFlowChars = 1;
		}
	    }
	    break;
    }
    return 0;
}

cleanup()
{
    int status;
    int portID;

	/* vhangup();		 No hangups in Sprite yet. */
	if (childPID != 0) {	/* Partial compensation of lack of vhangup. */	
	    status = kill(childPID, SIGHUP);
	    /*
	     * It would be nice to make the reaper process be able to
	     * clean up after logins, but there's no easy way for it
	     * to determine the "port number" that we've found out
	     * here.  Record the logout here, if we succeed in killing
	     * the child login process.  If the kill fails, then
	     * the child likely already exited and recorded the logout, so
	     * don't record it again (just in case the port has already
	     * been recycled).
	     *
	     * Note: the ULog_RecordLogout routine doesn't use the
	     * userID passed into it yet, and this program really
	     * can't say who eventually logs in (only the login
	     * program knows for sure).  Therefore, there's no point
	     * in getting the userID from the user name.  Portid had
	     * better be non-zero since we're using port 0 for the
	     * console.
	     */
	    if (status == 0) {
		portID = atoi(portStr);
		if (portID <= 0 || portID >= ULOG_MAX_PORTS) {
		    syslog(LOG_ERR, "error converting rlogin port number (%s)",
			   portStr);
		} else {
		    Ulog_RecordLogout(NULL, portID);
		}
	    }
	}
	shutdown(netf, 2);
	exit(1);
}

fatal(f, msg)
	int f;
	char *msg;
{
	char buf[BUFSIZ];

	buf[0] = '\01';		/* error indicator */
	(void) sprintf(buf + 1, "rlogind: %s.\r\n", msg);
	(void) write(f, buf, strlen(buf));
	exit(1);
}

fatalperror(f, msg)
	int f;
	char *msg;
{
	char buf[BUFSIZ];
	extern int sys_nerr;
	extern char *sys_errlist[];

	if ((unsigned)errno < sys_nerr)
		(void) sprintf(buf, "%s: %s", msg, sys_errlist[errno]);
	else
		(void) sprintf(buf, "%s: Error %d", msg, errno);
	fatal(f, buf);
}

getstr(f, buf, cnt, err)
	int f;
	char *buf;
	int cnt;
	char *err;
{
	char c;

	do {
		if (read(f, &c, 1) != 1)
			exit(1);
		if (--cnt < 0) {
			(void) fprintf(stderr, "%s too long\r\n", err);
			exit(1);
		}
		*buf++ = c;
	} while (c != 0);
}
