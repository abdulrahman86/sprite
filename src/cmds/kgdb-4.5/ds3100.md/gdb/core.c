/* Work with core dump and executable files, for GDB.
   Copyright 1986, 1987, 1989, 1991, 1992 Free Software Foundation, Inc.

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

#include "defs.h"
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include "frame.h"  /* required by inferior.h */
#include "inferior.h"
#include "symtab.h"
#include "command.h"
#include "bfd.h"
#include "target.h"
#include "gdbcore.h"
#include "./../bfd/vm-core.h"
#include <kernel/dbg.h>
#include <kernel/machTypes.h>
#include <kernel/procTypes.h>

CORE_ADDR data_start;
CORE_ADDR data_end;
CORE_ADDR stack_start;
CORE_ADDR stack_end;
CORE_ADDR text_start;
CORE_ADDR text_end;
CORE_ADDR exec_data_start;
CORE_ADDR exec_data_end;

#ifdef SOLIB_ADD
static int 
solib_add_stub PARAMS ((char *));
#endif

static void
core_close PARAMS ((int));

static void
core_open PARAMS ((char *, int));

static void
core_detach PARAMS ((char *, int));

static void
get_core_registers PARAMS ((int));

static void
core_files_info PARAMS ((struct target_ops *));

extern int sys_nerr;
extern char *sys_errlist[];
extern char *sys_siglist[];
extern StopInfo remoteStopInfo;
extern int lastPid;

extern char registers[];

/* Hook for `exec_file_command' command to call.  */

void (*exec_file_display_hook) PARAMS ((char *)) = NULL;

/* Binary file diddling handle for the core file.  */

bfd *core_bfd = NULL;

/* Forward decl */
extern struct target_ops core_ops;


/* Discard all vestiges of any previous core file
   and mark data and stack spaces as empty.  */

/* ARGSUSED */
static void
core_close (quitting)
     int quitting;
{
  if (core_bfd) {
    free (bfd_get_filename (core_bfd));
    bfd_close (core_bfd);
    core_bfd = NULL;
#ifdef CLEAR_SOLIB
    CLEAR_SOLIB ();
#endif
    if (core_ops.to_sections) {
      free ((PTR)core_ops.to_sections);
      core_ops.to_sections = NULL;
      core_ops.to_sections_end = NULL;
    }
  }
}

#ifdef SOLIB_ADD
/* Stub function for catch_errors around shared library hacking. */

static int 
solib_add_stub (from_tty)
     char *from_tty;
{
    SOLIB_ADD (NULL, (int)from_tty, &core_ops);
    return 0;
}
#endif /* SOLIB_ADD */

/* This routine opens and sets up the core file bfd */

