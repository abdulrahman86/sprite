/* Read coff symbol tables and convert to internal format, for GDB.
   Design and support routines derived from dbxread.c, and UMAX COFF
   specific routines written 9/1/87 by David D. Johnson, Brown University.
   Revised 11/27/87 ddj@cs.brown.edu
   Copyright (C) 1987, 1988, 1989 Free Software Foundation, Inc.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


#include <a.out.h>
#include <stdio.h>
#include "symtab.h"
#include <sys/file.h>

PDR *proc_desc_table = NULL;
long proc_desc_length = 0;

static void add_symbol_to_list ();
static int read_coff_symtab ();
static void patch_opaque_types ();
static struct type *decode_function_type ();
static struct type *decode_type ();
static struct type *decode_base_type ();
static struct type *read_enum_type ();
static struct type *read_struct_type ();
static struct type *read_type ();
static void finish_block ();
static struct blockvector *make_blockvector ();
static struct symbol *process_coff_symbol ();
static char *getfilename ();
static char *getsymname ();

extern int fclose ();
extern void close ();
extern void free_all_symtabs ();
extern void free_all_psymtabs ();


/* Name of source file whose symbol data we are now processing.
   This comes from a symbol named ".file".  */

static char *last_source_file;

/* Core address of the end of the first object file.  */
static CORE_ADDR first_object_file_end;

/* End of the text segment of the executable file,
   as found in the symbol _etext.  */

static CORE_ADDR end_of_text_addr;

/* The end address of the last seen procedure. */

static CORE_ADDR last_end_addr;

/* The file, a.out  and text section headers of the symbol file */

static FILHDR file_hdr;
static SCNHDR text_hdr;
static AOUTHDR aout_hdr;

/* The index in the symbol table of the last coff symbol that was processed.  */

static int symnum;

/* Vector of types defined so far, indexed by their coff symnum.  */

static struct typevector *type_vector;

/* Number of elements allocated for type_vector currently.  */

static int type_vector_length;

/* Chain of typedefs of pointers to empty struct/union types.
   They are chained thru the SYMBOL_VALUE.  */

#define HASHSIZE 127
static struct symbol *opaque_type_chain[HASHSIZE];

/* Record the symbols defined for each context in a list.
   We don't create a struct block for the context until we
   know how long to make it.  */

struct pending
{
  struct pending *next;
  struct symbol *symbol;
};

/* Here are the three lists that symbols are put on.  */

struct pending *file_symbols;	/* static at top level, and types */

struct pending *global_symbols;	/* global functions and variables */

struct pending **global_symbols_all, **file_symbols_all;

struct pending *local_symbols;	/* everything local to lexical context */

/* List of unclosed lexical contexts
   (that will become blocks, eventually).  */

struct context_stack
{
  struct context_stack *next;
  struct pending *locals;
  struct pending_block *old_blocks;
  struct symbol *name;
  CORE_ADDR start_addr;
};

struct context_stack *context_stack;

/* Nonzero if within a function (so symbols should be local,
   if nothing says specifically).  */

int within_function;

/* List of blocks already made (lexical contexts already closed).
   This is used at the end to make the blockvector.  */

struct pending_block
{
  struct pending_block *next;
  struct block *block;
};

struct pending_block *pending_blocks;

extern CORE_ADDR startup_file_start;	/* From blockframe.c */
extern CORE_ADDR startup_file_end;	/* From blockframe.c */

/* File name symbols were loaded from.  */

static char *symfile;

/* Look up a coff type-number index.  Return the address of the slot
   where the type for that index is stored.
   The type-number is in INDEX. 

   This can be used for finding the type associated with that index
   or for associating a new type with the index.  */

static struct type **
coff_lookup_type (index)
     register int index;
{
  if (index >= type_vector_length)
    {
      int old_vector_length = type_vector_length;

      type_vector_length *= 2;
      if (type_vector_length < index) {
	type_vector_length = index * 2;
      }
      type_vector = (struct typevector *)
	xrealloc (type_vector, sizeof (struct typevector)
				+ type_vector_length * sizeof (struct type *));
      bzero (&type_vector->type[ old_vector_length ],
	     (type_vector_length - old_vector_length) * sizeof(struct type *));
    }
  return &type_vector->type[index];
}

/* Make sure there is a type allocated for type number index
   and return the type object.
   This can create an empty (zeroed) type object.  */

static struct type *
coff_alloc_type (index)
     int index;
{
  register struct type **type_addr = coff_lookup_type (index);
  register struct type *type = *type_addr;

  /* If we are referring to a type not known at all yet,
     allocate an empty type for it.
     We will fill it in later if we find out how.  */
  if (type == 0)
    {
      type = (struct type *) obstack_alloc (symbol_obstack,
					    sizeof (struct type));
      bzero (type, sizeof (struct type));
      *type_addr = type;
    }
  return type;
}

/* maintain the lists of symbols and blocks */

/* Add a symbol to one of the lists of symbols.  */
static void
add_symbol_to_list (symbol, listhead)
     struct symbol *symbol;
     struct pending **listhead;
{
  register struct pending *link
    = (struct pending *) xmalloc (sizeof (struct pending));

  link->next = *listhead;
  link->symbol = symbol;
  *listhead = link;
}

/* Take one of the lists of symbols and make a block from it.
   Put the block on the list of pending blocks.  */

static void
finish_block (symbol, listhead, old_blocks, start, end)
     struct symbol *symbol;
     struct pending **listhead;
     struct pending_block *old_blocks;
     CORE_ADDR start, end;
{
  register struct pending *next, *next1;
  register struct block *block;
  register struct pending_block *pblock;
  struct pending_block *opblock;
  register int i;

  /* Count the length of the list of symbols.  */

  for (next = *listhead, i = 0; next; next = next->next, i++);

  block = (struct block *)
	    obstack_alloc (symbol_obstack, sizeof (struct block) + (i - 1) * sizeof (struct symbol *));

  /* Copy the symbols into the block.  */

  BLOCK_NSYMS (block) = i;
  for (next = *listhead; next; next = next->next)
    BLOCK_SYM (block, --i) = next->symbol;

  BLOCK_START (block) = start;
  BLOCK_END (block) = end;
  BLOCK_SUPERBLOCK (block) = 0;	/* Filled in when containing block is made */

  /* Put the block in as the value of the symbol that names it.  */

  if (symbol)
    {
      SYMBOL_BLOCK_VALUE (symbol) = block;
      BLOCK_FUNCTION (block) = symbol;
    }
  else
    BLOCK_FUNCTION (block) = 0;

  /* Now free the links of the list, and empty the list.  */

  for (next = *listhead; next; next = next1)
    {
      next1 = next->next;
      free (next);
    }
  *listhead = 0;

  /* Install this block as the superblock
     of all blocks made since the start of this scope
     that don't have superblocks yet.  */

  opblock = 0;
  for (pblock = pending_blocks; pblock != old_blocks; pblock = pblock->next)
    {
      if (BLOCK_SUPERBLOCK (pblock->block) == 0)
	BLOCK_SUPERBLOCK (pblock->block) = block;
      opblock = pblock;
    }

  /* Record this block on the list of all blocks in the file.
     Put it after opblock, or at the beginning if opblock is 0.
     This puts the block in the list after all its subblocks.  */

  pblock = (struct pending_block *) xmalloc (sizeof (struct pending_block));
  pblock->block = block;
  if (opblock)
    {
      pblock->next = opblock->next;
      opblock->next = pblock;
    }
  else
    {
      pblock->next = pending_blocks;
      pending_blocks = pblock;
    }
}

static struct blockvector *
make_blockvector ()
{
  register struct pending_block *next, *next1;
  register struct blockvector *blockvector;
  register int i;

  /* Count the length of the list of blocks.  */

  for (next = pending_blocks, i = 0; next; next = next->next, i++);

  blockvector = (struct blockvector *)
		  obstack_alloc (symbol_obstack, sizeof (struct blockvector) + (i - 1) * sizeof (struct block *));

  /* Copy the blocks into the blockvector.
     This is done in reverse order, which happens to put
     the blocks into the proper order (ascending starting address).
     finish_block has hair to insert each block into the list
     after its subblocks in order to make sure this is true.  */

  BLOCKVECTOR_NBLOCKS (blockvector) = i;
  for (next = pending_blocks; next; next = next->next)
    BLOCKVECTOR_BLOCK (blockvector, --i) = next->block;

  /* Now free the links of the list, and empty the list.  */

  for (next = pending_blocks; next; next = next1)
    {
      next1 = next->next;
      free (next);
    }
  pending_blocks = 0;

  return blockvector;
}

