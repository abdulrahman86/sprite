/* 
 * sysCalls.c --
 *
 *	Miscellaneous system calls that are lumped under the Sys_ prefix.
 *
 * Copyright 1986 Regents of the University of California
 * All rights reserved.
 */

#ifndef lint
static char rcsid[] = "$Header: /cdrom/src/kernel/Cvsroot/kernel/sys/sysCalls.c,v 9.50 92/09/21 11:35:01 mgbaker Exp $ SPRITE (Berkeley)";
#endif not lint


#include <sprite.h>
#include <mach.h>
#include <sys.h>
#include <sysInt.h>
#include <sysStats.h>
#include <time.h>
#include <timer.h>
#include <vm.h>
#include <machMon.h>
#include <proc.h>
#include <dbg.h>
#include <fs.h>
#include <fsutil.h>
#include <fsprefix.h>
#include <rpc.h>
#include <net.h>
#include <sched.h>
#include <dev.h>
#include <recov.h>
#include <recovBox.h>
#include <procMigrate.h>
#include <string.h>
#include <stdio.h>
#include <main.h>
#ifdef sun4
#include <vmMach.h>
#endif
#ifdef sun4c
#include <devSCSIC90.h>
#endif /* sun4c */
#include <user/sys/param.h>
#include <fsrecov.h>

Boolean	sys_ErrorShutdown = FALSE;
Boolean	sys_ShuttingDown = FALSE;

char	sys_HostName[MAXHOSTNAMELEN];

/*
 *----------------------------------------------------------------------
 *
 * Sys_GetTimeOfDay --
 *
 *	Returns the current system time to a local user process.
 *	If any argument is USER_NIL, that value is not returned.
 *
 *	The "real" time of day is returned, rather than the software
 *	time.
 *
 * Results:
 *	SUCCESS 		The call was successful.
 *	SYS_ARG_NOACCESS	The user arguments were not accessible.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

ReturnStatus
Sys_GetTimeOfDay(timePtr, localOffsetPtr, DSTPtr)
    Time	*timePtr;		/* Buffer to store the UT time. */
    int		*localOffsetPtr;	/* Buffer to store the number of minutes
					 * from UT (a negative value means you 
					 * are west of the Prime Meridian. */
    Boolean	*DSTPtr;		/* Buffer to store a flag that's TRUE
					 * if DST is followed. */
{
    Time	curTime;
    int		curLocalOffset;
    Boolean	curDST;

    Timer_GetRealTimeOfDay(&curTime, &curLocalOffset, &curDST);

    if (timePtr != USER_NIL) {
	if (Proc_ByteCopy(FALSE, sizeof(Time),
	      (Address) &curTime, (Address) timePtr) != SUCCESS) {
	    return(SYS_ARG_NOACCESS);
	}
    }
    if (localOffsetPtr != USER_NIL) {
	if (Proc_ByteCopy(FALSE, sizeof(int),(Address) &curLocalOffset, 
				(Address) localOffsetPtr) != SUCCESS) {
	    return(SYS_ARG_NOACCESS);
	}
    }
    if (DSTPtr != USER_NIL) {
	if (Proc_ByteCopy(FALSE, sizeof(Boolean),(Address) &curDST,
					(Address) DSTPtr) != SUCCESS) {
	    return(SYS_ARG_NOACCESS);
	}
    }
    return(SUCCESS);
}

/*
 *----------------------------------------------------------------------
 *
 * Sys_SetTimeOfDay --
 *
 *	Changes the current system time to the value specified by
 *	the arguments. The timePtr argument  must be valid.
 *
 * Results:
 *	SUCCESS 		The call was successful.
 *	SYS_ARG_NOACCESS	The user arguments were not accessible.
 *
 * Side effects:
 *	The system time is updated to a new value.
 *
 *----------------------------------------------------------------------
 */

ReturnStatus
Sys_SetTimeOfDay(timePtr, localOffset, DST)
    Time	*timePtr;	/* New value for the UT (GMT) time. */
    int		localOffset;	/* new value for the offset in minutes 
				 * from UT.*/
    Boolean	DST;		/* If TRUE, DST is used at this site. */
{
    Time	newTime;

    if (timePtr == USER_NIL || 
        (Proc_ByteCopy(TRUE, sizeof(Time), (Address) timePtr, 
			(Address) &newTime) != SUCCESS)) {
	return(SYS_ARG_NOACCESS);
    }
    Timer_SetTimeOfDay(newTime, localOffset, DST);
    return(SUCCESS);
}


