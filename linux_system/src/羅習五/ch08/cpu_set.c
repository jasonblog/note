#define _GNU_SOURCE
#include <sched.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char **argv) {
	cpu_set_t set;
	CPU_ZERO(&set);
	int i,j,ret;
	CPU_ZERO(&set);
	CPU_SET(3, &set);
	//ret=sched_setaffinity(0, sizeof(cpu_set_t), &set);
	//if (ret==-1)
	//	perror("sched_setaffinity");
	for(i=0; i<100000; i++) {
		for(j=0; j<10000000; j++) {
			j=j+1;
		}
	}
	return j;
}