static void
core_open (filename, from_tty)
     char *filename;
     int from_tty;
{
  const char *p;
  int siggy;
  struct cleanup *old_chain;
  char *temp;
  bfd *temp_bfd;
  int ontop;
  int scratch_chan;

  target_preopen (from_tty);
  if (!filename)
    {
      error (core_bfd? 
       "No core file specified.  (Use `detach' to stop debugging a core file.)"
     : "No core file specified.");
    }

  filename = tilde_expand (filename);
  if (filename[0] != '/') {
    temp = concat (current_directory, "/", filename, NULL);
    free (filename);
    filename = temp;
  }

  old_chain = make_cleanup (free, filename);

  scratch_chan = open (filename, write_files? O_RDWR: O_RDONLY, 0);
  if (scratch_chan < 0)
    perror_with_name (filename);

  temp_bfd = bfd_fdopenr (filename, NULL, scratch_chan);
  if (temp_bfd == NULL)
    {
      perror_with_name (filename);
    }

  if (!bfd_check_format (temp_bfd, bfd_core))
    {
      /* Do it after the err msg */
      make_cleanup (bfd_close, temp_bfd);
      error ("\"%s\" is not a core dump: %s", filename, bfd_errmsg(bfd_error));
    }

  /* Looks semi-reasonable.  Toss the old core file and work on the new.  */

  discard_cleanups (old_chain);		/* Don't free filename any more */
  unpush_target (&core_ops);
  core_bfd = temp_bfd;
  old_chain = make_cleanup (core_close, core_bfd);

  validate_files ();

  /* Find the data section */
  if (build_section_table (core_bfd, &core_ops.to_sections,
			   &core_ops.to_sections_end))
    error ("Can't find sections in `%s': %s", bfd_get_filename(core_bfd),
	   bfd_errmsg (bfd_error));

  ontop = !push_target (&core_ops);
  discard_cleanups (old_chain);

  p = bfd_core_file_failing_command (core_bfd);
  if (p)
    printf ("Core was generated by `%s'.\n", p);

  siggy = bfd_core_file_failing_signal (core_bfd);
  if (siggy > 0)
    printf ("Program terminated with signal %d, %s.\n", siggy,
	    siggy < NSIG ? sys_siglist[siggy] : "(undocumented)");

  if (ontop) {
    /* Fetch all registers from core file */
    target_fetch_registers (-1);

    /* Add symbols and section mappings for any shared libraries */
#ifdef SOLIB_ADD
    (void) catch_errors (solib_add_stub, (char *)from_tty, (char *)0);
#endif
    
    bcopy(bfd_get_vm_core_stopinfo(core_bfd), &remoteStopInfo, sizeof remoteStopInfo);
    supply_register(FP_REGNUM, &remoteStopInfo.regs.regs[29]);
    supply_register(SP_REGNUM, &remoteStopInfo.regs.regs[29]);
    supply_register(PC_REGNUM, &remoteStopInfo.regs.pc);
    /* Now, set up the frame cache, and print the top of stack */
    set_current_frame (create_new_frame (read_register (FP_REGNUM),
					 read_pc ()));
    select_frame (get_current_frame (), 0);
    print_stack_frame (selected_frame, selected_frame_level, 1);
  } else {
    printf (
"Warning: you won't be able to access this core file until you terminate\n\
your %s; do ``info files''\n", current_target->to_longname);
  }
}

