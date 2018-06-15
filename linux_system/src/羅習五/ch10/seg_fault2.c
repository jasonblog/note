/*作者 https://www.facebook.com/scottt.tw*/
/*修改 羅習五*/
#define _GNU_SOURCE
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ucontext.h>

/* Define 'c' as a global register variable
   https://gcc.gnu.org/onlinedocs/gcc/Global-Register-Variables.html */
register unsigned long *c __asm__("r12");

void sighandler(int signumber, siginfo_t *sinfo, void *ucontext) {
	ucontext_t *context = ucontext;

	printf("got a signal %d(%s)\n", signumber,
			sys_siglist[signumber]);
  
	/* NOTE: assigning to 'c' instead of 'REG_R12' likely won't work on most systems
	   due to register content restoration after a signal handler returns */
	context->uc_mcontext.gregs[REG_R12] = (unsigned long) malloc(sizeof(char));
}

__attribute__((optimize("Os")))
int main(int argc, char **argv) {
    int r;
    struct sigaction sa = { {0} };
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sighandler;
    r = sigaction(SIGSEGV, &sa, NULL);
    assert(r == 0);
    *c = 0xC0FE; /*segmentation fault*/
    //printf("my pid is %d\n", getpid());
    printf("press any key to resume\n");
    getchar();
    return 0;
}