/*
 *----------------------------------------------------------------------
 *
 * Sys_GetHostName --
 *
 *	Stub for the Sys_GetHostName system call.
 *	Returns the name of the host.
 *
 * Results:
 *	SUCCESS 		The call was successful.
 *	SYS_ARG_NOACCESS	The user arguments were not accessible.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

ReturnStatus
Sys_GetHostName(namePtr)
    char	*namePtr;		/* Buffer to store host name. */
{
    int		nameLen;

    nameLen = strlen(sys_HostName);
    if (Proc_ByteCopy(FALSE, nameLen+1, (Address) sys_HostName, 
	(Address) namePtr) != SUCCESS) {

	return SYS_ARG_NOACCESS;
    }
    return(SUCCESS);
}

/*
 *----------------------------------------------------------------------
 *
 * Sys_SetHostName --
 *
 *	Stub for the Sys_SetHostName system call.
 *	Sets the name of the host. Only the root can make this call.
 *
 * Results:
 *	SUCCESS 		The call was successful.
 *	SYS_ARG_NOACCESS	The user arguments were not accessible.
 *	SYS_INVALID_ARG		The name was too long.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

ReturnStatus
Sys_SetHostName(namePtr)
    char	*namePtr;		/* Buffer to store host name. */
{
    char		tmp[MAXHOSTNAMELEN+1];
    int			length;
    Proc_ControlBlock 	*procPtr;

    procPtr = Proc_GetEffectiveProc();
    if (procPtr->effectiveUserID != 0) {
	return GEN_NO_PERMISSION;
    }
    if (Proc_StringNCopy(MAXHOSTNAMELEN+1, namePtr, tmp, &length) != SUCCESS) {
	return SYS_ARG_NOACCESS;
    }
    /*
     * Make sure that the name wasn't too long.
     */
    if (length == MAXHOSTNAMELEN+1) {
	return SYS_INVALID_ARG;
    }
    strcpy(sys_HostName, tmp);
    return(SUCCESS);
}

/*
 *----------------------------------------------------------------------
 *
 * Sys_DoNothing --
 *
 *	This system call simply returns SUCCESS. It does not perform any
 *	function.
 *
 * Results:
 *	SUCCESS 		This value is always returned.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

ReturnStatus
Sys_DoNothing()
{
    Sched_ContextSwitch(PROC_READY);
    return(SUCCESS);
}


#define	MAX_WAIT_INTERVALS	3
Boolean	shutdownDebug = FALSE;

/*
 *----------------------------------------------------------------------
 *
 * Sys_Shutdown --
 *
 *	This system call calls appropriate routines to shutdown
 *	the system in an orderly fashion.
 *
 * Results:
 *	SUCCESS 	This value is always returned.
 *	SYS_ARG_NOACESS	The reboot string was inaccessible.	
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

ReturnStatus
Sys_Shutdown(flags, rebootString)
    int		flags;
    char	*rebootString;
{
    Time		waitTime;
    int			alive;
    int			timesWaited;
    Boolean		userDead = FALSE;
    char		string[100];
    int			accLength;
    int			strLength;
    ReturnStatus	status;
    Proc_ControlBlock	*procPtr;


    if (flags & SYS_REBOOT) {
	if (rebootString == (char *) USER_NIL) {
	    string[0] = '\0';
	} else {
	    status = Proc_MakeStringAccessible(100, &rebootString, &accLength,
						    &strLength);
	    if (status != SUCCESS) {
		return(SYS_ARG_NOACCESS);
	    }
	    (void) strcpy(string, rebootString);
	    Proc_MakeUnaccessible(rebootString, accLength);
	}
    }

    if (flags & SYS_DEBUG) {
	sys_ErrorShutdown = TRUE;
    }

    if (flags & SYS_KILL_PROCESSES) {
	if (flags & SYS_WRITE_BACK) { 
	    /*
	     * Do a few initial syncs.
	     * These are necessary because the cache isn't getting written
	     * out properly with the new block cleaner.
	     */
	    printf("Doing initial syncs\n");
	    Fsutil_Sync(-1, 0);
	    Fsutil_Sync(-1, 0);
	    Fsutil_Sync(-1, 0);
	    printf("Done initial syncs\n");
	}
	/*
	 * Turn ourselves into a kernel process since we no longer need
	 * user process resources.
	 */
	procPtr = Proc_GetCurrentProc();
	Proc_Lock(procPtr);
