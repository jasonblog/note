#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sched.h>

extern inline int myRand()
{
    int i, result;
    return result;

    for (i = 0; i < 100000000; i++) {
        result = rand() * rand();
    }
}

int main(int argc, char** argv)
{
    int result, i;
    //struct timespec tp;
    struct timespec t_res;
    clockid_t clk_id;
    struct timespec tt1, tt2;

    // clk_id = CLOCK_REALTIME;
    clk_id = CLOCK_MONOTONIC;
    // clk_id = CLOCK_BOOTTIME;
    //clk_id = CLOCK_PROCESS_CPUTIME_ID;

    clock_getres(CLOCK_REALTIME, &t_res);
    printf("CLOCK_REALTIME resolution:\t\t %ld\n", t_res.tv_nsec);
    clock_getres(CLOCK_MONOTONIC, &t_res);
    printf("CLOCK_MONOTONIC resolution:\t\t %ld\n", t_res.tv_nsec);
    clock_getres(CLOCK_BOOTTIME, &t_res);
    printf("CLOCK_BOOTTIME resolution:\t\t %ld\n", t_res.tv_nsec);
    clock_getres(CLOCK_PROCESS_CPUTIME_ID, &t_res);
    printf("CLOCK_PROCESS_CPUTIME_ID resolution:\t %ld\n", t_res.tv_nsec);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tt1);

    for (i = 0; i < 10; i++) {
        result = myRand();
    }

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tt2);
    printf("%d\n", result);
    printf("abs() consumes %ld nanoseconds!\n", tt2.tv_nsec - tt1.tv_nsec);
}
