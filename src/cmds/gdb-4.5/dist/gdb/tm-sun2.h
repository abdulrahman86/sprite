/* Parameters for execution on a Sun, for GDB, the GNU debugger.
   Copyright (C) 1986, 1987, 1989, 199 Free Software Foundation, Inc.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Define this if the C compiler puts an underscore at the front
   of external names before giving them to the linker.  */

#define NAMES_HAVE_UNDERSCORE

/* Address of the end of stack space.  We get this from the system
   include files. */

#include <sys/types.h>
#include <machine/vmparam.h>
#define STACK_END_ADDR USRSTACK

#include "tm-68k.h"