#ifdef sun4
	/*
	 * Flush the virt addresses cache on the sun4 before turning ourselves
	 * into a kernel process. If we don't do this we will get cache
	 * write-back errors from dirty cache blocks of the shutdown program.
	 */
	Mach_FlushWindowsToStack();
	VmMach_FlushCurrentContext();
#endif
	procPtr->genFlags &= ~PROC_USER;
	procPtr->genFlags |= PROC_KERNEL;
	Proc_Unlock(procPtr);
	VmMach_ReinitContext(procPtr);

	/*
	 * Get rid of any migrated processes.
	 */
	(void) Proc_EvictForeignProcs();
	
	waitTime.seconds = 5;
	waitTime.microseconds = 0;
	while (TRUE) {
	    if (userDead) {
		sys_ShuttingDown = TRUE;
	    }
	    timesWaited = 0;
	    while (TRUE) {
		alive = Proc_KillAllProcesses(!userDead);
		if (alive == 0) {
		    break;
		}
		if (timesWaited >= MAX_WAIT_INTERVALS) {
		    printf("%d %s processes still alive.\n", alive,
				userDead ? "kernel" : "user");
		    break;
		}
		timesWaited++;
		printf("Waiting with %d %s processes still alive\n", alive,
				userDead ? "kernel" : "user");
		if (shutdownDebug) {
		    DBG_CALL;
		}
		(void) Sync_WaitTime(waitTime);
	    }
	    if (userDead) {
		break;
	    }
	    userDead = TRUE;
	}
	/*
	 * Give this process highest priority so that no other process 
	 * can interrupt it.
	 */
	(void) Proc_SetPriority(PROC_MY_PID, PROC_NO_INTR_PRIORITY, FALSE);
    }

    /*
     * Sync the disks.
     */
    if (flags & SYS_WRITE_BACK) {
	printf("Syncing disks\n");
	Fsutil_Sync(-1, flags & SYS_KILL_PROCESSES);
    }

    if (flags & SYS_HALT) {
	Mach_MonAbort();
    } else if (flags & SYS_REBOOT) {
	Mach_MonReboot(string);
    } else if (flags & SYS_DEBUG) {
	sys_ShuttingDown = FALSE;
	sys_ErrorShutdown = FALSE;
	DBG_CALL;
    }

    return(SUCCESS);
}

Boolean	sys_ShouldSyncDisks = TRUE;

/*
 *----------------------------------------------------------------------
 *
 * Sys_SyncDisks --
 *
 *	This routine is called when the system encountered an error and
 *	the disks should be synced.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

/*
 * Exported so we can deny RPC requests as we die.
 * Otherwise we can hang RPCs if we hang trying to sync our disks.
 */
Boolean	sys_ErrorSync = FALSE;


void
Sys_SyncDisks(trapType)
    int	trapType;
{
    char	*SpriteVersion();
    
    if (sys_ErrorSync) {
	printf("Error type %d while syncing disks.\n", trapType);
	sys_ShouldSyncDisks = FALSE;
	sys_ErrorSync = FALSE;
	DBG_CALL;
	return;
    }
    if (sys_ShouldSyncDisks && !mach_AtInterruptLevel && !sys_ShuttingDown &&
        !dbg_BeingDebugged && (trapType != MACH_BRKPT_TRAP || sysPanicing)) {
	printf("Syncing disks.  Version: %s\n", SpriteVersion());
	sys_ErrorSync = TRUE;
	Fsutil_Sync(-1, TRUE);
	sys_ErrorSync = FALSE;
    }
}


