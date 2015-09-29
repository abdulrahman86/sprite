/* 
 * quick test program to cause an address fault. 
 * 
 * $Header: /user5/kupfer/spriteserver/tests/fault/RCS/fault.c,v 1.3 92/03/12 20:47:29 kupfer Exp $
 */

/* 
 * usage: fault [ stack|readonly|ignore|handle ] 
 * 
 * where "stack" means to touch enough pages to test the stack-extension 
 * code in Sprite, "readonly" means to try writing read-only memory, and 
 * "ignore" means to disable receipt of the signal.  Only one option can be 
 * specified. 
 * 
 * Should probably not be built with optimization turned on.
 */

#include <sprite.h>
#include <mach.h>
#include <setjmp.h>
#include <signal.h>
#include <status.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>
#include <vm.h>

jmp_buf jmpBuf;			/* state saved by setjmp */
int numFaults = 0;		/* number of times we caused an exception */
char globalCh;			/* sometimes used by signal handler */

void DoStack();
void DoReadOnly();
void SegvHandler(), FpeHandler();

int
main(argc, argv)
    int argc;
    char *argv[];
{
    char *bogusPtr = (char *)0x23456789; /* 0x10000000 is heap on DECstation */
    char ch;

    if (argc > 1) {
	if (strcmp(argv[1], "stack") == 0) {
	    (void)DoStack((int)vm_page_size * 2);
	    exit(0);
	} else if (strcmp(argv[1], "readonly") == 0) {
	    DoReadOnly();
	    exit(0);
	} else if (strcmp(argv[1], "ignore") == 0) {
	    if (signal(SIGSEGV, SIG_IGN) == BADSIG) {
		perror("can't ignore SIGSEGV");	/* XXX not allowed by Sprite */
		exit(1);
	    }
	} else if (strcmp(argv[1], "handle") == 0) {
	    if (signal(SIGSEGV, SegvHandler) == BADSIG) {
		perror("Can't register SIGSEGV handler");
		exit(1);
	    }
	    if (signal(SIGFPE, FpeHandler) == BADSIG) {
		perror("Can't register SIGFPE handler");
		exit(1);
	    }
	}
    }

    if (setjmp(jmpBuf)) {
	Test_PutMessage("longjmp\n");
    }
    if (numFaults > 5) {
	exit(numFaults);
    }

    ch = *bogusPtr;
    Test_PutMessage("it worked?\n");
    return ch;			/* prevent optimizer from interfering */
}

/* 
 * Recurse a bunch of times to force automatic growing of the stack. 
 */
void
DoStack(numTimes)
    int numTimes;		/* number of times to recurse */
{
    int foo;			/* force something to go on the stack */

    foo = numTimes;
    if (numTimes > 0) {
	(void)DoStack(numTimes - 1);
    }
#ifdef lint
    foo = foo;
#endif
}

/* 
 * Map a file read-only then try to write to it.  The if'd out code has 
 * interesting addressing errors you can get (some only happen on certain 
 * machine types).
 */
void
DoReadOnly()
{
    ReturnStatus status;
    Address startAddr;
    char *fileName = "testInput";
    int foo = 0;

    status = Vm_MapFile(fileName, TRUE, (off_t)0, 1, &startAddr);
    if (status != SUCCESS) {
	Test_PutMessage("Couldn't map `");
	Test_PutMessage(fileName);
	Test_PutMessage("': ");
	Test_PutMessage(Stat_GetMsg(status));
	Test_PutMessage("\n");
	return;
    }

#if 0
    foo = *(int *)(startAddr + 1); /* force word reference to unaligned addr */
    *(int *)(startAddr+1) = 42;	/* ditto */
#endif
    *(int *)startAddr = 42;

#ifdef lint
    foo = foo;
#endif
}

void
SegvHandler(sigNum, code, contextPtr, addr)
    int sigNum, code;
    Address contextPtr, addr;
{
#if 0
    printf("(%d, %d, 0x%x, 0x%x)\n", sigNum, code, contextPtr, addr);
    exit(0);
#endif
#if 0
    ++numFaults;
    longjmp(jmpBuf, 1);
#endif
#if 0
    /* Cause another instance of the same exception. */
    globalCh = *(char *)-1;
    Test_PutMessage("SegvHandler: shouldn't get here.\n");
    exit(1);
#endif
    /* Cause a different exception. */
    double foo, bar;

    foo = 0.0;
    bar = 0.0;
    printf("%f\n", foo/bar);
    Test_PutMessage("SegvHandler: shouldn't get here.\n");
    exit(1);

#ifdef lint
    sigNum = sigNum;
    code = code;
    contextPtr = contextPtr;
    addr = addr;
#endif
}

void
FpeHandler()
{
    ++numFaults;
    longjmp(jmpBuf, 1);
}
