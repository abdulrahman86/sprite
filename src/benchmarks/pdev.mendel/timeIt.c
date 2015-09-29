/* $Header: /sprite/src/benchmarks/pdev.mendel/RCS/timeIt.c,v 1.2 92/04/10 15:52:44 kupfer Exp $ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/time.h>
#include <signal.h>
#include "pdev.h"

ReturnStatus writeSrv();
ReturnStatus readSvr();

static Pdev_CallBacks pdevDefaultCallBacks = {
     NULL,
     readSvr,
     writeSrv,
     NULL,
     NULL,
     NULL,
     NULL,
};

main(arv)
{
	register int i;
	int	junk1, junk2;
	int starttime;
	struct timeval stp, etp;
	Pdev_Token	pdev;
	int		fd;
	int		size;
	int		pid;
	char		buf[8192];
	char		*pdevName;

	pdev = Pdev_Open("./pdevD", &pdevName, 9000, 0, &pdevDefaultCallBacks, 0);
	if (pdev == (Pdev_Token) NULL) {
	    printf("Can't open pdev\n");
	    exit(1);
	}
	printf("Using pdev %s\n", pdevName);
	pid = fork();
	if (pid != 0) {
	    while(1) {
		Fs_Dispatch();
	    }
	}
	fd = open(pdevName, O_RDWR, 0);
	if (fd < 0) {
	    perror("open");
	    exit(1);
	}
	bzero(buf,8192);
	for (size = 1; size <= 8192; size *= 2) {
	    gettimeofday(&stp,0);
	    for (i = 0; i < 1000; i++) {
		if (write(fd, buf, size) != size) {
		    perror("write");
		}
	    }
	    gettimeofday(&etp,0);
	    fixtime(&stp,&etp);
	    printf("Write %d bytes 1000 times , time = %4d.%03d\n", size,
		etp.tv_sec, etp.tv_usec/1000);
	}
	for (size = 1; size <= 8192; size *= 2) {
	    gettimeofday(&stp,0);
	    for (i = 0; i < 1000; i++) {
		if (read(fd, buf, size) != size) {
		    perror("read");
		}
	    }
	    gettimeofday(&etp,0);
	    fixtime(&stp,&etp);
	    printf("Read %d bytes 1000 times , time = %4d.%-03d\n", size,
		etp.tv_sec, etp.tv_usec/1000);
	}
	killpg(getpgrp(0),SIGTERM);
}
fixtime(s, e)
        struct  timeval *s, *e;
{

        e->tv_sec -= s->tv_sec;
        e->tv_usec -= s->tv_usec;
        if (e->tv_usec < 0) {
                e->tv_sec--; e->tv_usec += 1000000;
        }
}

ReturnStatus
writeSrv(streamPtr, async, writePtr, selectBitsPtr, sigPtr)
              Pdev_Stream *streamPtr;   
              int async;                
              Pdev_RWParam *writePtr;   
              int *selectBitsPtr;       
              Pdev_Signal *sigPtr;  
{

    *selectBitsPtr = FS_READABLE | FS_WRITABLE;
    return(SUCCESS);
}
ReturnStatus
readSvr(streamPtr, readPtr, freeItPtr, selectBitsPtr, sigPtr)
              Pdev_Stream *streamPtr;  
              Pdev_RWParam *readPtr;   
              Boolean *freeItPtr;       
              int *selectBitsPtr;       
              Pdev_Signal *sigPtr;      
{
    *freeItPtr = FALSE;
    *selectBitsPtr = FS_READABLE | FS_WRITABLE;
    return(SUCCESS);

}
