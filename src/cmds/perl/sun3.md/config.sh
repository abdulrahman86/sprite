#!/bin/sh
# config.sh
# This file was produced by running the Configure script.
d_eunice='undef'
define='define'
eunicefix=':'
loclist='
cat
cp
echo
expr
grep
mkdir
mv
rm
sed
sort
tr
uniq
'
expr='/sprite/cmds.sun3/expr'
sed='/sprite/cmds.sun3/sed'
echo='echo'
cat='/sprite/cmds.sun3/cat'
rm='/sprite/cmds.sun3/rm'
mv='/sprite/cmds.sun3/mv'
cp='/sprite/cmds.sun3/cp'
tail=''
tr='/sprite/cmds.sun3/tr'
mkdir='/sprite/cmds.sun3/mkdir'
sort='/sprite/cmds.sun3/sort'
uniq='/sprite/cmds.sun3/uniq'
grep='/sprite/cmds.sun3/grep'
trylist='
Mcc
bison
cpp
csh
egrep
line
nroff
perl
test
uname
yacc
'
test='test'
inews=''
egrep='/sprite/cmds.sun3/egrep'
more=''
pg=''
Mcc='Mcc'
vi=''
mailx=''
mail=''
cpp='/sprite/cmds.sun3/cpp'
perl='/sprite/cmds.sun3/perl'
emacs=''
ls=''
rmail=''
sendmail=''
shar=''
smail=''
tbl=''
troff=''
nroff='/sprite/cmds.sun3/nroff'
uname='uname'
uuname=''
line='line'
chgrp=''
chmod=''
lint=''
sleep=''
pr=''
tar=''
ln=''
lpr=''
lp=''
touch=''
make=''
date=''
csh='/sprite/cmds.sun3/csh'
bash=''
ksh=''
lex=''
flex=''
bison='/sprite/cmds.sun3/bison'
Log='$Log'
Header='$Header'
Id='$Id'
lastuname='Configure: uname: not found'
alignbytes='2'
bin='/sprite/lib/perl'
installbin='/sprite/cmds'
byteorder='4321'
contains='grep'
cppstdin='/sprite/cmds.sun3/cpp'
cppminus=''
d_bcmp='define'
d_bcopy='define'
d_bzero='define'
d_castneg='undef'
castflags='3'
d_charsprf='define'
d_chsize='undef'
d_crypt='define'
cryptlib=''
d_csh='define'
d_dosuid='undef'
d_dup2='define'
d_fchmod='define'
d_fchown='define'
d_fcntl='define'
d_flexfnam='define'
d_flock='define'
d_getgrps='define'
d_gethent='undef'
d_getpgrp='define'
d_getpgrp2='undef'
d_getprior='define'
d_htonl='define'
d_index='define'
d_killpg='define'
d_lstat='define'
d_memcmp='define'
d_memcpy='define'
d_mkdir='define'
d_msg='undef'
d_msgctl='undef'
d_msgget='undef'
d_msgrcv='undef'
d_msgsnd='undef'
d_ndbm='define'
d_odbm='define'
d_open3='define'
d_readdir='define'
d_rename='define'
d_rmdir='define'
d_select='define'
d_sem='define'
d_semctl='define'
d_semget='define'
d_semop='define'
d_setegid='define'
d_seteuid='define'
d_setpgrp='define'
d_setpgrp2='undef'
d_setprior='define'
d_setregid='define'
d_setresgid='undef'
d_setreuid='define'
d_setresuid='undef'
d_setrgid='define'
d_setruid='define'
d_shm='define'
d_shmat='define'
d_voidshmat='undef'
d_shmctl='define'
d_shmdt='define'
d_shmget='define'
d_socket='define'
d_sockpair='undef'
d_oldsock='undef'
socketlib=''
d_statblks='define'
d_stdstdio='undef'
d_strctcpy='define'
d_strerror='define'
d_symlink='define'
d_syscall='undef'
d_truncate='define'
d_vfork='define'
d_voidsig='define'
d_tosignal='int'
d_volatile='define'
d_vprintf='define'
d_charvspr='define'
d_wait4='undef'
d_waitpid='undef'
gidtype='gid_t'
groupstype='int'
i_fcntl='undef'
i_gdbm='undef'
i_grp='define'
i_niin='define'
i_sysin='undef'
i_pwd='define'
d_pwquota='undef'
d_pwage='undef'
d_pwchange='define'
d_pwclass='define'
d_pwexpire='define'
d_pwcomment='undef'
i_sys_file='define'
i_sysioctl='define'
i_time='undef'
i_sys_time='define'
i_sys_select='undef'
d_systimekernel='undef'
i_utime='undef'
i_varargs='define'
i_vfork='undef'
intsize='4'
libc='/lib/libc.a'
nm_opts=''
libndir=''
i_my_dir='undef'
i_ndir='undef'
i_sys_ndir='undef'
i_dirent='undef'
i_sys_dir='define'
d_dirnamlen='define'
ndirc=''
ndiro=''
mallocsrc=''
mallocobj=''
d_mymalloc='define'
mallocptrtype='void'
mansrc='/sprite/man/cmds'
manext='1'
models='none'
split=''
small=''
medium=''
large=''
huge=''
optimize='-O'
ccflags=' -I/usr/include -I/usr/include/sun'
cppflags=' -I/usr/include -I/usr/include/sun'
ldflags=''
cc='cc'
nativegcc=''
libs='-lnet -lm'
n='-n'
c=''
package='perl'
randbits='31'
scriptdir='/sprite/cmds'
installscr='/sprite/cmds'
sig_name='ZERO HUP INT DEBUG ILL TRAP IOT EMT FPE KILL MIG SEGV SYS PIPE ALRM TERM URG STOP TSTP CONT CHLD TTIN TTOU IO XCPU XFSZ VTALRM PROF WINCH MIGHOME USR1 USR2'
spitshell='cat'
shsharp='true'
sharpbang='#!'
startsh='#!/bin/sh'
stdchar='char'
uidtype='uid_t'
usrinclude='/sprite/lib/include'
inclPath=''
void=''
voidhave='7'
voidwant='7'
w_localtim='1'
w_s_timevl='1'
w_s_tm='1'
yacc='/sprite/cmds.sun3/yacc'
lib=''
privlib='/sprite/lib/perl'
installprivlib='/sprite/lib/perl'
PATCHLEVEL=19
CONFIG=true
