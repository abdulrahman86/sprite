#
# Prototype Makefile for machine-dependent directories.
#
# A file of this form resides in each ".md" subdirectory of a
# command.  Its name is typically "md.mk".  During makes in the
# parent directory, this file (or a similar file in a sibling
# subdirectory) is included to define machine-specific things
# such as additional source and object files.
#
# This Makefile is automatically generated.
# DO NOT EDIT IT OR YOU MAY LOSE YOUR CHANGES.
#
# Generated from /sprite/lib/mkmf/Makefile.md
# Mon Jun  8 14:25:17 PDT 1992
#
# For more information, refer to the mkmf manual page.
#
# $Header: /sprite/lib/mkmf/RCS/Makefile.md,v 1.6 90/03/12 23:28:42 jhh Exp $
#
# Allow mkmf

SRCS		= sun4.md/nlist.c getgrent.c getgrgid.c getgrnam.c getpass.c getpwent.c getwd.c ndbm.c pfs.c regex.c setegid.c seteuid.c setgid.c setrgid.c setruid.c setuid.c syslog.c utime.c a.out.c getopt.c popen.c remque.c siglist.c alarm.c ecvt.c fmt.c gcvt.c insque.c isatty.c mktemp.c signal.c ttyname.c ttyslot.c crypt.c initgroups.c iszero.c option.c panic.c pdev.c ttyDriver.c ttyPdev.c getlogin.c swapBuffer.c gtty.c getusershell.c ldexp.c shm.c stty.c pause.c sleep.c ftime.c status.c statusPrint.c frexp.c isinf.c fsDispatch.c isnan.c Misc_InvokeEditor.c Rpc_GetName.c valloc.c usleep.c
HDRS		= 
MDPUBHDRS	= 
OBJS		= sun4.md/nlist.o sun4.md/getgrent.o sun4.md/getgrgid.o sun4.md/getgrnam.o sun4.md/getpass.o sun4.md/getpwent.o sun4.md/getwd.o sun4.md/ndbm.o sun4.md/pfs.o sun4.md/regex.o sun4.md/setegid.o sun4.md/seteuid.o sun4.md/setgid.o sun4.md/setrgid.o sun4.md/setruid.o sun4.md/setuid.o sun4.md/syslog.o sun4.md/utime.o sun4.md/a.out.o sun4.md/getopt.o sun4.md/popen.o sun4.md/remque.o sun4.md/siglist.o sun4.md/alarm.o sun4.md/ecvt.o sun4.md/fmt.o sun4.md/gcvt.o sun4.md/insque.o sun4.md/isatty.o sun4.md/mktemp.o sun4.md/signal.o sun4.md/ttyname.o sun4.md/ttyslot.o sun4.md/crypt.o sun4.md/initgroups.o sun4.md/iszero.o sun4.md/option.o sun4.md/panic.o sun4.md/pdev.o sun4.md/ttyDriver.o sun4.md/ttyPdev.o sun4.md/getlogin.o sun4.md/swapBuffer.o sun4.md/gtty.o sun4.md/getusershell.o sun4.md/ldexp.o sun4.md/shm.o sun4.md/stty.o sun4.md/pause.o sun4.md/sleep.o sun4.md/ftime.o sun4.md/status.o sun4.md/statusPrint.o sun4.md/frexp.o sun4.md/isinf.o sun4.md/fsDispatch.o sun4.md/isnan.o sun4.md/Misc_InvokeEditor.o sun4.md/Rpc_GetName.o sun4.md/valloc.o sun4.md/usleep.o
CLEANOBJS	= sun4.md/nlist.o sun4.md/getgrent.o sun4.md/getgrgid.o sun4.md/getgrnam.o sun4.md/getpass.o sun4.md/getpwent.o sun4.md/getwd.o sun4.md/ndbm.o sun4.md/pfs.o sun4.md/regex.o sun4.md/setegid.o sun4.md/seteuid.o sun4.md/setgid.o sun4.md/setrgid.o sun4.md/setruid.o sun4.md/setuid.o sun4.md/syslog.o sun4.md/utime.o sun4.md/a.out.o sun4.md/getopt.o sun4.md/popen.o sun4.md/remque.o sun4.md/siglist.o sun4.md/alarm.o sun4.md/ecvt.o sun4.md/fmt.o sun4.md/gcvt.o sun4.md/insque.o sun4.md/isatty.o sun4.md/mktemp.o sun4.md/signal.o sun4.md/ttyname.o sun4.md/ttyslot.o sun4.md/crypt.o sun4.md/initgroups.o sun4.md/iszero.o sun4.md/option.o sun4.md/panic.o sun4.md/pdev.o sun4.md/ttyDriver.o sun4.md/ttyPdev.o sun4.md/getlogin.o sun4.md/swapBuffer.o sun4.md/gtty.o sun4.md/getusershell.o sun4.md/ldexp.o sun4.md/shm.o sun4.md/stty.o sun4.md/pause.o sun4.md/sleep.o sun4.md/ftime.o sun4.md/status.o sun4.md/statusPrint.o sun4.md/frexp.o sun4.md/isinf.o sun4.md/fsDispatch.o sun4.md/isnan.o sun4.md/Misc_InvokeEditor.o sun4.md/Rpc_GetName.o sun4.md/valloc.o sun4.md/usleep.o
INSTFILES	= sun4.md/md.mk sun4.md/dependencies.mk Makefile tags TAGS
SACREDOBJS	= 
