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
# Mon Jun  8 14:32:29 PDT 1992
#
# For more information, refer to the mkmf manual page.
#
# $Header: /sprite/lib/mkmf/RCS/Makefile.md,v 1.6 90/03/12 23:28:42 jhh Exp $
#
# Allow mkmf

SRCS		= Quad_AddUns.c Quad_AddUnsLong.c Quad_CompareUns.c Quad_PutUns.c Quad_UnsToDouble.c
HDRS		= 
MDPUBHDRS	= 
OBJS		= ds3100.md/Quad_AddUns.o ds3100.md/Quad_AddUnsLong.o ds3100.md/Quad_CompareUns.o ds3100.md/Quad_PutUns.o ds3100.md/Quad_UnsToDouble.o
CLEANOBJS	= ds3100.md/Quad_AddUns.o ds3100.md/Quad_AddUnsLong.o ds3100.md/Quad_CompareUns.o ds3100.md/Quad_PutUns.o ds3100.md/Quad_UnsToDouble.o
INSTFILES	= ds3100.md/md.mk ds3100.md/dependencies.mk Makefile
SACREDOBJS	= 
