#!/bin/sh
#
# Mach Operating System
# Copyright (c) 1991,1990 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
# 
# Carnegie Mellon requests users of this software to return to
# 
#  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
# 
# any improvements or extensions that they make and grant Carnegie Mellon
# the rights to redistribute these changes.
#
#
# HISTORY
# $Log:	mig.sh,v $
# Revision 1.4  92/04/24  21:52:13  kupfer
# MK73 merge.
# 
# Revision 2.9  92/04/01  19:36:14  rpd
# 	Added option -cc arg
# 	It informs mig that it should use the C compiler driver to
# 	call the preprocessor. The argument gives the name of the 
# 	driver and the flags that make it preprocess.
# 	[92/03/18            jvh]
# 
# Revision 2.8  92/01/23  15:21:28  rpd
# 	Changed to generate a dependency on migcom itself.
# 	[92/01/19            rpd]
# 
# Revision 2.7  92/01/14  16:46:20  rpd
# 	Removed -theader switched.
# 	Fixed dependency file generation to remove /dev/null.
# 	[92/01/10            rpd]
# 
# Revision 1.3  91/10/29  22:14:08  kupfer
# MK63 merge.
# 
# Revision 2.6  91/07/31  18:09:45  dbg
# 	Allow both -header and -sheader switches.
# 	Fix copyright.
# 	[91/07/30  17:14:28  dbg]
# 
# Revision 2.5  91/06/25  10:31:42  rpd
# 	Added -sheader.
# 	[91/05/23            rpd]
# 
# Revision 1.2  91/05/28  16:23:49  kupfer
# Set correct path for cpp & migcom.
# 
# Revision 2.4  91/02/05  17:55:08  mrt
# 	Changed to new Mach copyright
# 	[91/02/01  17:54:47  mrt]
# 
# Revision 2.3  90/06/19  23:01:10  rpd
# 	The -i option takes an argument now.
# 	[90/06/03            rpd]
# 
# Revision 2.2  90/06/02  15:05:05  rpd
# 	For BobLand: changed /usr/cs/bin/wh to wh.
# 	[90/06/02            rpd]
# 
# 	Created for new IPC.
# 	[90/03/26  21:12:04  rpd]
# 
# 27-May-87  Richard Draves (rpd) at Carnegie-Mellon University
#	Created.
#

CPP=/sprite/cmds/cpp
MIGCOM=/sprite/cmds/migcom

usecc=false
cppflags=
migflags=
files=

# If an argument to this shell script contains whitespace,
# then we will screw up.  migcom will see it as multiple arguments.
#
# As a special hack, if -i is specified first we don't pass -user to migcom.
# We do use the -user argument for the dependencies.
# In this case, the -user argument can have whitespace.

until [ $# -eq 0 ]
do
    case "$1" in
	-[qQvVtTrRsS] ) migflags="$migflags $1"; shift;;
	-i	) sawI=1; migflags="$migflags $1 $2"; shift; shift;;
	-user   ) user="$2"; if [ ! "${sawI-}" ]; then migflags="$migflags $1 $2"; fi; shift; shift;;
	-server ) server="$2"; migflags="$migflags $1 $2"; shift; shift;;
	-header ) header="$2"; migflags="$migflags $1 $2"; shift; shift;;
	-sheader ) sheader="$2"; migflags="$migflags $1 $2"; shift; shift;;
	-iheader ) iheader="$2"; migflags="$migflags $1 $2"; shift; shift;;

	-MD ) sawMD=1; cppflags="$cppflags $1"; shift;;
	-cc) usecc=true; CPP="$2"; shift; shift;;
	-* ) cppflags="$cppflags $1"; shift;;
	* ) files="$files $1"; shift;;
    esac
done

for file in $files
do
    base="`/usr/bin/basename "$file" .defs`"
	if $usecc; then
		$CPP $cppflags "$file" | $MIGCOM $migflags || exit
	else
	    	$CPP $cppflags "$file" - ${sawMD+"$base".defs.d~} |
					$MIGCOM $migflags || exit
	fi
    if [ $sawMD ]
    then
	deps=
	s=
	rheader="${header-${base}.h}"
	if [ "$rheader" != /dev/null ]; then
		deps="${deps}${s}${rheader}"; s=" "
	fi
	ruser="${user-${base}User.c}"
	if [ "$ruser" != /dev/null ]; then
		deps="${deps}${s}${ruser}"; s=" "
	fi
	rserver="${server-${base}Server.c}"
	if [ "$rserver" != /dev/null ]; then
		deps="${deps}${s}${rserver}"; s=" "
	fi
	rsheader="${sheader-/dev/null}"
	if [ "$rsheader" != /dev/null ]; then
		deps="${deps}${s}${rsheader}"; s=" "
	fi
	riheader="${iheader-/dev/null}"
	if [ "$riheader" != /dev/null ]; then
		deps="${deps}${s}${riheader}"; s=" "
	fi
# fixup dependencies, adding a dependency on migcom itself
	if $usecc; then
		{ sed 's;^'"$base"'.o;'"$deps"';' < "$base".d; \
		echo "${deps}: ${MIGCOM}"; } > "$base".defs.d
	       	rm -f "$base".d

	else
		{ sed 's;^'"$base"'.o;'"$deps"';' < "$base".defs.d~; \
	  	echo "${deps}: ${MIGCOM}"; } > "$base".defs.d
		rm -f "$base".defs.d~
	fi

    fi
done

exit 0
