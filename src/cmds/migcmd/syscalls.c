/* 
 * syscalls.c --
 *
 *	File to declare system call names as a global array.
 *
 * Copyright 1988 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /sprite/src/cmds/migcmd/RCS/syscalls.c,v 1.2 90/02/16 11:08:43 douglis Exp $ SPRITE (Berkeley)";
#endif not lint


#include "syscalls.h"

SysCallInfo sysCallArray[] = {
    "Proc_Fork",		0,
    "Proc_Exec",                0,
    "Proc_Exit",                0,
    "Sync_WaitTime",            1,
    "Test_PrintOut",            1,    
    "Test_GetLine",             1,
    "Test_GetChar",             1,
    "Fs_OpenStub",              1,
    "Fs_ReadStub",              1,
    "Fs_WriteStub",             1,
    "Fs_UserClose",             1,
    "Fs_RemoveStub",            1,
    "Fs_RemoveDirStub",         1,
    "Fs_MakeDirStub",           1,
    "Fs_ChangeDirStub",         1,
    "Proc_Wait",                0,
    "Proc_Detach",              0,
    "Proc_GetIDs",              1,
    "Proc_SetIDs",              1,
    "Proc_GetGroupIDs",         1,
    "Proc_SetGroupIDs",         0,
    "Proc_GetFamilyID",         0,
    "Proc_SetFamilyID",         0,
    "Test_RpcStub",             1,
    "Test_StatsStub",           1,
    "Vm_CreateVA",              1,
    "Vm_DestroyVA",             1,
    "Sig_UserSend",             0,
    "Sig_Pause",                1,
    "Sig_SetHoldMask",          1,
    "Sig_SetAction",            1,
    "Prof_Start",               1,
    "Prof_End",                 1,
    "Prof_DumpStub",            0,
    "Vm_Cmd",                   0,
    "Sys_GetTimeOfDay",         0,
    "Sys_SetTimeOfDay",         0,
    "Sys_DoNothing",            1,
    "Proc_GetPCBInfo",          1,
    "Vm_GetSegInfo",            1,
    "Proc_GetResUsage",         0,
    "Proc_GetPriority",         0,
    "Proc_SetPriority",         0,
    "Proc_Debug",               0,
    "Proc_Profile",             0,
    "Fs_TruncateStub",          1,
    "Fs_TruncateIDStub",        1,
    "Fs_GetNewIDStub",          1,
    "Fs_GetAttributesStub",     1,
    "Fs_GetAttributesIDStub",   1,
    "Fs_SetAttributesStub",     1,
    "Fs_SetAttributesIDStub",   1,
    "Fs_SetDefPermStub",        1,
    "Fs_IOControlStub",         1,
    "Dev_VidEnable",            0,
    "Proc_SetEnviron",          0,
    "Proc_UnsetEnviron",        0,
    "Proc_GetEnvironVar",       0,
    "Proc_GetEnvironRange",     0,
    "Proc_InstallEnviron",      0,
    "Proc_CopyEnviron",         0,
    "Sync_SlowLockStub",        1,
    "Sync_SlowWaitStub",        1,
    "Sync_SlowBroadcastStub",   1,
    "Vm_PageSize",              1,
    "Fs_HardLinkStub",          1,
    "Fs_RenameStub",            1,
    "Fs_SymLinkStub",           1,
    "Fs_ReadLinkStub",          1,
    "Fs_CreatePipeStub",        1,
    "Vm_MapKernelIntoUser",     0,
    "Fs_AttachDiskStub",        0,
    "Fs_SelectStub",            1,
    "Sys_Shutdown",             0,
    "Proc_Migrate",             0,
    "Fs_MakeDeviceStub",        1,
    "Fs_CommandStub",           0,
    "Fs_LockStub",              1,
    "Sys_GetMachineInfo", 	1,
    "Net_InstallRoute", 	0,
    "Fs_ReadVector", 		1,
    "Fs_WriteVector", 		1,
    "Fs_CheckAccess", 		1,
    "Proc_GetIntervalTimer", 	1,
    "Proc_SetIntervalTimer", 	1,
    "Fs_FileWriteBackStub",	1,
    "Proc_ExecEnv",		1,
    "Fs_SetAttributes",		1,
    "Fs_SetAttributesID",	1,
    "Proc_GetHostIDs",		0,
};

int sysCallArraySize = sizeof(sysCallArray);
