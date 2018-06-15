/*
使用方法：sudo perfstat 執行檔 參數1 參數2
如果要產生制式化的顯示，執行 touch ~/.perflog
如果要產生可讀的的顯示，執行 rm ~/.perflog
*/

#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <setjmp.h>
#include <sys/resource.h> 
#include <assert.h>
#include <fcntl.h>


//#define basic "Power:u,CPI:u,ILP:u,cpu-cycles:u,instructions:u,MLP:u,Load_Miss_Real_Latency:u,cache-references:u,cache-misses:u,branch-instructions:u,branch-misses:u,bus-cycles:u,cpu-clock:u,task-clock:u,page-faults:u,minor-faults:u,major-faults:u,context-switches:u,cpu-migrations:u,Page_Walks_Utilization:u"
//#define basic "cpu-cycles:u,instructions:u,cache-references:u,cache-misses:u,branch-instructions:u,branch-misses:u,bus-cycles:u,cpu-clock:u,task-clock:u,page-faults:u,minor-faults:u,major-faults:u,context-switches:u,cpu-migrations:u"
#define basic "cpu-cycles:u,instructions:u,cache-references:u,cache-misses:u,branch-instructions:u,branch-misses:u,bus-cycles:u,cpu-clock:u,page-faults:u,minor-faults:u,major-faults:u,context-switches:u"
//L1, L2 cache
//#define L1L2 "L1-dcache-loads:u,L1-dcache-load-misses:u,L1-dcache-stores:u,L1-icache-load-misses:u,l2_rqsts.demand_data_rd_hit:u,l2_rqsts.demand_data_rd_miss"
//mem load L3 xsnp
#define mem_load "mem_inst_retired.lock_loads:u,mem_inst_retired.split_loads:u,mem_inst_retired.split_stores:u"
#define cache "mem_load_retired.l1_hit:u,mem_load_retired.l1_miss:u,mem_load_retired.l2_hit:u,mem_load_retired.l2_miss:u,mem_load_retired.l3_hit:u,mem_load_retired.l3_miss:u"
//cross core snoop in one pkg (in package?)
#define xsnp "mem_load_l3_hit_retired.xsnp_hit:u,mem_load_l3_hit_retired.xsnp_hitm:u,mem_load_l3_hit_retired.xsnp_miss:u,mem_load_l3_hit_retired.xsnp_none:u"
//Counts the number of machine clears due to memory order conflicts
#define reorder "machine_clears.memory_ordering:u"
//last level cache
//#define LLC "LLC-loads:u,LLC-load-misses:u,LLC-stores:u,LLC-store-misses:u"
//TLB
#define TLB "dTLB-loads:u,dTLB-load-misses:u,dTLB-stores:u,dTLB-store-misses:u,iTLB-loads:u,iTLB-load-misses:u"
//branch target buffer?
//#define branch "branch-loads:u,branch-load-misses:u"
//cross core (offcore?) snoop
//#define snoop "offcore_response.demand_code_rd.l3_hit.any_snoop:u,offcore_response.demand_code_rd.l3_hit.snoop_hit_no_fwd:u,offcore_response.demand_code_rd.l3_hit.snoop_hitm:u,offcore_response.demand_code_rd.l3_hit.snoop_miss:u,offcore_response.demand_code_rd.l3_hit.snoop_none:u,offcore_response.demand_code_rd.l3_hit.snoop_not_needed:u"
//uncore_cbox L3 Lookup any request that access cache and found line in MESI-state
//#define mesi "unc_cbo_cache_lookup.read_mesi:u,unc_cbo_cache_lookup.write_mesi:u"
//A cross-core snoop initiated by this Cbox
//#define unc_xsnp "unc_cbo_xsnp_response.hit_xcore:u,unc_cbo_xsnp_response.hitm_xcore:u,unc_cbo_xsnp_response.miss_eviction:u,unc_cbo_xsnp_response.miss_xcore:u"

char* argVect[256];
volatile sig_atomic_t hasChild = 0;
pid_t childPid;

/*signal handler專門用來處理ctr-c*/
void ctrC_handler(int sigNumber) {
    if (hasChild) {
        kill(childPid, sigNumber);
        hasChild = 0;
    } else {
        return;
    }
}

int main (int argc, char** argv) {
    int pid, wstatus, logmode=0;
    struct rusage resUsage;     //資源使用率
    struct timespec statTime, endTime;
    /*底下程式碼註冊signal的處理方式*/
    signal(SIGINT, ctrC_handler);

    logmode = 1;

    if (logmode == 0) {
        argVect[0]="perf";
        argVect[1]="stat";
        argVect[2]="-a";
        argVect[3]="-r";
        argVect[4]="10";
        argVect[5]="--log-fd";
        argVect[6]="2";
        argVect[7]="-e";
        argVect[8]=basic "," mem_load "," cache "," xsnp "," reorder "," TLB ;
        for (int i=1; i< argc; i++)
            argVect[8+i] = argv[i];
    } else {
        argVect[0]="perf";
        argVect[1]="stat";
        argVect[2]="-a";
        argVect[3]="-r";
        argVect[4]="10";
        argVect[5]="-x";
        argVect[6]=";";
        argVect[7]="--log-fd";
        argVect[8]="2";
        argVect[9]="-e";
        argVect[10]=basic "," mem_load "," cache "," xsnp "," reorder "," TLB ;
        for (int i=1; i< argc; i++)
            argVect[10+i] = argv[i];
    }

    pid = fork();   //除了exit, cd，其餘為外部指令
    if (pid == 0) {
        /*
        cpu_set_t set;
        CPU_ZERO(&set);
        CPU_SET(5, &set);
        int ret = sched_setaffinity(1, sizeof(set), &set);
        if (ret==-1) {
            perror("sudo");
            exit(errno);
        }
        */
        if (execvp(argVect[0], argVect)==-1) {
            perror("perfstat");
            exit(errno);
        }
    } else {
        childPid = pid;/*通知singal handler，如果使用者按下ctr-c時，要處理這個child*/
        hasChild = 1;/*通知singal handler，正在處理child*/
        wait(&wstatus);
    }
}