int
core_attach(pid)
    int	pid;
{
  int	status;
  struct expression *expr;
  register struct cleanup *old_chain;
  register value val;
  int	machRegStateAddr;
  Proc_ControlBlock	*procPtr;
  Mach_RegState machRegState;
  char	exp[128];
  
  if (pid != lastPid) {
    /*
     * Switching to a new process. 
     */
    if (pid == -1) {
      /*
       * Process -1 is the trap process. Whose stop info is 
       * in the beginning of the corefile.
       */
      bcopy(bfd_get_vm_core_stopinfo(core_bfd), &remoteStopInfo, sizeof remoteStopInfo);
      supply_register(FP_REGNUM, &remoteStopInfo.regs.regs[29]);
      supply_register(SP_REGNUM, &remoteStopInfo.regs.regs[29]);
      supply_register(PC_REGNUM, &remoteStopInfo.regs.pc);
      /* Now, set up the frame cache, and print the top of stack */
      set_current_frame (create_new_frame (read_register (FP_REGNUM),
					   read_pc ()));
      select_frame (get_current_frame (), 0);
      print_stack_frame (selected_frame, selected_frame_level, 1);
      lastPid = pid;
      return 1;
    }
    /*
     * Lookup the switch regs of the specified process.
     * They are in proc_PCBTable[pidSlot]->machStatePtr->switchRegs.
     */
    sprintf(exp,"proc_PCBTable[%d]", pid & 0xff);
    expr = parse_expression (exp);
    old_chain = make_cleanup (free_current_contents, &expr);
    
    val = evaluate_expression (expr);
    do_cleanups (old_chain);
    if ((value_as_long(val) == NIL) || (value_as_long(val) == 0)) {
      error("Pid 0x%x does not have a control block\n", pid);
    }
    sprintf(exp,"&(proc_PCBTable[%d]->machStatePtr->switchRegState)", pid & 0xff);
    expr = parse_expression (exp);
    old_chain = make_cleanup (free_current_contents, &expr);
    val = evaluate_expression (expr);
    do_cleanups (old_chain);
    machRegStateAddr = value_as_long(val);
    machRegState.pc = 320;
    if (target_xfer_memory(machRegStateAddr, &machRegState, 
			  sizeof(Mach_RegState), 0) != 0) {
      error("Can't read  regs from address 0x%x\n", machRegStateAddr);
    }
    remoteStopInfo.regs = machRegState;
    /*
     * The PC that is stored for a switch registers from on the
     * sun4 is bogus.  We set the PC to be inside Mach_ContextSwitch.
     */
    strcpy(exp,"&Mach_ContextSwitch");
    expr = parse_expression (exp);
    old_chain = make_cleanup (free_current_contents, &expr);
    val = evaluate_expression (expr);
    do_cleanups (old_chain);
    remoteStopInfo.regs.pc = value_as_long(val)+16;
    remoteStopInfo.regs.regs[29] = remoteStopInfo.regs.regs[29];
/*    supply_register(FP_REGNUM, &remoteStopInfo.regs.regs[29]);*/
    supply_register(SP_REGNUM, &remoteStopInfo.regs.regs[29]);
    supply_register(PC_REGNUM, &remoteStopInfo.regs.pc);
    /* Now, set up the frame cache, and print the top of stack */
    set_current_frame (create_new_frame (read_register (FP_REGNUM),
					 read_pc ()));
    select_frame (get_current_frame (), 0);
    print_stack_frame (selected_frame, selected_frame_level, 1);
    lastPid = pid;
  }
  return 1;
}

static void
core_detach (args, from_tty)
     char *args;
     int from_tty;
{
  if (args)
    error ("Too many arguments");
  unpush_target (&core_ops);
  if (from_tty)
    printf ("No core file now.\n");
}

/* Backward compatability with old way of specifying core files.  */

void
core_file_command (filename, from_tty)
     char *filename;
     int from_tty;
{
  dont_repeat ();			/* Either way, seems bogus. */
  if (!filename)
    core_detach (filename, from_tty);
  else
    core_open (filename, from_tty);
}


/* Call this to specify the hook for exec_file_command to call back.
   This is called from the x-window display code.  */

void
specify_exec_file_hook (hook)
     void (*hook) PARAMS ((char *));
{
  exec_file_display_hook = hook;
}

/* The exec file must be closed before running an inferior.
   If it is needed again after the inferior dies, it must
   be reopened.  */

void
close_exec_file ()
{
#ifdef FIXME
  if (exec_bfd)
    bfd_tempclose (exec_bfd);
#endif
}

void
reopen_exec_file ()
{
#ifdef FIXME
  if (exec_bfd)
    bfd_reopen (exec_bfd);
#endif
}

/* If we have both a core file and an exec file,
   print a warning if they don't go together.  */

void
validate_files ()
{
  if (exec_bfd && core_bfd)
    {
      if (!core_file_matches_executable_p (core_bfd, exec_bfd))
	printf ("Warning: core file may not match specified executable file.\n");
      else if (bfd_get_mtime(exec_bfd) > bfd_get_mtime(core_bfd))
	printf ("Warning: exec file is newer than core file.\n");
    }
}

/* Return the name of the executable file as a string.
   ERR nonzero means get error if there is none specified;
   otherwise return 0 in that case.  */

char *
get_exec_file (err)
     int err;
{
  if (exec_bfd) return bfd_get_filename(exec_bfd);
  if (!err)     return NULL;

  error ("No executable file specified.\n\
Use the \"file\" or \"exec-file\" command.");
  return NULL;
}