/* Manage the vector of line numbers.  */

static
record_line (line, pc)
     int line;
     CORE_ADDR pc;
{
}

/* Start a new symtab for a new source file.
   This is called when a COFF ".file" symbol is seen;
   it indicates the start of data for one original source file.  */

static void
start_symtab ()
{
  context_stack = 0;
  within_function = 0;
  last_source_file = 0;

  /* Initialize the source file information for this file.  */

}

/* Finish the symbol definitions for one main source file,
   close off all the lexical contexts for that file
   (creating struct block's for them), then make the
   struct symtab for that file and put it in the list of all such.

   END_ADDR is the address of the end of the file's text.  */

static void
end_symtab (start_addr, end_addr, linetable)
     CORE_ADDR start_addr, end_addr;
     struct linetable *linetable;
{
  register struct symtab *symtab;
  register struct context_stack *cstk;
  register struct blockvector *blockvector;
  struct partial_symtab *psymtab;

  if (aout_hdr.entry < end_addr
      && aout_hdr.entry >= start_addr)
    {
      startup_file_start = start_addr;
      startup_file_end = end_addr;
    }

  /* Finish the lexical context of the last function in the file.  */

  if (context_stack)
    {
      cstk = context_stack;
      context_stack = 0;
      /* Make a block for the local symbols within.  */
      finish_block (cstk->name, &local_symbols, cstk->old_blocks,
		    cstk->start_addr, end_addr);
      free (cstk);
    }

  /* Create the two top-level blocks for this file.  */
  finish_block (0, &file_symbols, 0, start_addr, end_addr);
  finish_block (0, &global_symbols, 0, start_addr, end_addr);

  /* Create the blockvector that points to all the file's blocks.  */
  blockvector = make_blockvector ();

  /* Now create the symtab object for this source file.  */
  symtab = (struct symtab *) xmalloc (sizeof (struct symtab));
  symtab->free_ptr = 0;

  /* Fill in its components.  */
  symtab->blockvector = blockvector;
  symtab->free_code = free_linetable;
  symtab->filename = last_source_file;
  symtab->linetable = linetable;
  symtab->nlines = 0;
  symtab->line_charpos = 0;

  /* Link the new symtab into the list of such.  */
  symtab->next = symtab_list;
  symtab_list = symtab;

  /* Reinitialize for beginning of new file. */
  last_source_file = 0;

  /* Create a fake partial_symtab, so that find_pc_partial_function
   * will do the right thing. */
  psymtab = (struct partial_symtab *)
      obstack_alloc (psymbol_obstack,
		     sizeof (struct partial_symtab));
  bzero (psymtab, sizeof (struct partial_symtab));
  psymtab->next = partial_symtab_list;
  partial_symtab_list = psymtab;
  psymtab->textlow = start_addr;
  psymtab->texthigh = end_addr;
  psymtab->filename = symtab->filename ? symtab->filename : "";
  psymtab->readin = 1;

}

/* Accumulate the misc functions in bunches of 127.
   At the end, copy them all into one newly allocated structure.  */

#define MISC_BUNCH_SIZE 127

struct misc_bunch
{
  struct misc_bunch *next;
  struct misc_function contents[MISC_BUNCH_SIZE];
};

/* Bunch currently being filled up.
   The next field points to chain of filled bunches.  */

static struct misc_bunch *misc_bunch;

/* Number of slots filled in current bunch.  */

static int misc_bunch_index;

/* Total number of misc functions recorded so far.  */

static int misc_count;

static void
init_misc_functions ()
{
  misc_count = 0;
  misc_bunch = 0;
  misc_bunch_index = MISC_BUNCH_SIZE;
}

static void
record_misc_function (name, address)
     char *name;
     CORE_ADDR address;
{
  register struct misc_bunch *new;

  if (misc_bunch_index == MISC_BUNCH_SIZE)
    {
      new = (struct misc_bunch *) xmalloc (sizeof (struct misc_bunch));
      misc_bunch_index = 0;
      new->next = misc_bunch;
      misc_bunch = new;
    }
  misc_bunch->contents[misc_bunch_index].name = savestring (name, strlen (name));
  misc_bunch->contents[misc_bunch_index].address = address;
  misc_bunch->contents[misc_bunch_index].type = (char)mf_unknown;
  misc_bunch_index++;
  misc_count++;
}

/* if we see a function symbol, we do record_misc_function.
 * however, if it turns out the next symbol is '.bf', then
 * we call here to undo the misc definition
 */
static void
unrecord_misc_function ()
{
  if (misc_bunch_index == 0)
    error ("Internal error processing symbol table, at symbol %d.",
	   symnum);
  misc_bunch_index--;
  misc_count--;
}


static int
compare_misc_functions (fn1, fn2)
     struct misc_function *fn1, *fn2;
{
  /* Return a signed result based on unsigned comparisons
     so that we sort into unsigned numeric order.  */
  if (fn1->address < fn2->address)
    return -1;
  if (fn1->address > fn2->address)
    return 1;
  return 0;
}

static void
discard_misc_bunches ()
{
  register struct misc_bunch *next;

  while (misc_bunch)
    {
      next = misc_bunch->next;
      free (misc_bunch);
      misc_bunch = next;
    }
}

static void
condense_misc_bunches ()
{
  register int i, j;
  register struct misc_bunch *bunch;
#ifdef NAMES_HAVE_UNDERSCORE
  int offset = 1;
#else
  int offset = 0;
#endif

  misc_function_vector
    = (struct misc_function *)
      xmalloc (misc_count * sizeof (struct misc_function));

  j = 0;
  bunch = misc_bunch;
  while (bunch)
    {
      for (i = 0; i < misc_bunch_index; i++)
	{
	  register char *tmp;

	  misc_function_vector[j] = bunch->contents[i];
	  tmp = misc_function_vector[j].name;
	  misc_function_vector[j].name = (tmp[0] == '_' ? tmp + offset : tmp);
	  j++;
	}
      bunch = bunch->next;
      misc_bunch_index = MISC_BUNCH_SIZE;
    }

  misc_function_count = j;

  /* Sort the misc functions by address.  */

  qsort (misc_function_vector, j, sizeof (struct misc_function),
	 compare_misc_functions);
}

/* Call sort_syms to sort alphabetically
   the symbols of each block of each symtab.  */

static int
compare_symbols (s1, s2)
     struct symbol **s1, **s2;
{
  /* Names that are less should come first.  */
  register int namediff = strcmp (SYMBOL_NAME (*s1), SYMBOL_NAME (*s2));
  if (namediff != 0) return namediff;
  /* For symbols of the same name, registers should come first.  */
  return ((SYMBOL_CLASS (*s2) == LOC_REGISTER)
	  - (SYMBOL_CLASS (*s1) == LOC_REGISTER));
}

static void
sort_syms ()
{
  register struct symtab *s;
  register int i, nbl;
  register struct blockvector *bv;
  register struct block *b;

  for (s = symtab_list; s; s = s->next)
    {
      bv = BLOCKVECTOR (s);
      nbl = BLOCKVECTOR_NBLOCKS (bv);
      for (i = 0; i < nbl; i++)
	{
	  b = BLOCKVECTOR_BLOCK (bv, i);
	  if (BLOCK_SHOULD_SORT (b))
		  qsort (&BLOCK_SYM (b, 0), BLOCK_NSYMS (b),
			 sizeof (struct symbol *), compare_symbols);
	}
    }
}

/* Call sort_proc_descs to sort the procedure descriptors by address.  */