/*
 *----------------------------------------------------------------------
 *
 * Sys_GetMachineInfo --
 *
 *	Returns the machine architecture and type information.
 *
 * Results:
 *	SUCCESS 		The call was successful.
 *	SYS_ARG_NOACCESS	The user arguments were not accessible.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

ReturnStatus
Sys_GetMachineInfo(archPtr, typePtr, clientIDPtr)
    int	*archPtr;	/* Buffer to hold machine architecture #. */
    int	*typePtr;	/* Buffer to hold machine type. */
    int	*clientIDPtr;	/* Buffer to hold client ID. */
{

    if (archPtr != (int *) USER_NIL) {
	int arch = Mach_GetMachineArch();

	if (Proc_ByteCopy(FALSE, sizeof(int), (Address) &arch, 
				(Address) archPtr) != SUCCESS) {
	    return(SYS_ARG_NOACCESS);
	}
    }

    if (typePtr != (int *) USER_NIL) {

	int type = Mach_GetMachineType();

	if (Proc_ByteCopy(FALSE, sizeof(int), (Address) &type, 
				(Address) typePtr) != SUCCESS) {
	    return(SYS_ARG_NOACCESS);
	}
    }


    if (clientIDPtr != (int *) USER_NIL) {
	if (Proc_ByteCopy(FALSE, sizeof(int), (Address) &rpc_SpriteID, 
				(Address) clientIDPtr) != SUCCESS) {
	    return(SYS_ARG_NOACCESS);
	}
    }

    return(SUCCESS);
}
/*
 *----------------------------------------------------------------------
 *
 * Sys_GetMachineInfoNew --
 *
 *	Returns the machine information..
 *
 * Results:
 *	SUCCESS 		The call was successful.
 *	SYS_ARG_NOACCESS	The user arguments were not accessible.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

ReturnStatus
Sys_GetMachineInfoNew(infoSize, infoBufPtr)
    int			infoSize;	/* Size of the info structure. */
    Address	 	infoBufPtr;	/* Info structure to fill in */
{
    Sys_MachineInfo	info;
    int			bytesToCopy;

    if (infoSize < sizeof(Sys_MachineInfo)) {
	bytesToCopy = infoSize;
    } else {
	bytesToCopy = sizeof(Sys_MachineInfo);
    }
    info.architecture = Mach_GetMachineArch();
    info.type = Mach_GetMachineType();
    info.processors = Mach_GetNumProcessors();
    if (Vm_CopyOut(bytesToCopy, (Address) &info, infoBufPtr) != SUCCESS) {
	return(SYS_ARG_NOACCESS);
    }
    return(SUCCESS);
}


/*
 *----------------------------------------------------------------------
 *
 * Sys_StatsStub --
 *
 *	System call stub for the Statistics hook.
 *
 * Results:
 *	SUCCESS			- the data were returned.
 *	GEN_INVALID_ARG		- if a bad argument was passed in.
 *	?			- result from Vm_CopyOut.
 *	
 *
 * Side effects:
 *	Fill in the requested statistics.
 *
 *----------------------------------------------------------------------
 */
