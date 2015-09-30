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
# Tue Jul  2 14:18:31 PDT 1991
#
# For more information, refer to the mkmf manual page.
#
# $Header: /sprite/lib/mkmf/RCS/Makefile.md,v 1.6 90/03/12 23:28:42 jhh Exp $
#
# Allow mkmf

SRCS		= ds3100.md/dumpEvents.c dump.c hash.c trace.c traceLog.c
HDRS		= bf.h byte.h dump.h dumpInt.h hash.h sospRecord.h trace.h traceLog.h
MDPUBHDRS	= 
OBJS		= ds3100.md/dump.o ds3100.md/dumpEvents.o ds3100.md/hash.o ds3100.md/trace.o ds3100.md/traceLog.o
CLEANOBJS	= ds3100.md/dumpEvents.o ds3100.md/dump.o ds3100.md/hash.o ds3100.md/trace.o ds3100.md/traceLog.o
INSTFILES	= ds3100.md/md.mk ds3100.md/dependencies.mk Makefile local.mk tags TAGS
SACREDOBJS	= 