static int
compare_proc_descs (p1, p2)
PDR *p1, *p2;
{
  unsigned int a1, a2;

  a1 = PROC_LOW_ADDR (p1);
  a2 = PROC_LOW_ADDR (p2);
  if (a1 < a2)
    return -1;
  if (a1 > a2)
    return 1;
  return 0;
}

static void
sort_proc_descs ()
{
  qsort (proc_desc_table, proc_desc_length,
	 sizeof (PDR), compare_proc_descs);
}


/* This is the symbol-file command.  Read the file, analyze its symbols,
   and add a struct symtab to symtab_list.  */

/* !!! !!! */
int dump_stuff=1;

static void free_proc_descs ()
{
    if (proc_desc_table != NULL) {
	free (proc_desc_table);
	proc_desc_table = NULL;
	proc_desc_length = 0;
    }
}

void
symbol_file_command (name)
     char *name;
{
  int desc;
  int num_symbols;
  int num_sections;
  int symtab_offset;
  register int val;
  struct cleanup *old_chain;

  dont_repeat ();

  if (name == 0)
    {
      if (symtab_list && !query ("Discard symbol table? ", 0))
	error ("Not confirmed.");
      if (symfile)
	free (symfile);
      symfile = 0;
      free_all_symtabs ();
      free_proc_descs ();
      return;
    }

  name = tilde_expand (name);
  make_cleanup (free, name);

  if (symtab_list && !query ("Load new symbol table from \"%s\"? ", name))
    error ("Not confirmed.");

  if (symfile)
    free (symfile);
  symfile = 0;

  {
    char *absolute_name;

    desc = openp (getenv ("PATH"), 1, name, O_RDONLY, 0, &absolute_name);
    if (desc < 0)
      perror_with_name (name);
    else
      name = absolute_name;
  }

  old_chain = make_cleanup (close, desc);
  make_cleanup (free_current_contents, &name);

  if ((num_symbols = read_file_hdr (desc, &file_hdr)) < 0)
    error ("File \"%s\" not in executable format.", name);

  /* If an a.out header is present, read it in.  If not (e.g. a .o file)
     deal with its absence.  */
  if (file_hdr.f_opthdr == 0
      || read_aout_hdr (desc, &aout_hdr, file_hdr.f_opthdr) < 0)
    {
      /* We will not actually be able to run code, since backtraces would
	 fly off the bottom of the stack (there is no way to reliably
	 detect bottom of stack), but that's fine since the kernel won't
	 run something without an a.out header anyway.  Passive examination
	 of .o files is one place this might make sense.  */
      /* ~0 will not be in any file.  */
      aout_hdr.entry = ~0;
      /* set the startup file to be an empty range.  */
      startup_file_start = 0;
      startup_file_end = 0;
    }

  if (num_symbols == 0)
    {
      free_all_symtabs ();
      free_proc_descs ();
      printf ("%s does not have a symbol-table.\n", name);
      fflush (stdout);
      do_cleanups (old_chain);
      return;
    }

  printf ("Reading symbol data from %s...", name);
  fflush (stdout);

  /* Throw away the old symbol table.  */

  free_all_symtabs ();
  free_proc_descs ();
  free_all_psymtabs ();		/* Make sure that partial_symtab_list */
				/* is 0 also. */

  num_sections = file_hdr.f_nscns;
  symtab_offset = file_hdr.f_symptr;

  if (read_section_hdr (desc, _TEXT, &text_hdr, num_sections) < 0)
    error ("\"%s\": can't read text section header", name);

  /* Position to read the symbol table.  Do not read it all at once. */
  val = lseek (desc, (long)symtab_offset, 0);
  if (val < 0)
    perror_with_name (name);

  init_misc_functions ();
  make_cleanup (discard_misc_bunches, 0);

  /* Now that the executable file is positioned at symbol table,
     process it and define symbols accordingly.  */

  val = read_coff_symtab (desc, num_symbols, symtab_offset);
  if (val < 0) error("Bad symbol table format");

  patch_opaque_types ();

  /* Sort symbols alphabetically within each block.  */

  sort_syms ();

  /* Go over the misc functions and install them in vector.  */

  condense_misc_bunches ();

  /* Sort the procedure descriptor table.  */

  sort_proc_descs ();

  /* Don't allow char * to have a typename (else would get caddr_t.)  */

  TYPE_NAME (lookup_pointer_type (builtin_type_char)) = 0;

  /* Make a default for file to list.  */

  symfile = savestring (name, strlen (name));

  do_cleanups (old_chain);

  printf ("done.\n");
  fflush (stdout);
}

/* Return name of file symbols were loaded from, or 0 if none..  */

char *
get_sym_file ()
{
  return symfile;
}

/* Simplified internal version of coff symbol table information */

struct coff_symbol {
  char *c_name;
  int c_symnum;		/* symbol number of this entry */
  int c_nsyms;		/* 1 if syment only, 2 if syment + auxent */
  long c_value;
  int c_sclass;
  int c_secnum;
  unsigned int c_type;
};

static char *
read_table(desc, size, offset)
    long offset;
{
    char *buf;
    if (lseek(desc, offset, 0) < 0)
      error("Bad symbol table format [seek]");
    buf = xmalloc(size);
    if (buf == NULL) error("Not enough memory");
    if (myread (desc, buf, size) != size)
      error("Bad symbol table format [read]");
    return buf;
}

static char * (MapStNames[]) = { /* symbol type names */
    "Nil", "Global", "Static", "Param", "Local", "Label", "Proc", "Block",
    "End", "Member", "Typedef", "File", "RegReloc", "Forward", "StaticProc",
    "Constant", "BlockPatched"
};

long stNameSize = sizeof(MapStNames)/sizeof(MapStNames[0]);

static char * MapScNames[] = {    /* storage class names */
    "Nil", "Text", "Data", "Bss", "Register", "Abs", "Undefined", "CdbLocal",
    "Bits", "Dbx", "RegImage", "Info", "UserStruct", "SData", "SBss", "RData",
    "Var", "Common", "SCommon", "VarRegister", "Variant", "SUndefined", "Init"
};


long scNameSize = sizeof(MapScNames)/sizeof(MapScNames[0]);

static char *(MapBtNames[]) = {    /* base type names */
    "Nil", "Adr", "Char", "UChar", "Short", "UShort", "Int", "UInt", "Long",
    "ULong", "Float", "Double", "Struct", "Union", "Enum", "Typedef", "Range",
    "Set", "Complex", "DComplex", "Indirect", "FixedDec", "FloatDec", "String",
    "Bit", "Picture"
};

long btNameSize = sizeof(MapBtNames)/sizeof(MapBtNames[0]);

/* Given pointers to a symbol table in coff style exec file,
   analyze them and create struct symtab's describing the symbols.
   NSYMS is the number of symbols in the symbol table.
   We read them one at a time using read_one_sym ().  */

/* stBlockPatched indicates an stBlock symbol which has been patched
 * so that the value points to s struct type */

#define stBlockPatched 16

static FDR *file_descriptor_table;
static RFDT *rel_file_table;
static SYMR *local_symbol_table;
static char *local_string_table;
static AUXU *aux_symbol_table;
static FDR *cur_file_descriptor;
static int file_descriptor_count;
/* Note that PDR is also used for frame chaining, and is defined above */

static int cur_proc_number; /* number of procedure within current file */
static CORE_ADDR cur_proc_addr;
static int cur_file_number;
static int cur_isymBase;
static int cur_issBase;

static void
select_file(i)
    int i;
{
    global_symbols_all[cur_file_number] = global_symbols;
    file_symbols_all[cur_file_number] = file_symbols;
    global_symbols = global_symbols_all[i];
    file_symbols = file_symbols_all[i];
    cur_file_number = i;
    cur_file_descriptor = &file_descriptor_table[i];
    cur_isymBase = cur_file_descriptor->isymBase;
    cur_issBase = cur_file_descriptor->issBase;
}

/* side-effect: changes cur_file_descriptor */

