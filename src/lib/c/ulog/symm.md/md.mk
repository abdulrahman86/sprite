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
# Mon Jun  8 14:38:40 PDT 1992
#
# For more information, refer to the mkmf manual page.
#
# $Header: /sprite/lib/mkmf/RCS/Makefile.md,v 1.6 90/03/12 23:28:42 jhh Exp $
#
# Allow mkmf

SRCS		= Ulog_GetAllLogins.c Ulog_LastLogin.c Ulog_RecordLogout.c Ulog_ClearLogins.c Ulog_RecordLogin.c
HDRS		= ulogInt.h
MDPUBHDRS	= 
OBJS		= symm.md/Ulog_GetAllLogins.o symm.md/Ulog_LastLogin.o symm.md/Ulog_RecordLogout.o symm.md/Ulog_ClearLogins.o symm.md/Ulog_RecordLogin.o
CLEANOBJS	= symm.md/Ulog_GetAllLogins.o symm.md/Ulog_LastLogin.o symm.md/Ulog_RecordLogout.o symm.md/Ulog_ClearLogins.o symm.md/Ulog_RecordLogin.o
INSTFILES	= symm.md/md.mk symm.md/dependencies.mk Makefile
SACREDOBJS	= 
