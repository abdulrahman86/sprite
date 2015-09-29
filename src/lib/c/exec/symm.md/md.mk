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
# Mon Jun  8 14:26:17 PDT 1992
#
# For more information, refer to the mkmf manual page.
#
# $Header: /sprite/lib/mkmf/RCS/Makefile.md,v 1.6 90/03/12 23:28:42 jhh Exp $
#
# Allow mkmf

SRCS		= _ExecArgs.c execl.c execle.c execlp.c execv.c execvp.c
HDRS		= 
MDPUBHDRS	= 
OBJS		= symm.md/_ExecArgs.o symm.md/execl.o symm.md/execle.o symm.md/execlp.o symm.md/execv.o symm.md/execvp.o
CLEANOBJS	= symm.md/_ExecArgs.o symm.md/execl.o symm.md/execle.o symm.md/execlp.o symm.md/execv.o symm.md/execvp.o
INSTFILES	= symm.md/md.mk symm.md/dependencies.mk Makefile
SACREDOBJS	= 