static SYMR *
get_type_context(auxp)
    AUXU** auxp;
{
  SYMR *sym;
  FDR *sym_file_desc;
  int rfi;
  int rfd = (*auxp)->rndx.rfd;
  int sym_index = (*auxp)->rndx.index;
  (*auxp)++;
  if (rfd == ST_RFDESCAPE) { rfd = (*auxp)->isym; (*auxp)++; }
  if (rfd == ST_EXTIFD || sym_index == ST_ANONINDEX) return NULL;
  rfi = rel_file_table[cur_file_descriptor->rfdBase + rfd];
  if (rfi >= file_descriptor_count) return NULL;
  sym_file_desc = &file_descriptor_table[rfi];
  if (sym_index >= sym_file_desc->csym || sym_index == 0) return NULL;
  sym = &local_symbol_table[sym_file_desc->isymBase + sym_index];
  if (sym->index == 0) return NULL;
  select_file(rfi);
  if (dump_stuff)
    printf("rfd(%d,%d)->[%s,%x,st%s,sc%s,inx:%d (%x)]\n",
	 rfi, sym_index,
	 &local_string_table[cur_issBase+sym->iss],
	 sym->value,
		   MapStNames[sym->st], MapScNames[sym->sc],
		   sym->index, sym);
  return sym;
}

static struct type *
get_struct_type(sym, kind)
     SYMR *sym;
     int kind; /* either btStruct or btUnion or btNil (unknown) */
{
  struct type *type;
  if (sym->st == stBlockPatched)
    {
      type = (struct type*)sym->value;
      /* Patch up possibly-wrong guess about struct vs. union */
      if (kind == btStruct) TYPE_CODE(type) = TYPE_CODE_STRUCT;
      else if (kind == btUnion) TYPE_CODE(type) = TYPE_CODE_UNION;
    }
  else if (sym->st != stBlock)
    {
      fprintf(stderr,"Bad symbol table: struct/union points to non-stBlock\n");
      type = builtin_type_void;
    }
  else
    {
      int nfields = 0;
      SYMR *first_field = sym+1;
      register struct field *cur_field;
      char *sym_name = &local_string_table[cur_issBase+sym->iss];
      int iauxBase = cur_file_descriptor->iauxBase;
      type = (struct type *) obstack_alloc (symbol_obstack,
					    sizeof (struct type));
      bzero (type, sizeof (struct type));
      TYPE_LENGTH (type) = sym->value;

      /* we "remember" the type generated from this block */
      sym->st = stBlockPatched;
      sym->value = (long)type;

      /* first count the number of fields */
      for (sym = first_field; sym->st != stEnd; sym++)
	if (sym->st == stMember) nfields++;
	else if (sym->st == stBlock || sym->st == stBlockPatched)
	  {
	    if (sym->sc == scVariant) ; /* UNIMPLEMENTED */
	    if (sym->index != 0)
	      sym = &local_symbol_table[cur_isymBase + sym->index - 1];
	  }

      TYPE_NFIELDS (type) = nfields;
      TYPE_FIELDS (type) = cur_field = (struct field*)
	obstack_alloc (symbol_obstack, nfields * sizeof (struct field));
      for (sym = first_field; sym->st != stEnd; sym++)
	if (sym->st == stMember)
	  {
	    AUXU *aux = &aux_symbol_table[iauxBase + sym->index];
	    char *name = &local_string_table[cur_issBase+sym->iss];
	    cur_field->name =
	      obstack_copy0 (symbol_obstack, name, strlen (name));

	    cur_field->type = read_type(sym->index, &cur_field->bitsize);
	    cur_field->bitpos = sym->value;
	    cur_field++;
	  }
	else if (sym->st == stTypedef) ; /* just ignore it */
	else if (sym->st == stBlock || sym->st == stBlockPatched)
	  {
	    if (sym->sc == scVariant) ; /* UNIMPLEMENTED */
	    if (sym->index != 0)
	      sym = &local_symbol_table[cur_isymBase + sym->index - 1];
	  }
	else
	  fprintf(stderr, "[Bad member in struct/union field list]\n");
      /*
       * Heuristic: if 2nd field has offset 0, this is a union,
       * otherwise, a struct definition.
       * If nfields <= 1, we guess struct.
       * If the type is used later, we fix it up.
       * (This is done in the stBlockPatched case at the top of this routine).
       */
      if (kind == btNil && nfields > 1)
	{
	  if (TYPE_FIELDS(type)[1].bitpos > 0) kind = btStruct;
	  else kind = btUnion;
	}
      TYPE_CODE (type) = kind == btUnion ? TYPE_CODE_UNION : TYPE_CODE_STRUCT;
      TYPE_NAME (type) = concat ("",
				 (kind == btUnion ? "union " : "struct "),
				 sym_name);
    }
    return type;
}

static struct type *
read_struct_type(auxp, kind)
     AUXU** auxp;
     int kind; /* either btStruct or btUnion */
{
  struct type *type;
  int save_file_number = cur_file_number;
  SYMR *sym = get_type_context(auxp);
  if (sym == NULL) {
    /*
     * If we get here then there is reference to a structure that is never
     * defined.  Such a thing is perfectly legal (e.g. a pointer to a
     * structure that is never defined), and the error message is annoying.
     */
#ifndef sprite
    printf(stderr, "Bad symbol for struct/union definition\n");
#endif
    return builtin_type_void;
  }
  if (sym->st == stTypedef)
      type = read_type(sym->index, NULL);
  else
      type = get_struct_type(sym, kind);
  select_file (save_file_number);
  return type;
}

static struct type *
get_enum_type(sym)
    SYMR *sym;
{
  struct type *type;
  if (sym->st == stBlockPatched)
    type = (struct type*)sym->value;
  else if (sym->st != stBlock)
    {
      fprintf(stderr,"Bad symbol table: enum points to non-stBlock\n");
      type = builtin_type_void;
    }
  else
    {
      int nfields = 0;
      SYMR *first_field = sym+1;
      register struct field *cur_field;
      type = (struct type *) obstack_alloc (symbol_obstack,
					    sizeof (struct type));
      bzero (type, sizeof (struct type));
      TYPE_LENGTH (type) = sym->value;

      /* we "remember" the type generated from this block */
      sym->st = stBlockPatched;
      sym->value = (long)type;

      TYPE_CODE (type) = TYPE_CODE_ENUM;
      TYPE_LENGTH (type) = sizeof (int);
      TYPE_NAME (type) = concat ("", "enum ",
				 &local_string_table[cur_issBase+sym->iss]);

      nfields = &local_symbol_table[cur_isymBase+sym->index] - first_field - 1;

      TYPE_NFIELDS (type) = nfields;
      TYPE_FIELDS (type) = cur_field = (struct field*)
	obstack_alloc (symbol_obstack, nfields * sizeof (struct field));
      for (sym = first_field; sym->st != stEnd; sym++)
	if (sym->st == stMember)
	  {
	    char *name = &local_string_table[cur_issBase+sym->iss];
	    cur_field->name =
	      obstack_copy0 (symbol_obstack, name, strlen (name));

	    cur_field->bitpos = sym->value;
	    cur_field->bitsize = 0;
	    cur_field++;
	  }
	else
	  fprintf(stderr, "[Bad member in enum list]\n");
    }
  return type;
}

static struct type *
read_enum_type(auxp)
     AUXU** auxp;
{
  struct type *type;
  int save_file_number = cur_file_number;
  SYMR *sym = get_type_context(auxp);
  if (sym == NULL) {
    fprintf(stderr, "Bad symbol for enum definition");
    return builtin_type_int;
  }
  if (sym->st == stTypedef)
      type = read_type(sym->index, NULL);
  else
      type = get_enum_type (sym);
  select_file (save_file_number);
  return type;
}

static struct type *
read_range_type(auxp)
     AUXU** auxp;
{
  struct type *range_type;
  int save_file_number = cur_file_number;
  SYMR *sym = get_type_context(auxp);
  /* sym (if non-NULL) may point at an stBlock/scInfo, but ignore it */
   range_type = (struct type *) obstack_alloc (symbol_obstack,
					      sizeof (struct type));
  TYPE_CODE (range_type) = TYPE_CODE_RANGE;
  TYPE_TARGET_TYPE (range_type) = builtin_type_int;
  TYPE_LENGTH (range_type) = sizeof (int);
  TYPE_NFIELDS (range_type) = 2;
  TYPE_FIELDS (range_type) =
      (struct field *) obstack_alloc (symbol_obstack,
				      2 * sizeof (struct field));
  TYPE_FIELD_BITPOS (range_type, 0) = (*auxp)++->dnLow;
  TYPE_FIELD_BITPOS (range_type, 1) = (*auxp)++->dnHigh;

  select_file (save_file_number);
  return range_type;
}