ReturnStatus
Sys_StatsStub(command, option, argPtr)
    int command;		/* Specifies what to do */
    int option;			/* Modifier for command */
    Address argPtr;		/* Argument for command */
{
    /*
     * These extern decl's are temporary and should be removed when the
     * START_STATS and END_STATS are removed.
     */
    extern void	Dev_StartIOStats();
    extern void	Dev_StopIOStats();
    extern void	Sched_StopSchedStats();
    extern void	Sched_StartSchedStats();

    ReturnStatus status = SUCCESS;
    
    switch(command) {
	case SYS_GET_VERSION_STRING: {
	    /*
	     * option is the length of the storage referenced by argPtr.
	     */
	    register int length;
	    register char *version;
	    version = (char *)SpriteVersion();
	    length = strlen(version) + 1;
	    if (option <= 0) {
		status = GEN_INVALID_ARG;
		break;
	    }
	    if (option < length) {
		length = option;
	    }
	    status = Vm_CopyOut(length, version, argPtr);
	    break;
	}
	case SYS_SYNC_STATS: {
	    register Sync_Instrument *syncStatPtr;

	    syncStatPtr = (Sync_Instrument *)argPtr;
	    if (syncStatPtr == (Sync_Instrument *)NIL ||
		syncStatPtr == (Sync_Instrument *)0 ||
		syncStatPtr == (Sync_Instrument *)USER_NIL) {
		
		Sync_PrintStat();
	    } else if (option <= 0) {
		status = GEN_INVALID_ARG;
		break;
	    } else {
		if (option > sizeof(sync_Instrument)) {
		    option = sizeof(sync_Instrument);
		}
		status = Vm_CopyOut(option,
				  (Address) sync_Instrument,
				  (Address) syncStatPtr);
	    }
	    break;
	}
	case SYS_VM_STATS: {
	    if (argPtr == (Address)NIL ||
		argPtr == (Address)0 ||
		argPtr == (Address)USER_NIL) {
		return(GEN_INVALID_ARG);
	    } else {
		status = Vm_CopyOut(sizeof(Vm_Stat), (Address)&vmStat, argPtr);
	    }
	    break;
	}
	case SYS_SCHED_STATS: {
	    Sched_Instrument *schedStatPtr;
	    Time curTime;

	    schedStatPtr = (Sched_Instrument *)argPtr;
	    if (schedStatPtr == (Sched_Instrument *)NIL ||
		schedStatPtr == (Sched_Instrument *)0 ||
		schedStatPtr == (Sched_Instrument *)USER_NIL) {
		
		Sched_PrintStat();
	    } else {
		register int cpu;
		for (cpu = 0; cpu < MACH_MAX_NUM_PROCESSORS; cpu++) {  
		    Timer_TicksToTime(sched_Instrument.processor[cpu].
						noProcessRunning,
				  &sched_Instrument.processor[cpu].idleTime);
		}
		/*
		 * If no interrupts received, dev_LastConsoleInput will
		 * be 0, so set it to the current time.  This will happen
		 * the first time Sched_Stats is called if there were no
		 * keyboard interrupts already.
		 *
		 * Note: dev_LastConsoleInput can't be set during
		 * initialization because the timer has not yet been
		 * initialized.
		 */
		if (dev_LastConsoleInput.seconds == 0) {
		    Timer_GetTimeOfDay(&dev_LastConsoleInput,
				       (int *) NIL, (Boolean *) NIL);
		}
		Timer_GetTimeOfDay(&curTime, (int *) NIL, (Boolean *) NIL);
		Time_Subtract(curTime, dev_LastConsoleInput,
			      &sched_Instrument.noUserInput);
		status = Vm_CopyOut(sizeof(Sched_Instrument),
					  (Address)&sched_Instrument, argPtr);
	    }
	    break;
	}
	case SYS_RPC_CLT_STATS:
	case SYS_RPC_SRV_STATS:
	case SYS_RPC_TRACE_STATS:
	case SYS_RPC_SERVER_HIST:
	case SYS_RPC_CLIENT_HIST:
	case SYS_RPC_SRV_STATE:
	case SYS_RPC_CLT_STATE:
	case SYS_RPC_ENABLE_SERVICE:
	case SYS_RPC_SRV_COUNTS:
	case SYS_RPC_CALL_COUNTS:
	case SYS_RPC_SET_MAX:
	case SYS_RPC_SET_NUM:
	case SYS_RPC_NEG_ACKS:
	case SYS_RPC_CHANNEL_NEG_ACKS:
	case SYS_RPC_NUM_NACK_BUFS:
	case SYS_RPC_SANITY_CHECK:
	    status = Rpc_GetStats(command, option, argPtr);
	    break;
	case SYS_PROC_MIGRATION: {
	    switch(option) {
		/*
		 * The first two are for backward compatibility.
		 */
		case SYS_PROC_MIG_ALLOW: 
		case SYS_PROC_MIG_REFUSE: {
		    register Proc_ControlBlock *procPtr;
		    procPtr = Proc_GetEffectiveProc();
		    if (procPtr->effectiveUserID != 0) {
			status = GEN_NO_PERMISSION;
		    } else {
			/*
			 * This part is simplified for now.
			 */
			if (option == SYS_PROC_MIG_REFUSE) {
			    proc_AllowMigrationState &= ~PROC_MIG_IMPORT_ALL;
			} else {
			    proc_AllowMigrationState |= PROC_MIG_IMPORT_ALL;
			}
		    }
		}
		break;

		case SYS_PROC_MIG_SET_STATE: {
		    register Proc_ControlBlock *procPtr;
		    int arg;

		    procPtr = Proc_GetEffectiveProc();
		    if (procPtr->effectiveUserID != 0) {
			status = GEN_NO_PERMISSION;
		    } else {
			status = Vm_CopyIn(sizeof(int), argPtr, (Address)&arg);
			if (status == SUCCESS) {
			    proc_AllowMigrationState = arg;
			}
		    }
		}
		break;

		/*
		 * Also obsolete, here for backward compatibility for a while.
		 */
	        case SYS_PROC_MIG_GET_STATUS: {
		    if (argPtr != (Address) NIL) {
			/*
			 * This part is simplified for now.
			 */
			int refuse;
			if (proc_AllowMigrationState & PROC_MIG_IMPORT_ALL) {
			    refuse = 0;
			} else {
			    refuse = 1;
			}
			status = Vm_CopyOut(sizeof(Boolean),
					    (Address)&refuse,
					    argPtr);
		    } else {
			status = GEN_INVALID_ARG;
		    }
		}
		break;
	        case SYS_PROC_MIG_GET_STATE: {
		    if (argPtr != (Address) NIL) {
			status = Vm_CopyOut(sizeof(Boolean),
					    (Address)&proc_AllowMigrationState,
					    argPtr);
		    } else {
			status = GEN_INVALID_ARG;
		    }
		}
		break;
	        case SYS_PROC_MIG_GET_VERSION: {
		    if (argPtr != (Address) NIL) {
			status = Vm_CopyOut(sizeof(int),
					    (Address)&proc_MigrationVersion,
					    argPtr);
		    } else {
			status = GEN_INVALID_ARG;
		    }
		}
		break;
	        case SYS_PROC_MIG_SET_VERSION: {
		    register Proc_ControlBlock *procPtr;
		    int arg;

		    procPtr = Proc_GetEffectiveProc();
		    if (procPtr->effectiveUserID != 0) {
			status = GEN_NO_PERMISSION;
		    } else {
			status = Vm_CopyIn(sizeof(int), argPtr, (Address)&arg);
			if (status == SUCCESS && arg >= 0) {
			    proc_MigrationVersion = arg;
			} else if (status == SUCCESS) {
			    status = GEN_INVALID_ARG;
			}
		    }
		}
		break;
		case SYS_PROC_MIG_SET_DEBUG: {
		    int arg;
		    status = Vm_CopyIn(sizeof(int), argPtr, (Address)&arg);
		    if (status == SUCCESS && arg >= 0) {
			proc_MigDebugLevel = arg;
		    } else if (status == SUCCESS) {
			status = GEN_INVALID_ARG;
		    }
		}
		break;

		case SYS_PROC_MIG_GET_STATS: {
		    status = Proc_MigGetStats(argPtr);
		    break;
		}
		case SYS_PROC_MIG_RESET_STATS: {
		    status = Proc_MigResetStats();
		    break;
		}
		default:{
		    status = GEN_INVALID_ARG;
		}
		break;
	    }
	    break;
	}
	
	case SYS_PROC_TRACE_STATS: {
	    switch(option) {
		case SYS_PROC_TRACING_PRINT:
		    printf("%s %s\n", "Warning:",
			    "Printing of proc trace records not implemented.");
		    break;
		case SYS_PROC_TRACING_ON:
		    Proc_MigrateStartTracing();
		    break;
		case SYS_PROC_TRACING_OFF:
		    proc_DoTrace = FALSE;
		    break;
		default:
		    /*
		     * The default is to copy 'option' trace records.
		     */
		    status = Trace_Dump(proc_TraceHdrPtr, option, argPtr);
		    break;
	    }
	    break;
	}
	case SYS_FS_PREFIX_STATS: {
	    status = Fsprefix_Dump(option, argPtr);
	    break;
	}
	case SYS_FS_PREFIX_EXPORT: {
	    status = Fsprefix_DumpExport(option, argPtr);
	    break;
	}
	case SYS_SYS_CALL_STATS_ENABLE: {
	    printf ("%s system call profiling; was %s.\n",
		    option ? "Enabling" : "Disabling",
		    sys_CallProfiling ? "enabled" : "disabled");
	    sys_CallProfiling = option;
	    break;
	}
	case SYS_SYS_CALL_TIMES: {
	    status = Sys_OutputNumCalls(option, argPtr, TRUE);
	    break;
	}
	case SYS_SYS_CALL_STATS: {
	    status = Sys_OutputNumCalls(option, argPtr, FALSE);
	    break;
	}
	case SYS_NET_GET_ROUTE: {
	    status = Net_IDToRouteOldStub(option, 
			    sizeof(Net_RouteInfoOld), argPtr);
	    break;
	}
	case SYS_NET_ETHER_STATS: {
	    Net_Stats	stats;
	    status = Net_GetStats(NET_NETWORK_ETHER, &stats);
	    status = Vm_CopyOut(sizeof(Net_EtherStats),
				(Address)&stats.ether, (Address)argPtr);
	    break;
	}
	case SYS_DISK_STATS: {
	    int			count;
	    Sys_DiskStats	*statArrPtr;

	    if ((option < 0) || (option > 10000)) {
		status = GEN_INVALID_ARG;
	    } else { 
		statArrPtr = (Sys_DiskStats *)
					malloc(sizeof(Sys_DiskStats) * option);
		count = Dev_GetDiskStats(statArrPtr, option);
		status = Vm_CopyOut(sizeof(Sys_DiskStats) * count, 
				    (Address)statArrPtr, (Address)argPtr);
		free((Address) statArrPtr);
	    }
	    break;
	}
	case SYS_LOCK_STATS: {
	    status = Sync_GetLockStats(option, argPtr);
	    break;
	}
	case SYS_LOCK_RESET_STATS: {
	    status = Sync_ResetLockStats();
	    break;
	}
	case SYS_INST_COUNTS: {
#ifdef spur
	    Mach_InstCountInfo	info[MACH_MAX_INST_COUNT];
	    Mach_GetInstCountInfo(info);
	    Vm_CopyOut(sizeof(info), (Address) info, 
		argPtr);
	    status = SUCCESS;
#else
	    status = GEN_NOT_IMPLEMENTED;
#endif
	    break;
	}
	case SYS_RESET_INST_COUNTS: {
#ifdef spur
	    bzero(mach_InstCount, sizeof(mach_InstCount));
	    status = SUCCESS;
#else
	    status = GEN_NOT_IMPLEMENTED;
#endif
	    break;
	}
	case SYS_RECOV_STATS: {
	    status = Recov_GetStats(option, argPtr);
	    break;
	}
	case SYS_RECOV_PRINT: {
	    Recov_ChangePrintLevel(option);
	    status = SUCCESS;
	    break;
	}
	case SYS_RECOV_ABS_PINGS: {
	    recov_AbsoluteIntervals = option;
	    status = SUCCESS;
	    break;
	}
	case SYS_FS_RECOV_INFO: {
	    int		length;
	    Address	resultPtr;
	    int		lengthNeeded;

	    resultPtr = argPtr;
	    /* option is actually an in/out param */
	    status = Vm_CopyIn(sizeof (int), (Address) option,
		    (Address) &length);
	    if (status != SUCCESS) {
		break;
	    }
	    if (length != 0 && (resultPtr == (Address) NIL || resultPtr ==
		    (Address) 0 || resultPtr == (Address) USER_NIL)) {
		status = GEN_INVALID_ARG;
		break;
	    }
	    if (length > 0) {
		resultPtr = (Address) malloc(length);
	    } else {
		resultPtr = (Address) NIL;
	    }
	    status = Fsutil_FsRecovInfo(length,
		    (Fsutil_FsRecovNamedStats *) resultPtr, &lengthNeeded);
	    if (status != SUCCESS) {
		if (resultPtr != (Address) NIL) {
		    free(resultPtr);
		}
		break;
	    }
	    status = Vm_CopyOut(length, resultPtr, argPtr);
	    if (status != SUCCESS) {
		if (resultPtr != (Address) NIL) {
		    free(resultPtr);
		}
		break;
	    }
	    status = Vm_CopyOut(sizeof (int), (Address) &lengthNeeded,
		    (Address) option);
	    if (resultPtr != (Address) NIL) {
		free(resultPtr);
	    }
	    break;
	}
	case SYS_RECOV_CLIENT_INFO: {
	    int		length;
	    Address	resultPtr;
	    int		lengthNeeded;

	    resultPtr = argPtr;
	    /* option is actually an in/out param */
	    status = Vm_CopyIn(sizeof (int), (Address) option,
		    (Address) &length);
	    if (status != SUCCESS) {
		break;
	    }
	    if (length != 0 && (resultPtr == (Address) NIL || resultPtr ==
		    (Address) 0 || resultPtr == (Address) USER_NIL)) {
		status = GEN_INVALID_ARG;
		break;
	    }
	    if (length > 0) {
		resultPtr = (Address) malloc(length);
	    } else {
		resultPtr = (Address) NIL;
	    }
	    status = Recov_DumpClientRecovInfo(length, resultPtr,
		    &lengthNeeded);
	    if (status != SUCCESS) {
		if (resultPtr != (Address) NIL) {
		    free(resultPtr);
		}
		break;
	    }
	    status = Vm_CopyOut(length, resultPtr, argPtr);
	    if (status != SUCCESS) {
		if (resultPtr != (Address) NIL) {
		    free(resultPtr);
		}
		break;
	    }
	    status = Vm_CopyOut(sizeof (int), (Address) &lengthNeeded,
		    (Address) option);
	    if (resultPtr != (Address) NIL) {
		free(resultPtr);
	    }
	    break;
	}
	case SYS_RPC_SERVER_TRACE:
	    if (option == TRUE) {
		Rpc_OkayToTrace(TRUE);
	    } else  {
		Rpc_OkayToTrace(FALSE);
	    }
	    status = SUCCESS;
		
	    break;
	case SYS_RPC_SERVER_FREE:
	    Rpc_FreeTraces();
	    status = SUCCESS;

	    break;
	case SYS_RPC_SERVER_INFO: {
	    int		length;
	    Address	resultPtr;
	    int		lengthNeeded;

	    resultPtr = argPtr;
	    /* option is actually an in/out param */
	    status = Vm_CopyIn(sizeof (int), (Address) option,
		    (Address) &length);
	    if (status != SUCCESS) {
		break;
	    }
	    if (length != 0 && (resultPtr == (Address) NIL || resultPtr ==
		    (Address) 0 || resultPtr == (Address) USER_NIL)) {
		status = GEN_INVALID_ARG;
		break;
	    }
	    if (length > 0) {
		resultPtr = (Address) malloc(length);
	    } else {
		resultPtr = (Address) NIL;
	    }
	    status = Rpc_DumpServerTraces(length,
		    (RpcServerUserStateInfo *)resultPtr, &lengthNeeded);
	    if (status != SUCCESS) {
		if (resultPtr != (Address) NIL) {
		    free(resultPtr);
		}
		break;
	    }
	    status = Vm_CopyOut(length, resultPtr, argPtr);
	    if (status != SUCCESS) {
		if (resultPtr != (Address) NIL) {
		    free(resultPtr);
		}
		break;
	    }
	    status = Vm_CopyOut(sizeof (int), (Address) &lengthNeeded,
		    (Address) option);
	    if (resultPtr != (Address) NIL) {
		free(resultPtr);
	    }
	    break;
	}
	case SYS_START_STATS: {
	    /* schedule the stuff */
	    Dev_StartIOStats();
	    Sched_StartSchedStats();
	    status = SUCCESS;
	    break;
	}
	case SYS_END_STATS: {
	    /* deschedule the stuff */
	    Dev_StopIOStats();
	    Sched_StopSchedStats();
	    status = SUCCESS;
	    break;
	}
#ifdef sun4c
	case SYS_FAST_RESTART: {
	    if (option == -1) {
		/* Do a fast reboot -- Turn off interrupts. */
		DISABLE_INTR();
		/* restart */
		Mach_FastBoot();
	    } else {
		/* Set debug level. */
		printf("Setting fsrecov_DebugLevel to %d, was %d.\n",
			option, fsrecov_DebugLevel);
		fsrecov_DebugLevel = option;
		status = SUCCESS;
	    }
	    break;
	}
        case SYS_DEV_CHANGE_SCSI_DEBUG: {
	    Dev_ChangeScsiDebugLevel(option);
	    status = SUCCESS;
	    break;
	}
        case SYS_MACH_PRINT_DEV_TREE: {
	    Mach_MonTraverseAndPrintDevTree();
	    status = SUCCESS;
	    break;
	}
#endif /* sun4c */
	case SYS_PROC_ADD_SERVERS: {
	    if (option < 0) {
		status = GEN_INVALID_ARG;
	    } else {
		int	numCreated;
		numCreated = Proc_ServerProcCreate(option);
		printf("Adding %d Proc_Servers to kernel\n", numCreated);
		status = SUCCESS;
	    }
	    break;
	}
	case SYS_RECOV_BOX: {
	    switch(option) {
	    case RECOV_PRINT_REBOOT_TIMES:
	    case RECOV_TEST_ADD_DELETE:
	    case RECOV_TEST_ADD: {
		status = Fsrecov_TestCmd(option, argPtr);
		break;
	    }
	    default:
		status = Recov_Cmd(option, argPtr);
		break;
	    }
	    break;
	}
        case SYS_DONT_PRINT: {
            if (option != 0) {
                /* Make it so we can't print. */
                sys_DontPrint = TRUE;
            } else {
                sys_DontPrint = FALSE;
            }
            status = SUCCESS;
            break;
        }
	case SYS_RECOV_PARAMS: {
	    status = Recov_Cmd(option, argPtr);
	    break;
	}
        default:
            status = GEN_INVALID_ARG;
            break;
    }

    return(status);
}
