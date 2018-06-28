#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sched.h>

inline int abs(int a)
{
    return a > 0 ? a : -1 * a;
}

extern __inline__ uint64_t rdtsc(void)
{
    uint64_t x;
    __asm__ volatile("rdtsc":"=A"(x));
    return x;
}

/*
The RDTSCP instruction reads the same TSC as the RDTSC instruction, so if RDTSC is invariant, then RDTSCP will be as well.
RDTSCP is slightly more ordered than RDTSC.  RDTSC is not ordered at all, which means that it will execute some time in the out-of-order window of the processor, which may be before or after the instruction(s) that you are interested in timing.   RDTSCP will not execute until all prior instructions (in program order) have executed.  So it can't execute "early", but there is no guarantee that the execution won't be delayed until after some subsequent (in program order) instructions have executed.
*/

extern __inline__ uint64_t rdtscp(void)
{
    uint32_t lo, hi;
    // take time stamp counter, rdtscp does serialize by itself, and is much cheaper than using CPUID
    __asm__ __volatile__("rdtscp":"=a"(lo), "=d"(hi));
    return ((uint64_t) lo) | (((uint64_t) hi) << 32);
}

int main(int argc, char** argv)
{
    int result, i;
    uint64_t cycles;
    char* cpu_info_file = (char*)malloc(200);
    char* cpu_freq = (char*)malloc(200);
    int cpu_freq_i;
    FILE* in;
    double d = 1.1;

    /*
        for (i=0; i<100000000; i++) {
            d=d*d;
        }
        printf("%f", d);
    */
    cycles = rdtsc();
    result = abs(-33);
    cycles = rdtsc() - cycles;
    printf("rdtsc: abs() consumes %d cycles!\n", (int)cycles);

    cycles = rdtscp();
    result = abs(-33);
    cycles = rdtscp() - cycles;
    printf("rdtscp: abs() consumes %d cycles!\n", (int)cycles);

    // for real machine only, you cannot run the following code on most
    // virtual machines
    printf("current CPU id is %d\n", sched_getcpu());
    sprintf(cpu_info_file,
            "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq",
            sched_getcpu());
    printf("cpu info path := %s\n", cpu_info_file);
    in = fopen(cpu_info_file, "r");
    fgets(cpu_freq, 200, in);
    sscanf(cpu_freq, "%d", &cpu_freq_i);
    cpu_freq_i = cpu_freq_i / 1000;
    printf("cpu freq := %dMHz\n", cpu_freq_i);
}