static struct type *
modify_type(type, qualifier, auxp)
     struct type *type;
     int qualifier;
     AUXU** auxp;
{
    switch (qualifier) {
      case tqPtr: return lookup_pointer_type(type);
      case tqNil: return type;
      case tqArray:
	{
	    struct type *range_type = read_range_type(auxp);
	    long lower = TYPE_FIELD_BITPOS (range_type, 0);
	    long upper = TYPE_FIELD_BITPOS (range_type, 1);
	    int elem_size = (*auxp)++->width; /* not used */
	    struct type *atype;

	    atype = (struct type *)
		obstack_alloc (symbol_obstack, sizeof (struct type));
	    bzero (atype, sizeof (struct type));

	    TYPE_CODE (atype) = TYPE_CODE_ARRAY;
	    TYPE_TARGET_TYPE (atype) = type;
	    TYPE_LENGTH (atype) = (upper - lower + 1) * TYPE_LENGTH (type);
	    TYPE_NFIELDS (atype) = 1;
	    TYPE_FIELDS (atype) =
		(struct field *) obstack_alloc (symbol_obstack,
						sizeof (struct field));
	    TYPE_FIELD_TYPE (atype, 0) = range_type;
	    return atype;
	}
      case tqVol: return type;
      case tqProc: return lookup_function_type(type);
      default:
	fprintf(stderr, "[Unimplemented type qualifier: %d]\n", qualifier);
	return type;
    }
}

static struct type *
apply_type_modifiers(type, tip, auxp)
     struct type *type;
     TIR *tip;
     AUXU** auxp;
{
    for (;;) {   
	if (tip->tq0 == tqNil) return type;
	type = modify_type(type, tip->tq0, auxp);
	if (tip->tq1 == tqNil) return type;
	type = modify_type(type, tip->tq1, auxp);
	if (tip->tq2 == tqNil) return type;
	type = modify_type(type, tip->tq2, auxp);
	if (tip->tq3 == tqNil) return type;
	type = modify_type(type, tip->tq3, auxp);
	if (tip->tq4 == tqNil) return type;
	type = modify_type(type, tip->tq4, auxp);
	if (tip->tq5 == tqNil) return type;
	type = modify_type(type, tip->tq5, auxp);
	if (!tip->continued) return type;
	tip++;
    }
}

static struct type *
read_type(index, widthp)
     int index;
     int *widthp; /* if non-NULL: set to (if bit field: width, else: 0) */
{
    TIR *tip;
    AUXU *aux;
    struct type *base_type;
    int width;
    if (index == 0xfffff) return builtin_type_int; /* no type info */
/*    if (index < 0 || index > ) ...; */

    aux = &aux_symbol_table[cur_file_descriptor->iauxBase + index];
    tip = &aux->ti;
    for ( ; aux->ti.continued; aux++) ;
    aux++; /* skip last TIR field */

    /* sym.h claims that width comes after RNDX, but seems to be wrong */
    width = tip->fBitfield ? (aux++)->width : 0;
    if (widthp) *widthp = width;

    switch (tip->bt) {
      case btNil: base_type = builtin_type_void; break;
      case btAdr: base_type = lookup_pointer_type(builtin_type_void); break;
      case btChar: base_type = builtin_type_char; break;
      case btUChar: base_type = builtin_type_unsigned_char; break;
      case btShort: base_type = builtin_type_short; break;
      case btUShort: base_type = builtin_type_unsigned_short; break;
      case btInt: base_type = builtin_type_int; break;
      case btUInt: base_type = builtin_type_unsigned_int; break;
      case btLong: base_type = builtin_type_long; break;
      case btULong: base_type = builtin_type_unsigned_long; break;
      case btFloat: base_type = builtin_type_float; break;
      case btDouble: base_type = builtin_type_double; break;
      case btStruct: case btUnion:
	base_type = read_struct_type(&aux, tip->bt); break;
      case btEnum:
	base_type = read_enum_type(&aux); break;
      case btRange:
	base_type = read_range_type(&aux); break;
      case btTypedef:
      case btSet: case btComplex: case btDComplex:
      case btIndirect:
      case btFixedDec: case btFloatDec: case btString:
      case btBit: case btPicture:
      default:
        /*
	 * Don't complain about void types (26). 
	 */
        if (tip->bt != 26) {
	    fprintf(stderr, "[Unimplemented kind of type: %d]\n", tip->bt);
	}
	base_type = builtin_type_void;
    }
    return apply_type_modifiers(base_type, tip, &aux);   
}

static struct symbol *
alloc_symbol(name, value)
  char *name;
  int value;
{
  register struct symbol *sym;
  sym = (struct symbol *)obstack_alloc (symbol_obstack, sizeof(struct symbol));
#ifdef NAMES_HAVE_UNDERSCORE
  if (name[0] == '_') name++;
#endif

  bzero (sym, sizeof (struct symbol));
  SYMBOL_NAME (sym) = obstack_copy0 (symbol_obstack, name, strlen (name));

  /* default assumptions */
  SYMBOL_VALUE (sym) = value;
  SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
  SYMBOL_TYPE (sym) = builtin_type_void;

  return sym;
}

static
read_symbol(csym, name)
  SYMR *csym;
  char *name;
{
  register struct symbol *sym = alloc_symbol(name, csym->value);

  switch (csym->st)
    {
    case stNil: break;
    case stFile:
      last_source_file = obstack_copy0 (symbol_obstack, name, strlen (name));
      break;
    case stGlobal:
      SYMBOL_TYPE (sym) = read_type(csym->index, NULL);
      SYMBOL_CLASS (sym) = LOC_STATIC;
      add_symbol_to_list (sym, &global_symbols);
      break;
    case stStatic:
      SYMBOL_TYPE (sym) = read_type(csym->index, NULL);
      SYMBOL_CLASS (sym) = LOC_STATIC;
      if (within_function) {
	  /* Static symbol of local scope */
	  add_symbol_to_list (sym, &local_symbols);
      }
      else {
	  /* Static symbol at top level of file */
	  add_symbol_to_list (sym, &file_symbols);
      }
      break;

    case stLocal:
      SYMBOL_TYPE (sym) = read_type(csym->index, NULL);
      switch (csym->sc) {
	case scRegister: SYMBOL_CLASS (sym) = LOC_REGISTER; break;
	case scAbs:    SYMBOL_CLASS (sym) = LOC_LOCAL; break;

      }
      add_symbol_to_list (sym, &local_symbols);
      break;

    case stParam:
      SYMBOL_TYPE (sym) = read_type(csym->index, NULL);
      switch (csym->sc) {
	case scAbs:	  SYMBOL_CLASS (sym) = LOC_ARG; break;
	case scRegister:  SYMBOL_CLASS (sym) = LOC_REGPARM; break;
	case scVar:	  SYMBOL_CLASS (sym) = LOC_REF_ARG; break;
	case scVarRegister: SYMBOL_CLASS (sym) = LOC_REGPARM; break; /*WRONG!*/
      }
      add_symbol_to_list (sym, &local_symbols);
      break;

    case stTypedef:
      SYMBOL_CLASS (sym) = LOC_TYPEDEF;
      SYMBOL_TYPE (sym) = read_type(csym->index, NULL);
      if (within_function) {
	  /* Static symbol of local scope */
	  add_symbol_to_list (sym, &local_symbols);
      }
      else {
	  /* Static symbol at top level of file */
	  add_symbol_to_list (sym, &file_symbols);
      }
      break;
    
    case stEnd:
    default: break;
    }
  return 1;
}

