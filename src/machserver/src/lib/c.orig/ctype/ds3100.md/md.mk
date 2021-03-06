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
# Tue Oct 24 00:29:02 PDT 1989
#
# For more information, refer to the mkmf manual page.
#
# $Header: /sprite/lib/mkmf/RCS/Makefile.md,v 1.4 89/10/09 21:28:30 rab Exp $
#
# Allow mkmf

SRCS		= ctypeBits.c isalnum.c isalpha.c isascii.c iscntrl.c isdigit.c isgraph.c islower.c isprint.c ispunct.c isspace.c isupper.c isxdigit.c toascii.c tolower.c toupper.c
HDRS		= 
MDPUBHDRS	= 
OBJS		= ds3100.md/ctypeBits.o ds3100.md/isalnum.o ds3100.md/isalpha.o ds3100.md/isascii.o ds3100.md/iscntrl.o ds3100.md/isdigit.o ds3100.md/isgraph.o ds3100.md/islower.o ds3100.md/isprint.o ds3100.md/ispunct.o ds3100.md/isspace.o ds3100.md/isupper.o ds3100.md/isxdigit.o ds3100.md/toascii.o ds3100.md/tolower.o ds3100.md/toupper.o
CLEANOBJS	= ds3100.md/ctypeBits.o ds3100.md/isalnum.o ds3100.md/isalpha.o ds3100.md/isascii.o ds3100.md/iscntrl.o ds3100.md/isdigit.o ds3100.md/isgraph.o ds3100.md/islower.o ds3100.md/isprint.o ds3100.md/ispunct.o ds3100.md/isspace.o ds3100.md/isupper.o ds3100.md/isxdigit.o ds3100.md/toascii.o ds3100.md/tolower.o ds3100.md/toupper.o
DISTDIR        ?= @(DISTDIR)
