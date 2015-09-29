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
# Thu Mar 15 14:51:47 PST 1990
#
# For more information, refer to the mkmf manual page.
#
# $Header: /sprite/lib/mkmf/RCS/Makefile.md,v 1.6 90/03/12 23:28:42 jhh Exp $
#
# Allow mkmf

SRCS		= array.c awk.y builtin.c debug.c eval.c field.c io.c main.c missing.c msg.c node.c regex.c version.c
HDRS		= awk.h patchlevel.h regex.h
MDPUBHDRS	= 
OBJS		= sun4.md/array.o sun4.md/awk.o sun4.md/builtin.o sun4.md/debug.o sun4.md/eval.o sun4.md/field.o sun4.md/io.o sun4.md/main.o sun4.md/missing.o sun4.md/msg.o sun4.md/node.o sun4.md/regex.o sun4.md/version.o
CLEANOBJS	= sun4.md/array.o sun4.md/awk.o sun4.md/builtin.o sun4.md/debug.o sun4.md/eval.o sun4.md/field.o sun4.md/io.o sun4.md/main.o sun4.md/missing.o sun4.md/msg.o sun4.md/node.o sun4.md/regex.o sun4.md/version.o
INSTFILES	= sun4.md/md.mk sun4.md/dependencies.mk Makefile local.mk
SACREDOBJS	= 