static int
read_tagged_block (index)
{
  SYMR *csym = &local_symbol_table[cur_isymBase+index];
  char *name = &local_string_table[cur_issBase+csym->iss];
  struct pending **symlist = within_function ? &local_symbols: &file_symbols;
  register struct symbol *sym;

  /* A kludge to decide if this is an enum definition. */
  if ((csym[1].st == stMember
       && read_type(csym[1].index, 0) == builtin_type_void)
   || (csym->st == stBlockPatched
       && TYPE_CODE((struct type*)(csym->value)) == TYPE_CODE_ENUM))
	{
	  struct type *type = get_enum_type (csym);
	  if (type && TYPE_CODE(type) == TYPE_CODE_ENUM)
	    {
	      int nfields = TYPE_NFIELDS (type);
	      register int i;
	      register struct field *cur_field = TYPE_FIELDS(type);
	      if (name && name[0] && name[0] != '.')
		{
		  sym = alloc_symbol(name, 0);
		  SYMBOL_TYPE (sym) = type;
		  SYMBOL_CLASS (sym) = LOC_TYPEDEF;
		  SYMBOL_NAMESPACE (sym) = STRUCT_NAMESPACE;
		  add_symbol_to_list (sym, symlist);
		}
	      for (i = nfields; --i >= 0; cur_field++)
		{
		  sym = alloc_symbol(cur_field->name, cur_field->bitpos);
		  SYMBOL_CLASS (sym) = LOC_CONST;
		  SYMBOL_TYPE (sym) = type;
		  add_symbol_to_list (sym, symlist);
		}
	      return nfields + 2;
	  }
      }
  else
    {
      if (name && name[0] && name[0] != '.')
	{
	  struct type *type = get_struct_type (csym, btNil);
	  sym = alloc_symbol(name, 0);
	  SYMBOL_TYPE (sym) = type;
	  SYMBOL_CLASS (sym) = LOC_TYPEDEF;
	  SYMBOL_NAMESPACE (sym) = STRUCT_NAMESPACE;
	  add_symbol_to_list (sym, symlist);
	}
      return read_symbols (index + 1) + 2; 
    }
}

/* Read symbols until an stEnd symbol is encountered. Return count. */

static int
read_symbols(index)
     int index; /* relative to current file */
{
  register struct context_stack *new;
  static int blocks_seen = 0;
  int index0 = index;
  SYMR *start_symbol;
  for (;;)
      {
	SYMR *csym = &local_symbol_table[cur_isymBase+index];
	char *name = &local_string_table[cur_issBase+csym->iss];

	if (dump_stuff)
	    printf("[%s,%x,st%s,sc%s,inx:%d (%x)]\n", name, csym->value,
		   MapStNames[csym->st], MapScNames[csym->sc],
		   csym->index, csym);

	switch (csym->st)
	    {
	    case stFile:
	      last_source_file =
		  obstack_copy0 (symbol_obstack, name, strlen (name));
	      index++;
	      index += read_symbols(index) + 1;
	      return index - index0;
	    case stBlock:
	    case stBlockPatched:
	      if (csym->sc == scText)
		{ /* lexical block */
		  if (!local_symbol_table[cur_isymBase+csym->index-1].value)
		    {
		      new = 0;
		    }
		  else
		    {
		      new = (struct context_stack *)
			xmalloc (sizeof (struct context_stack));
		      new->next = context_stack;
		      context_stack = new;
		      new->locals = local_symbols;
		      new->old_blocks = pending_blocks;
		      new->start_addr = cur_proc_addr + csym->value;
		      new->name = 0;
		      local_symbols = 0;
		    }
		  blocks_seen++;
		  start_symbol=csym;
		  index++;
		  index += read_symbols (index);
		  csym = &local_symbol_table[cur_isymBase+index];  /* stEnd */
		  index++;

if (csym->st != stEnd ||
   start_symbol != &local_symbol_table[cur_isymBase+csym->index])
abort();
		  if (new)
		    {
		      if (local_symbols && context_stack->next)
			{
			  /* Make a block for the local symbols within.  */
			  finish_block (0, &local_symbols, new->old_blocks,
					new->start_addr,
					cur_proc_addr + csym->value);
			}
		      local_symbols = new->locals;
		      context_stack = new->next;
		      free (new);
		    }
		  }
	      else if (csym->sc == scInfo)
		  index += read_tagged_block (index);
	      break;
	    case stProc:
	    case stStaticProc:
	      {
		PDR *proc = &proc_desc_table
		    [cur_file_descriptor->ipdFirst+cur_proc_number++];
		CORE_ADDR saved_cur_proc_addr = cur_proc_addr;
		struct symbol *sym = alloc_symbol(name, csym->value);
		struct context_stack *saved_context_stack = context_stack;
		int save_blocks_seen = blocks_seen;
		SYMBOL_CLASS (sym) = LOC_BLOCK;
		/* Add one to csym->index, to the skip the isymMac value
		 * (which is there only for st*Proc). */
		SYMBOL_TYPE (sym) = 
		    lookup_function_type (read_type(csym->index+1, NULL));
		if (csym->st == stStaticProc)
		    add_symbol_to_list (sym, &file_symbols);
		else
		    add_symbol_to_list (sym, &global_symbols);
		cur_proc_addr = csym->value;
		within_function++;
		new = (struct context_stack *)
		    xmalloc (sizeof (struct context_stack));
		new->next = 0;
		context_stack = new;
		new->locals = 0;
		new->old_blocks = pending_blocks;
		new->start_addr = csym->value;
		new->name = sym;
		if (dump_stuff)
		    printf("[PROC:%s,@%x,framereg:%d,froff:%d,rmsk:%d,fmsk:%d,floff:%d,regoff:%d,iline:%d,lnLo:%d,lnHi:%d (%x)]\n",
			   name, proc->adr,
		   proc->framereg, proc->frameoffset, proc->regmask,
		   proc->fregmask, proc->fregoffset,
			   proc->regoffset,
			   proc->iline,proc->lnLow,proc->lnHigh, proc);

		start_symbol = csym;
		index++;
		index += read_symbols(index);

		csym = &local_symbol_table[cur_isymBase+index];/*stEnd symbol*/

if (csym->st != stEnd ||
   start_symbol != &local_symbol_table[cur_isymBase+csym->index])
abort();

		index++;
		PROC_SYMBOL(proc) =
		    blocks_seen == save_blocks_seen ? NULL : sym;
		PROC_LOW_ADDR(proc) = start_symbol->value;
		PROC_HIGH_ADDR(proc) = last_end_addr =
		    start_symbol->value + csym->value;

		finish_block (new->name, &local_symbols, new->old_blocks,
			      start_symbol->value, last_end_addr);
		context_stack = saved_context_stack;
		cur_proc_addr = saved_cur_proc_addr;
		within_function--;
		free (new);
		break;
	      }

	    case stEnd:
#if 1
		    return index - index0;
#else
	      start_symbol = &local_symbol_table[cur_isymBase+csym->index];
	      switch (start_symbol->st)
		  {
		  case stProc:
		  case stStaticProc:
		  case stFile:
		  case stBlock:
		  case stBlockPatched:
		    return index - index0;
		  default:
		    index++;
		  }
	      break;
#endif
	    default:
	      read_symbol(csym, name);
	      index++;
	    }
      }
}

