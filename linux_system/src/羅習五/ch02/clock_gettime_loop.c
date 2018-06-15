#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sched.h>

int main(int argc, char **argv)
{
	int i;
	clockid_t clk_id;
	struct timespec tt;

	// clk_id = CLOCK_REALTIME;
	clk_id = CLOCK_MONOTONIC;
	// clk_id = CLOCK_BOOTTIME;
	//clk_id = CLOCK_PROCESS_CPUTIME_ID;
	for (i=0; i<1000000000; i++) {
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tt);
	}
}
