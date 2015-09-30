if (!defined &_SYSSTATS) {
    eval 'sub _SYSSTATS {1;}';
    eval 'sub SYS_RPC_CLT_STATS {1;}';
    eval 'sub SYS_RPC_SRV_STATS {2;}';
    eval 'sub SYS_SYNC_STATS {3;}';
    eval 'sub SYS_SCHED_STATS {4;}';
    eval 'sub SYS_VM_STATS {5;}';
    eval 'sub SYS_RPC_TRACE_STATS {6;}';
    eval 'sub SYS_FS_PREFIX_STATS {7;}';
    eval 'sub SYS_PROC_TRACE_STATS {8;}';
    eval 'sub SYS_SYS_CALL_STATS {9;}';
    eval 'sub SYS_RPC_SERVER_HIST {10;}';
    eval 'sub SYS_RPC_CLIENT_HIST {11;}';
    eval 'sub SYS_NET_GET_ROUTE {12;}';
    eval 'sub SYS_RPC_SRV_STATE {13;}';
    eval 'sub SYS_RPC_CLT_STATE {14;}';
    eval 'sub SYS_NET_ETHER_STATS {15;}';
    eval 'sub SYS_RPC_ENABLE_SERVICE {16;}';
    eval 'sub SYS_GET_VERSION_STRING {17;}';
    eval 'sub SYS_PROC_MIGRATION {18;}';
    eval 'sub SYS_DISK_STATS {19;}';
    eval 'sub SYS_FS_PREFIX_EXPORT {20;}';
    eval 'sub SYS_LOCK_STATS {21;}';
    eval 'sub SYS_RPC_SRV_COUNTS {22;}';
    eval 'sub SYS_RPC_CALL_COUNTS {23;}';
    eval 'sub SYS_LOCK_RESET_STATS {24;}';
    eval 'sub SYS_INST_COUNTS {25;}';
    eval 'sub SYS_RESET_INST_COUNTS {26;}';
    eval 'sub SYS_RECOV_STATS {27;}';
    eval 'sub SYS_FS_RECOV_INFO {28;}';
    eval 'sub SYS_RECOV_CLIENT_INFO {29;}';
    eval 'sub SYS_RPC_SERVER_TRACE {30;}';
    eval 'sub SYS_RPC_SERVER_INFO {31;}';
    eval 'sub SYS_RPC_SERVER_FREE {32;}';
    eval 'sub SYS_RPC_SET_MAX {33;}';
    eval 'sub SYS_RPC_SET_NUM {34;}';
    eval 'sub SYS_RPC_NEG_ACKS {35;}';
    eval 'sub SYS_RPC_CHANNEL_NEG_ACKS {36;}';
    eval 'sub SYS_RECOV_ABS_PINGS {37;}';
    eval 'sub SYS_RECOV_PRINT {38;}';
    eval 'sub SYS_RPC_NUM_NACK_BUFS {39;}';
    eval 'sub SYS_TRACELOG_STATS {40;}';
    eval 'sub SYS_START_STATS {100;}';
    eval 'sub SYS_END_STATS {101;}';
    eval 'sub SYS_DEV_CHANGE_SCSI_DEBUG {102;}';
    if (defined &SOSP91) {
	eval 'sub SYS_SCHED_MORE_STATS {103;}';
	eval 'sub SYS_FS_SOSP_MIG_STATS {104;}';
	eval 'sub SYS_FS_SOSP_NAME_STATS {105;}';
	eval 'sub SYS_FSCACHE_EXTRA_STATS {106;}';
    }
    eval 'sub SYS_RPC_SANITY_CHECK {107;}';
    eval 'sub SYS_FS_EXTRA_STATS {108;}';
    eval 'sub SYS_RPC_TRACING_PRINT {-1;}';
    eval 'sub SYS_RPC_TRACING_OFF {-2;}';
    eval 'sub SYS_RPC_TRACING_ON {-3;}';
    eval 'sub SYS_PROC_TRACING_PRINT {1;}';
    eval 'sub SYS_PROC_TRACING_OFF {2;}';
    eval 'sub SYS_PROC_TRACING_ON {3;}';
    eval 'sub SYS_PROC_MIG_ALLOW {0;}';
    eval 'sub SYS_PROC_MIG_REFUSE {1;}';
    eval 'sub SYS_PROC_MIG_GET_STATUS {2;}';
    eval 'sub SYS_PROC_MIG_SET_DEBUG {3;}';
    eval 'sub SYS_PROC_MIG_GET_VERSION {4;}';
    eval 'sub SYS_PROC_MIG_GET_STATE {5;}';
    eval 'sub SYS_PROC_MIG_SET_STATE {6;}';
    eval 'sub SYS_PROC_MIG_SET_VERSION {7;}';
    eval 'sub SYS_PROC_MIG_GET_STATS {8;}';
    eval 'sub SYS_PROC_MIG_RESET_STATS {9;}';
    eval 'sub SYS_TRACELOG_ON {1;}';
    eval 'sub SYS_TRACELOG_OFF {2;}';
    eval 'sub SYS_TRACELOG_DUMP {3;}';
    eval 'sub SYS_TRACELOG_RESET {4;}';
    eval 'sub SYS_TRACELOG_KERNELLEN {32;}';
    eval 'sub SYS_TRACELOG_TYPELEN {8;}';
    eval 'sub LOST_TYPE {128;}';
    eval 'sub TRACELOG_FLAGMASK {0xf0000000;}';
    eval 'sub TRACELOG_TYPEMASK {0x0fff0000;}';
    eval 'sub TRACELOG_BYTEMASK {0x0000ffff;}';
    eval 'sub TRACELOG_MAGIC {0x44554d50;}';
    eval 'sub TRACELOG_MAGIC2 {0x44554d51;}';
    eval 'sub SYS_DISK_NAME_LENGTH {100;}';
    if (defined &SOSP91) {
	require 'spriteTime.ph';
    }
}
1;