#
# This file is included by Makefile.  Makefile is generated automatically
# by mkmf, and this file provides additional local personalization.  The
# variable SYSMAKEFILE is provided by Makefile;  it's a system Makefile
# that must be included to set up various compilation stuff.
#


#include	<$(SYSMAKEFILE)>
install		::
	update -m 755 -l Pnews.header /sprite/lib/rn/Pnews.header