static void
core_files_info (t)
  struct target_ops *t;
{
  print_section_info (t, core_bfd);
}

/* Report a memory error with error().  */

void
memory_error (status, memaddr)
     int status;
     CORE_ADDR memaddr;
{

  if (status == EIO)
    {
      /* Actually, address between memaddr and memaddr + len
	 was out of bounds. */
      error ("Cannot access memory at address %s.", local_hex_string(memaddr));
    }
  else
    {
      if (status >= sys_nerr || status < 0)
	error ("Error accessing memory address %s: unknown error (%d).",
	       local_hex_string(memaddr), status);
      else
	error ("Error accessing memory address %s: %s.",
	       local_hex_string(memaddr), sys_errlist[status]);
    }
}

/* Same as target_read_memory, but report an error if can't read.  */
void
read_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int status;
  status = target_read_memory (memaddr, myaddr, len);
  if (status != 0)
    memory_error (status, memaddr);
}

/* Same as target_write_memory, but report an error if can't write.  */
void
write_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  int status;

  status = target_write_memory (memaddr, myaddr, len);
  if (status != 0)
    memory_error (status, memaddr);
}

/* Read an integer from debugged memory, given address and number of bytes.  */

long
read_memory_integer (memaddr, len)
     CORE_ADDR memaddr;
     int len;
{
  char cbuf;
  short sbuf;
  int ibuf;
  long lbuf;

  if (len == sizeof (char))
    {
      read_memory (memaddr, &cbuf, len);
      return cbuf;
    }
  if (len == sizeof (short))
    {
      read_memory (memaddr, (char *)&sbuf, len);
      SWAP_TARGET_AND_HOST (&sbuf, sizeof (short));
      return sbuf;
    }
  if (len == sizeof (int))
    {
      read_memory (memaddr, (char *)&ibuf, len);
      SWAP_TARGET_AND_HOST (&ibuf, sizeof (int));
      return ibuf;
    }
  if (len == sizeof (lbuf))
    {
      read_memory (memaddr, (char *)&lbuf, len);
      SWAP_TARGET_AND_HOST (&lbuf, sizeof (lbuf));
      return lbuf;
    }
  error ("Cannot handle integers of %d bytes.", len);
  return -1;	/* for lint */
}

/* Get the registers out of a core file.  This is the machine-
   independent part.  Fetch_core_registers is the machine-dependent
   part, typically implemented in the xm-file for each architecture.  */

/* We just get all the registers, so we don't use regno.  */
/* ARGSUSED */
static void
get_core_registers (regno)
     int regno;
{
  sec_ptr reg_sec;
  unsigned size;
  char *the_regs;

  the_regs = (char *) bfd_get_vm_core_regs (core_bfd);
  fetch_core_registers (the_regs, size, 0, NULL);
  
  registers_fetched();
}

struct target_ops core_ops = {
	"core", "Local core dump file",
	"Use a core file as a target.  Specify the filename of the core file.",
	core_open, core_close,
	core_attach, core_detach, 0, 0, /* resume, wait */
	get_core_registers, 
	0, 0, 0, 0, /* store_regs, prepare_to_store, conv_to, conv_from */
	xfer_memory, core_files_info,
	0, 0, /* core_insert_breakpoint, core_remove_breakpoint, */
	0, 0, 0, 0, 0, /* terminal stuff */
	0, 0, 0, /* kill, load, lookup sym */
	child_create_inferior, 0, /* mourn_inferior */
	core_stratum, 0, /* next */
	0, 1, 1, 1, 0,	/* all mem, mem, stack, regs, exec */
	0, 0,			/* section pointers */
	OPS_MAGIC,		/* Always the last thing */
};

void
_initialize_core()
{

  add_com ("core-file", class_files, core_file_command,
	   "Use FILE as core dump for examining memory and registers.\n\
No arg means have no core file.  This command has been superseded by the\n\
`target core' and `detach' commands.");
  add_target (&core_ops);
}
