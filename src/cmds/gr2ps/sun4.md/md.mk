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
# Wed Dec  5 22:21:43 PST 1990
#
# For more information, refer to the mkmf manual page.
#
# $Header: /sprite/lib/mkmf/RCS/Makefile.md,v 1.6 90/03/12 23:28:42 jhh Exp $
#
# Allow mkmf

SRCS		= curve.c gr2ps.c
HDRS		= 
MDPUBHDRS	= 
OBJS		= sun4.md/curve.o sun4.md/gr2ps.o
CLEANOBJS	= sun4.md/curve.o sun4.md/gr2ps.o
INSTFILES	= Makefile local.mk tags
SACREDOBJS	= 
