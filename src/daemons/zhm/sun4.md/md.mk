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
# Tue Nov 24 13:31:24 PST 1992
#
# For more information, refer to the mkmf manual page.
#
# $Header: /sprite/lib/mkmf/RCS/Makefile.md,v 1.6 90/03/12 23:28:42 jhh Exp $
#
# Allow mkmf

SRCS		= queue.c zhm.c zhm_client.c zhm_server.c
HDRS		= zhm.h
MDPUBHDRS	= 
OBJS		= sun4.md/queue.o sun4.md/zhm.o sun4.md/zhm_client.o sun4.md/zhm_server.o
CLEANOBJS	= sun4.md/queue.o sun4.md/zhm.o sun4.md/zhm_client.o sun4.md/zhm_server.o
INSTFILES	= sun4.md/md.mk sun4.md/dependencies.mk Makefile local.mk
SACREDOBJS	= 
