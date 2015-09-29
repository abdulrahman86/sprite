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
# Mon Jun  8 14:36:32 PDT 1992
#
# For more information, refer to the mkmf manual page.
#
# $Header: /sprite/lib/mkmf/RCS/Makefile.md,v 1.6 90/03/12 23:28:42 jhh Exp $
#
# Allow mkmf

SRCS		= symm.md/Sync_Unlock.s symm.md/Sync_GetLock.s Sync_SlowLock.c Sync_SlowWait.c
HDRS		= 
MDPUBHDRS	= 
OBJS		= symm.md/Sync_Unlock.o symm.md/Sync_GetLock.o symm.md/Sync_SlowLock.o symm.md/Sync_SlowWait.o
CLEANOBJS	= symm.md/Sync_Unlock.o symm.md/Sync_GetLock.o symm.md/Sync_SlowLock.o symm.md/Sync_SlowWait.o
INSTFILES	= symm.md/md.mk symm.md/dependencies.mk Makefile
SACREDOBJS	= 
