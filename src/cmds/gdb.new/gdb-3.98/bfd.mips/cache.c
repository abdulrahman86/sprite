/* BFD library -- caching of file descriptors.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.
   Hacked by Steve Chamberlain of Cygnus Support (steve@cygnus.com).

This file is part of BFD, the Binary File Descriptor library.

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

/*doc*
@section File Caching
The file caching mechanism is embedded within BFD and allows the application to open as many
bfds as it wants without regard to the underlying operating system's
file descriptor limit (often as low as 20 open files).

The module in @code{cache.c} maintains a least recently used list of
@code{BFD_CACHE_MAX_OPEN} files, and exports the name
@code{bfd_cache_lookup} which runs around and makes sure that the
required bfd is open. If not, then it chooses a file to close, closes
it and opens the one wanted, returning its file handle.

*/



/* $Id: cache.c,v 1.8 1991/07/31 16:57:29 gnu Exp $ */
#include <sysdep.h>
#include "bfd.h"
#include "libbfd.h"


/*proto-internal* BFD_CACHE_MAX_OPEN
The maxiumum number of files which the cache will keep open at one
time.
*+
#define BFD_CACHE_MAX_OPEN 10
*-

*/


static int open_files;

static bfd *cache_sentinel;	/* Chain of bfds with active fds we've
				   opened */

/*proto-internal*  bfd_last_cache
Zero, or a pointer to the topmost bfd on the chain.  This is used by the
bfd_cache_lookup() macro in libbfd.h to determine when it can avoid a function
call.  
*+
extern bfd *bfd_last_cache;
*-

*/

bfd *bfd_last_cache;

/*proto-internal*  bfd_cache_lookup
Checks to see if the required bfd is the same as the last one looked
up. If so then it can use the iostream in the bfd with impunity, since
it can't have changed since the last lookup, otherwise it has to
perform the complicated lookup function
*+
#define bfd_cache_lookup(x) \
     ((x)==bfd_last_cache? \
        (FILE*)(bfd_last_cache->iostream): \
         bfd_cache_lookup_worker(x))

*-

*/

static void bfd_cache_delete();


static void
DEFUN_VOID(close_one)
{
    bfd *kill = cache_sentinel;
    if (kill == 0)		/* Nothing in the cache */
	return ;

    /* We can only close files that want to play this game.  */
    while (!kill->cacheable) {
	kill = kill->lru_prev;
	if (kill == cache_sentinel) /* Nobody wants to play */
	   return ;
    }

    kill->where = ftell((FILE *)(kill->iostream));
    bfd_cache_delete(kill);
}

/* Cuts the bfd abfd out of the chain in the cache */
static void 
DEFUN(snip,(abfd),
      bfd *abfd)
{
  abfd->lru_prev->lru_next = abfd->lru_next;
  abfd->lru_next->lru_prev = abfd->lru_prev; 
  if (cache_sentinel == abfd) cache_sentinel = (bfd *)NULL;
}

static void
DEFUN(bfd_cache_delete,(abfd),
      bfd *abfd)
{
  fclose ((FILE *)(abfd->iostream));
  snip (abfd);
  abfd->iostream = NULL;
  open_files--;
  bfd_last_cache = 0;
}
  
static bfd *
DEFUN(insert,(x,y),
      bfd *x AND
      bfd *y)
{
  if (y) {
    x->lru_next = y;
    x->lru_prev = y->lru_prev;
    y->lru_prev->lru_next = x;
    y->lru_prev = x;

  }
  else {
    x->lru_prev = x;
    x->lru_next = x;
  }
  return x;
}


/*proto-internal*
*i bfd_cache_init
Initialize a BFD by putting it on the cache LRU.
*; PROTO(void, bfd_cache_init, (bfd *));
*-*/

void
DEFUN(bfd_cache_init,(abfd),
      bfd *abfd)
{
  cache_sentinel = insert(abfd, cache_sentinel);
}


/*proto-internal*
*i bfd_cache_close
Remove the bfd from the cache. If the attatched file is open, then close it too.
*; PROTO(void, bfd_cache_close, (bfd *));
*-*/
void
DEFUN(bfd_cache_close,(abfd),
      bfd *abfd)
{
  /* If this file is open then remove from the chain */
  if (abfd->iostream) 
    {
      bfd_cache_delete(abfd);
    }
}

/*proto-internal*
*i bfd_open_file
Call the OS to open a file for this BFD.  Returns the FILE *
(possibly null) that results from this operation.  Sets up the
BFD so that future accesses know the file is open. If the FILE *
returned is null, then there is won't have been put in the cache, so
it won't have to be removed from it.
*; PROTO(FILE *, bfd_open_file, (bfd *));
*-*/
FILE *
DEFUN(bfd_open_file, (abfd),
      bfd *abfd)
{
  abfd->cacheable = true;	/* Allow it to be closed later. */
  if(open_files >= BFD_CACHE_MAX_OPEN) {
    close_one();
  }
  switch (abfd->direction) {
  case read_direction:
  case no_direction:
    abfd->iostream = (char *) fopen(abfd->filename, "r");
    break;
  case both_direction:
  case write_direction:
    if (abfd->opened_once == true) {
      abfd->iostream = (char *) fopen(abfd->filename, "r+");
      if (!abfd->iostream) {
	abfd->iostream = (char *) fopen(abfd->filename, "w+");
      }
    } else {
      /*open for creat */
      abfd->iostream = (char *) fopen(abfd->filename, "w");
      abfd->opened_once = true;
    }
    break;
  }
  if (abfd->iostream) {
    open_files++;
    bfd_cache_init (abfd);
  }

  return (FILE *)(abfd->iostream);
}

/*proto-internal*
*i bfd_cache_lookup_worker
Called when the macro @code{bfd_cache_lookup} fails to find a quick
answer. Finds a file descriptor for this BFD.  If necessary, it open it.
If there are already more than BFD_CACHE_MAX_OPEN files open, it trys to close
one first, to avoid running out of file descriptors. 
*; PROTO(FILE *, bfd_cache_lookup_worker, (bfd *));

*-*/

FILE *
DEFUN(bfd_cache_lookup_worker,(abfd),
      bfd *abfd)
{
  if (abfd->my_archive) 
      {
	abfd = abfd->my_archive;
      }
  /* Is this file already open .. if so then quick exit */
  if (abfd->iostream) 
      {
	if (abfd != cache_sentinel) {
	  /* Place onto head of lru chain */
	  snip (abfd);
	  cache_sentinel = insert(abfd, cache_sentinel);
	}
      }
  /* This is a bfd without a stream -
     so it must have been closed or never opened.
     find an empty cache entry and use it.  */
  else 
      {

	if (open_files >= BFD_CACHE_MAX_OPEN) 
	    {
	      close_one();
	    }

	BFD_ASSERT(bfd_open_file (abfd) != (FILE *)NULL) ;
	fseek((FILE *)(abfd->iostream), abfd->where, false);
      }
  bfd_last_cache = abfd;
  return (FILE *)(abfd->iostream);
}
