/* $Header: /user5/kupfer/spriteserver/src/benchmarks/exectest/RCS/exectest.c,v 1.2 92/05/27 21:35:16 kupfer Exp $ */

/* 
 * Usage: exectest [-f]
 * -f = see how long a failed exec takes.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/time.h>
#ifdef sprite
#include "proc.h"
#endif

void TimeFailures();

main(argc, argv)
    int argc;
    char *argv[];
{
    register 	int 	i;
    char	buf[128];
    int		numReps;
    struct timeval startTime, endTime;
    int		pid;
    FILE	*fd;

    if (argc > 1 && strcmp(argv[1], "-f") == 0) {
	TimeFailures();
	exit(0);
    }

    if (argc == 1) { 
	fd = fopen("time.file","w");
	if (!fd) {
	    perror("fopen");
	    exit(1);
	}
	gettimeofday(&startTime,0);
	if (fwrite(&startTime, sizeof(startTime), 1, fd) != 1) {
	    perror("fwrite");
	    exit(1);
	}
	fclose(fd);
	sprintf(buf,"%d", 1000);
	execl(argv[0], argv[0], buf,0);
	perror("execl");
	fprintf(stderr,"Exec 1 failed\n");
	exit(1);
    }
    numReps = atoi(argv[1]);
    if (numReps <= 0) {
	gettimeofday(&endTime,0);
	fd = fopen("time.file","r");
	if (!fd) {
	    perror("fopen");
	    exit(1);
	}
	if (fread(&startTime, sizeof(startTime), 1, fd) != 1) {
	    perror("fread");
	    exit(1);
	}
	fclose(fd);
	fixtime(&startTime,&endTime);
	printf("exec test %d exec time %4d.%03d\n", 1000, 
	    endTime.tv_sec, endTime.tv_usec/1000);
	exit(0);
    }
    sprintf(buf,"%d",numReps-1);
    execl(argv[0], argv[0], buf,0);
	perror("execl");
    fprintf(stderr,"Exec failed");
    exit(1);

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

void
TimeFailures()
{
    struct timeval startTime, endTime;
    int i;

    gettimeofday(&startTime, 0);
    for (i = 0; i < 1000; ++i) {
	(void)execl("NoSuchProgram", "rent", "this", "space", 0);
    }
    gettimeofday(&endTime, 0);
    fixtime(&startTime, &endTime);
    printf("failed execs: %d calls, time %4d.%03d\n",
	   1000, endTime.tv_sec, endTime.tv_usec/1000);
}