static int
read_coff_symtab (desc, nsyms, symtab_offset)
     int desc;
     int nsyms;
     int symtab_offset;
{
  HDRR hdrr;
  struct coff_symbol coff_symbol;
  register struct coff_symbol *cs = &coff_symbol;
  struct coff_symbol fcn_cs_saved;

  int num_object_files = 0;
  int next_file_symnum = -1;
  char *filestring;
  int fcn_first_line;
  int fcn_last_line;
  int fcn_start_addr;
  long fcn_line_ptr;
  struct cleanup *file_chain, *ext_chain, *old_chain;
  int isym, ifile;

  char *line_number_table;
  EXTR *external_symbol_table;
  char *external_string_table;
  struct linetable **line_vector_all;

  if (myread (desc, (char *)&hdrr, sizeof hdrr) != sizeof hdrr)
    return -1;

  file_descriptor_count = hdrr.ifdMax;
  line_vector_all = (struct linetable**)
      alloca (file_descriptor_count * sizeof(struct linetable *));
  global_symbols_all = (struct pending**)
      alloca (file_descriptor_count * sizeof(struct pending *));
  file_symbols_all = (struct pending**)
      alloca (file_descriptor_count * sizeof(struct pending *));
  proc_desc_length = hdrr.ipdMax;
  proc_desc_table = (PDR*)
    read_table(desc, hdrr.ipdMax * sizeof(PDR), hdrr.cbPdOffset);
  old_chain = make_cleanup (free_all_symtabs, 0);
  make_cleanup (free_proc_descs, 0);
  file_descriptor_table = (FDR*)
    read_table(desc, hdrr.ifdMax * sizeof(FDR), hdrr.cbFdOffset);
  file_chain = make_cleanup (free, file_descriptor_table);

  line_number_table = read_table(desc, hdrr.cbLine, hdrr.cbLineOffset);
  ext_chain = make_cleanup (free, line_number_table);

  for (ifile = 0; ifile < hdrr.ifdMax; ifile++)
    {
      int iproc, ipdMax;
      char *cur_line_entry;
      int cur_addr_offset = 0;
      int cur_addr;

      /* Vector of line number information.  */
      struct linetable *line_vector;

      /* Index of next entry to go in line_vector_index.  */
      int line_vector_index;

      /* Number of elements allocated for line_vector currently.  */
      int line_vector_length;

      global_symbols_all[ifile] = 0;
      file_symbols_all[ifile] = 0;
      select_file (ifile);

      /* read line numbers */
      cur_addr = cur_file_descriptor->adr;
      iproc = cur_file_descriptor->ipdFirst;

      line_vector_index = 0;
      line_vector_length = 1000;
      line_vector = (struct linetable *)
	  xmalloc (sizeof (struct linetable)
		   + line_vector_length * sizeof (struct linetable_entry));

      /* we read the line numbers first, since read_symbol over-writes
	 some of the fields in a proc descriptor */
      ipdMax = iproc + cur_file_descriptor->cpd;
      cur_line_entry = line_number_table + cur_file_descriptor->cbLineOffset;
      for ( ; iproc < ipdMax; iproc++) {
	PDR *proc = &proc_desc_table[iproc];
	int nlines = (iproc+1 < ipdMax ? (proc+1)->iline : cur_file_descriptor->cline)
	    - proc->iline;
	int cur_source_line = proc->lnLow;
	if (dump_stuff)
	    printf(
	    "[proc:%x,framereg:%d,ofset:%d,rmask:%d,flmask:%d,floff:%d,iline:%d,lnLo:%d,lnHi:%d,csl:%d]\n",
	       proc->adr,
		   proc->framereg, proc->frameoffset, proc->regmask,
		   proc->fregmask, proc->fregoffset,
	       proc->iline,proc->lnLow,proc->lnHigh, cur_source_line);
	if (proc->iline == -1) continue;
	for (isym = proc->iline; nlines > 0; isym++) {
	  struct linetable_entry *e;
	  int code = *cur_line_entry++;
	  int delta = code >> 4;
	  int count = (code & 15) + 1;
	  if (dump_stuff & 1) printf("%2x", 0xFF & code);
	  if (delta == -8)
	    {
	      if (dump_stuff & 1)
	        printf(" %2x%2x", 0xff & cur_line_entry[0],
		     0xff & cur_line_entry[1]);
	      delta = *cur_line_entry++;
	      delta = (delta << 8) | (unsigned)*cur_line_entry++;
	      isym += 2;
	    }
	  cur_source_line += delta;
	  if (dump_stuff & 1)
	    printf("\t%2d.%2d li:%d %X", delta, count,
		 cur_source_line,
		 cur_file_descriptor->adr + 4 * cur_addr_offset);

	  e = &line_vector->item[line_vector_index];
	  if (line_vector_index == 0 || e[-1].line != cur_source_line)
	    {

	      /* Make sure line vector is big enough.  */
	      if (line_vector_index + 2 >= line_vector_length)
		{
		  line_vector_length *= 2;
		  line_vector = (struct linetable *)
		    xrealloc (line_vector, sizeof (struct linetable)
			      + (line_vector_length
				 * sizeof (struct linetable_entry)));
		  e = &line_vector->item[line_vector_index];
		}

	      e->line = cur_source_line;
	      e->pc = cur_file_descriptor->adr + 4 * cur_addr_offset;
	      line_vector_index++;
	    }

	  if (delta != 0)
	    {
	      int save_addr = cur_addr;
	      if (cur_addr_offset != 0)
		{
		  int toffset = (cur_addr_offset - 1) * 4;
		  if (dump_stuff & 1)
		  printf(" [li: %d %x-%x]",
			 cur_source_line, cur_addr, cur_addr+toffset);
		}
	      cur_addr = save_addr + cur_addr_offset * 4;
	    }
	  cur_addr_offset += count;
	  nlines -= count;
	  if (dump_stuff & 1) putchar('\n');
	/*  if (delta != 0) { } */
        }
      }

      line_vector->nitems = line_vector_index;
      line_vector_all[ifile] = (struct linetable *)
	  xrealloc (line_vector, (sizeof (struct linetable)
		       + line_vector_index * sizeof (struct linetable_entry)));
    }
  do_cleanups (ext_chain);

  aux_symbol_table = (AUXU*)
    read_table(desc, hdrr.iauxMax * sizeof(AUXU), hdrr.cbAuxOffset);
  make_cleanup (free, aux_symbol_table);
  rel_file_table = (RFDT*)
    read_table(desc, hdrr.crfd * sizeof(RFDT), hdrr.cbRfdOffset);
  make_cleanup (free, rel_file_table);
  local_symbol_table = (SYMR*)
    read_table(desc, hdrr.isymMax * sizeof(SYMR), hdrr.cbSymOffset);
  make_cleanup (free, local_symbol_table);
  local_string_table = read_table(desc, hdrr.issMax, hdrr.cbSsOffset);
  make_cleanup (free, local_string_table);

  external_symbol_table = (EXTR*)
    read_table(desc, hdrr.iextMax * sizeof(EXTR), hdrr.cbExtOffset);
  ext_chain = make_cleanup (free, external_symbol_table);
  external_string_table = read_table(desc, hdrr.issExtMax, hdrr.cbSsExtOffset);
  make_cleanup (free, external_string_table);

  /* do the external symbols first */
  /* The reason is that each file's end_symtab is done with the locals */

  for (isym = 0; isym < hdrr.iextMax; isym++)
    {
      EXTR *ext = &external_symbol_table[isym];
      SYMR *csym = &ext->asym;
      char *name = external_string_table+csym->iss;
      if ((unsigned)ext->ifd >= hdrr.ifdMax) return -1;
      select_file (ext->ifd);
      record_misc_function (name, csym->value);
      if (csym->st != stProc && csym->st != stStaticProc)
	  read_symbol(csym, name);
#if 1
if (dump_stuff)
      printf("EXT[%s,%x,st%s,sc%s,inx:%d,ifd:%d]\n",
	     name, csym->value, MapStNames[csym->st],
	     MapScNames[csym->sc], csym->index, ext->ifd);
#endif
    }

  do_cleanups (ext_chain);

  for (ifile = 0; ifile < hdrr.ifdMax; ifile++)
    {
      select_file (ifile);

      cur_proc_number = 0;
      last_end_addr = cur_file_descriptor->adr;

      start_symtab ();

if (dump_stuff)
printf("<FILE.adr:%x,ilB:%d,cb:%d,cbLO:%d,cbL:%d,iSB:%d,cS:%d,nProcs:%d,first:%d>\n",
       cur_file_descriptor->adr,
       cur_file_descriptor->ilineBase, cur_file_descriptor->cline,
       cur_file_descriptor->cbLineOffset, cur_file_descriptor->cbLine,
       cur_isymBase, cur_file_descriptor->csym,
       cur_file_descriptor->cpd,
       cur_file_descriptor->ipdFirst);

      for (isym = 0; isym < cur_file_descriptor->csym; )
	  {
	    int count = read_symbols(isym);
	    if (count == 0)
		{ /* handle un-handled stEnd */
		  SYMR *sym = &local_symbol_table[cur_isymBase+isym];
		  char *name = &local_string_table[cur_issBase+sym->iss];
printf("stEnd confusion!");
		  read_symbol(sym, name);
		  count = 1;
		}
	    isym += count;
	  }
      end_symtab (cur_file_descriptor->adr, last_end_addr,
		  line_vector_all[ifile]);
    }

  do_cleanups (file_chain);

  last_source_file = 0;
  bzero (opaque_type_chain, sizeof opaque_type_chain);

  type_vector_length = 160;
  type_vector = (struct typevector *)
		xmalloc (sizeof (struct typevector)
				+ type_vector_length * sizeof (struct type *));
  bzero (type_vector->type, type_vector_length * sizeof (struct type *));

  if (last_source_file)
    end_symtab ();
  discard_cleanups (old_chain);
  return 0;
}

