//gcc decSP4K.s getSP.s -o testStack
//usage testStack mem|reg

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <alloca.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>

void seg_fault(int sigNo) {
	while(1);
}

int main(int argc, char** argv) {
	char base;
	char *stackP=&base;
	int i;
	struct rlimit rlim;
	int usingSP;
	if (signal(SIGSEGV, seg_fault)==SIG_ERR) {
		perror("SIGSEGV");
	}

	if (argc != 2) exit(-1);
	if (strcmp("reg", argv[1]) ==0)
		usingSP = 1;
	else usingSP = 0;

	getrlimit(RLIMIT_STACK, &rlim);
	printf("soft=%dK, hard=%dK\n",
		(int)rlim.rlim_cur/1024, 
		(int)rlim.rlim_max/1024);
	printf("pid = %d", getpid());
	getchar();
	while(1) {
		if(usingSP) {
			__asm__(
				"SUB $(4096-8), %rsp;\n"
			);
			asm (
				"MOVQ %%rsp, %0;\n"
				:"=r"(stackP)
			);
		} else stackP -= 4096;
		*stackP = 1;
		printf("%ldK\n",(&base - stackP)/(1024));
	}
}