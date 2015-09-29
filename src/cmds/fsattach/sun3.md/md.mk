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
# Mon Dec 14 17:37:52 PST 1992
#
# For more information, refer to the mkmf manual page.
#
# $Header: /sprite/lib/mkmf/RCS/Makefile.md,v 1.6 90/03/12 23:28:42 jhh Exp $
#
# Allow mkmf

SRCS		= fsattach.c exec.c misc.c
HDRS		= fsattach.h
MDPUBHDRS	= 
OBJS		= sun3.md/exec.o sun3.md/fsattach.o sun3.md/misc.o
CLEANOBJS	= sun3.md/fsattach.o sun3.md/exec.o sun3.md/misc.o
INSTFILES	= sun3.md/md.mk sun3.md/dependencies.mk Makefile local.mk tags
SACREDOBJS	= 
