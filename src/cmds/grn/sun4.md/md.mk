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
# Thu Aug 24 14:58:51 PDT 1989
#
# For more information, refer to the mkmf manual page.
#
# $Header: /sprite/lib/mkmf/RCS/Makefile.md,v 1.3 88/06/06 17:23:47 ouster Exp $
#
# Allow mkmf

SRCS		= hdb.c hgraph.c hpoint.c main.c
HDRS		= dev.h gprint.h
MDPUBHDRS	= 
OBJS		= sun4.md/hdb.o sun4.md/hgraph.o sun4.md/hpoint.o sun4.md/main.o
CLEANOBJS	= sun4.md/hdb.o sun4.md/hgraph.o sun4.md/hpoint.o sun4.md/main.o