/* Routines for reading headers and symbols from executable.  */

/* Read COFF file header, check magic number,
   and return number of symbols. */
read_file_hdr (chan, file_hdr)
    int chan;
    FILHDR *file_hdr;
{
  lseek (chan, 0L, 0);
  if (myread (chan, (char *)file_hdr, FILHSZ) < 0)
    return -1;

  switch (file_hdr->f_magic)
    {
#ifdef MIPSEBMAGIC
    case MIPSEBMAGIC:
#endif
#ifdef MIPSELMAGIC
    case MIPSELMAGIC:
#endif
#ifdef MC68MAGIC
    case MC68MAGIC:
#endif
#ifdef NS32GMAGIC
      case NS32GMAGIC:
      case NS32SMAGIC:
#endif
#ifdef I386MAGIC
    case I386MAGIC:
#endif
#ifdef CLIPPERMAGIC
    case CLIPPERMAGIC:
#endif      
	return file_hdr->f_nsyms;

      default:
#ifdef BADMAG
	if (BADMAG(file_hdr))
	  return -1;
	else
	  return file_hdr->f_nsyms;
#else
	return -1;
#endif
    }
}

read_aout_hdr (chan, aout_hdr, size)
    int chan;
    AOUTHDR *aout_hdr;
    int size;
{
  lseek (chan, (long)FILHSZ, 0);
  if (size != sizeof (AOUTHDR))
    return -1;
  if (myread (chan, (char *)aout_hdr, size) != size)
    return -1;
  return 0;
}

read_section_hdr (chan, section_name, section_hdr, nsects)
    register int chan;
    register char *section_name;
    SCNHDR *section_hdr;
    register int nsects;
{
  register int i;

  if (lseek (chan, FILHSZ + sizeof (AOUTHDR), 0) < 0)
    return -1;

  for (i = 0; i < nsects; i++)
    {
      if (myread (chan, (char *)section_hdr, SCNHSZ) < 0)
	return -1;
      if (strncmp (section_hdr->s_name, section_name, 8) == 0)
	return 0;
    }
    return -1;
}


/* Support for string table handling */

static char *stringtab = NULL;


#if 0
static char *
getsymname (symbol_entry)
    SYMENT *symbol_entry;
{
  static char buffer[SYMNMLEN+1];
  char *result;

  if (symbol_entry->n_zeroes == 0)
    {
      result = stringtab + symbol_entry->n_offset;
    }
  else
    {
      strncpy (buffer, symbol_entry->n_name, SYMNMLEN);
      buffer[SYMNMLEN] = '\0';
      result = buffer;
    }
  return result;
}
#endif

/* Support for line number handling */
static char *linetab = NULL;
static long linetab_offset;
static int linetab_count;

static int
hashname (name)
     char *name;
{
  register char *p = name;
  register int total = p[0];
  register int c;

  c = p[1];
  total += c << 2;
  if (c)
    {
      c = p[2];
      total += c << 4;
      if (c)
	total += p[3] << 6;
    }
  
  return total % HASHSIZE;
}

static void
patch_type (type, real_type)
    struct type *type;
    struct type *real_type;
{
  register struct type *target = TYPE_TARGET_TYPE (type);
  register struct type *real_target = TYPE_TARGET_TYPE (real_type);
  int field_size = TYPE_NFIELDS (real_target) * sizeof (struct field);

  TYPE_LENGTH (target) = TYPE_LENGTH (real_target);
  TYPE_NFIELDS (target) = TYPE_NFIELDS (real_target);
  TYPE_FIELDS (target) = (struct field *)
				obstack_alloc (symbol_obstack, field_size);

  bcopy (TYPE_FIELDS (real_target), TYPE_FIELDS (target), field_size);

  if (TYPE_NAME (real_target))
    {
      if (TYPE_NAME (target))
	free (TYPE_NAME (target));
      TYPE_NAME (target) = concat (TYPE_NAME (real_target), "", "");
    }
}

/* Patch up all appropriate typdef symbols in the opaque_type_chains
   so that they can be used to print out opaque data structures properly */

static void
patch_opaque_types ()
{
  struct symtab *s;

  /* Look at each symbol in the per-file block of each symtab.  */
  for (s = symtab_list; s; s = s->next)
    {
      register struct block *b;
      register int i;

      /* Go through the per-file symbols only */
      b = BLOCKVECTOR_BLOCK (BLOCKVECTOR (s), 1);
      for (i = BLOCK_NSYMS (b) - 1; i >= 0; i--)
	{
	  register struct symbol *real_sym;

	  /* Find completed typedefs to use to fix opaque ones.
	     Remove syms from the chain when their types are stored,
	     but search the whole chain, as there may be several syms
	     from different files with the same name.  */
	  real_sym = BLOCK_SYM (b, i);
	  if (SYMBOL_CLASS (real_sym) == LOC_TYPEDEF &&
	      SYMBOL_NAMESPACE (real_sym) == VAR_NAMESPACE &&
	      TYPE_CODE (SYMBOL_TYPE (real_sym)) == TYPE_CODE_PTR &&
	      TYPE_LENGTH (TYPE_TARGET_TYPE (SYMBOL_TYPE (real_sym))) != 0)
	    {
	      register char *name = SYMBOL_NAME (real_sym);
	      register int hash = hashname (name);
	      register struct symbol *sym, *prev;

	      prev = 0;
	      for (sym = opaque_type_chain[hash]; sym;)
		{
		  if (name[0] == SYMBOL_NAME (sym)[0] &&
		      !strcmp (name + 1, SYMBOL_NAME (sym) + 1))
		    {
		      if (prev)
			SYMBOL_VALUE (prev) = SYMBOL_VALUE (sym);
		      else
			opaque_type_chain[hash]
			  = (struct symbol *) SYMBOL_VALUE (sym);

		      patch_type (SYMBOL_TYPE (sym), SYMBOL_TYPE (real_sym));

		      if (prev)
			sym = (struct symbol *) SYMBOL_VALUE (prev);
		      else
			sym = opaque_type_chain[hash];
		    }
		  else
		    {
		      prev = sym;
		      sym = (struct symbol *) SYMBOL_VALUE (sym);
		    }
		}
	    }
	}
    }
}

/* This function is really horrible, but to avoid it, there would need
   to be more filling in of forward references.  THIS SHOULD BE MOVED
   OUT OF COFFREAD.C AND DBXREAD.C TO SOME PLACE WHERE IT CAN BE SHARED. */
int
fill_in_vptr_fieldno (type)
     struct type *type;
{
  if (TYPE_VPTR_FIELDNO (type) < 0)
    TYPE_VPTR_FIELDNO (type) =
      fill_in_vptr_fieldno (TYPE_BASECLASS (type, 1));
  return TYPE_VPTR_FIELDNO (type);
}

/* partial symbol tables are not implemented in coff, therefore
   block_for_pc() (and others) will never decide to call this. */

extern struct symtab *
psymtab_to_symtab ()
{
  fatal ("error: Someone called psymtab_to_symtab\n");
}

/* These will stay zero all the time */
struct psymbol_allocation_list global_psymbols, static_psymbols;
