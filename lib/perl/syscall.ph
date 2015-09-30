if (!defined &_SYSCALL_H) {
    eval 'sub _SYSCALL_H {1;}';
    eval 'sub SYS_exit {1;}';
    eval 'sub SYS_fork {2;}';
    eval 'sub SYS_read {3;}';
    eval 'sub SYS_write {4;}';
    eval 'sub SYS_open {5;}';
    eval 'sub SYS_close {6;}';
    eval 'sub SYS_creat {8;}';
    eval 'sub SYS_link {9;}';
    eval 'sub SYS_unlink {10;}';
    eval 'sub SYS_execv {11;}';
    eval 'sub SYS_chdir {12;}';
    eval 'sub SYS_mknod {14;}';
    eval 'sub SYS_chmod {15;}';
    eval 'sub SYS_chown {16;}';
    eval 'sub SYS_lseek {19;}';
    eval 'sub SYS_getpid {20;}';
    eval 'sub SYS_getuid {24;}';
    eval 'sub SYS_ptrace {26;}';
    eval 'sub SYS_access {33;}';
    eval 'sub SYS_sync {36;}';
    eval 'sub SYS_kill {37;}';
    eval 'sub SYS_stat {38;}';
    eval 'sub SYS_lstat {40;}';
    eval 'sub SYS_dup {41;}';
    eval 'sub SYS_pipe {42;}';
    eval 'sub SYS_profil {44;}';
    eval 'sub SYS_getgid {47;}';
    eval 'sub SYS_acct {51;}';
    eval 'sub SYS_ioctl {54;}';
    eval 'sub SYS_reboot {55;}';
    eval 'sub SYS_symlink {57;}';
    eval 'sub SYS_readlink {58;}';
    eval 'sub SYS_execve {59;}';
    eval 'sub SYS_umask {60;}';
    eval 'sub SYS_chroot {61;}';
    eval 'sub SYS_fstat {62;}';
    eval 'sub SYS_getpagesize {64;}';
    eval 'sub SYS_mremap {65;}';
    eval 'sub SYS_sbrk {69;}';
    eval 'sub SYS_sstk {70;}';
    eval 'sub SYS_mmap {71;}';
    eval 'sub SYS_munmap {73;}';
    eval 'sub SYS_mprotect {74;}';
    eval 'sub SYS_madvise {75;}';
    eval 'sub SYS_vhangup {76;}';
    eval 'sub SYS_mincore {78;}';
    eval 'sub SYS_getgroups {79;}';
    eval 'sub SYS_setgroups {80;}';
    eval 'sub SYS_getpgrp {81;}';
    eval 'sub SYS_setpgrp {82;}';
    eval 'sub SYS_setitimer {83;}';
    eval 'sub SYS_wait {84;}';
    eval 'sub SYS_swapon {85;}';
    eval 'sub SYS_getitimer {86;}';
    eval 'sub SYS_gethostname {87;}';
    eval 'sub SYS_sethostname {88;}';
    eval 'sub SYS_getdtablesize {89;}';
    eval 'sub SYS_dup2 {90;}';
    eval 'sub SYS_getdopt {91;}';
    eval 'sub SYS_fcntl {92;}';
    eval 'sub SYS_select {93;}';
    eval 'sub SYS_setdopt {94;}';
    eval 'sub SYS_fsync {95;}';
    eval 'sub SYS_setpriority {96;}';
    eval 'sub SYS_socket {97;}';
    eval 'sub SYS_connect {98;}';
    eval 'sub SYS_accept {99;}';
    eval 'sub SYS_getpriority {100;}';
    eval 'sub SYS_send {101;}';
    eval 'sub SYS_recv {102;}';
    eval 'sub SYS_bind {104;}';
    eval 'sub SYS_setsockopt {105;}';
    eval 'sub SYS_listen {106;}';
    eval 'sub SYS_sigvec {108;}';
    eval 'sub SYS_sigblock {109;}';
    eval 'sub SYS_sigsetmask {110;}';
    eval 'sub SYS_sigpause {111;}';
    eval 'sub SYS_sigstack {112;}';
    eval 'sub SYS_recvmsg {113;}';
    eval 'sub SYS_sendmsg {114;}';
    eval 'sub SYS_gettimeofday {116;}';
    eval 'sub SYS_getrusage {117;}';
    eval 'sub SYS_getsockopt {118;}';
    eval 'sub SYS_readv {120;}';
    eval 'sub SYS_writev {121;}';
    eval 'sub SYS_settimeofday {122;}';
    eval 'sub SYS_fchown {123;}';
    eval 'sub SYS_fchmod {124;}';
    eval 'sub SYS_recvfrom {125;}';
    eval 'sub SYS_setreuid {126;}';
    eval 'sub SYS_setregid {127;}';
    eval 'sub SYS_rename {128;}';
    eval 'sub SYS_truncate {129;}';
    eval 'sub SYS_ftruncate {130;}';
    eval 'sub SYS_flock {131;}';
    eval 'sub SYS_sendto {133;}';
    eval 'sub SYS_shutdown {134;}';
    eval 'sub SYS_socketpair {135;}';
    eval 'sub SYS_mkdir {136;}';
    eval 'sub SYS_rmdir {137;}';
    eval 'sub SYS_utimes {138;}';
    eval 'sub SYS_adjtime {140;}';
    eval 'sub SYS_getpeername {141;}';
    eval 'sub SYS_gethostid {142;}';
    eval 'sub SYS_getrlimit {144;}';
    eval 'sub SYS_setrlimit {145;}';
    eval 'sub SYS_killpg {146;}';
    eval 'sub SYS_getsockname {150;}';
    eval 'sub SYS_nfssvc {155;}';
    eval 'sub SYS_getdirentries {156;}';
    eval 'sub SYS_statfs {157;}';
    eval 'sub SYS_fstatfs {158;}';
    eval 'sub SYS_unmount {159;}';
    eval 'sub SYS_async_daemon {160;}';
    eval 'sub SYS_getfh {161;}';
    eval 'sub SYS_getdomainname {162;}';
    eval 'sub SYS_setdomainname {163;}';
    eval 'sub SYS_quotactl {165;}';
    eval 'sub SYS_exportfs {166;}';
    eval 'sub SYS_mount {167;}';
    eval 'sub SYS_ustat {168;}';
    eval 'sub SYS_semsys {169;}';
    eval 'sub SYS_msgsys {170;}';
    eval 'sub SYS_shmsys {171;}';
}
1;