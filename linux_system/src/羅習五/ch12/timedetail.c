/*執行方法：
./myShell
>> ls -R /
ctr-c
*/
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
#include <sys/time.h>
#include <sys/wait.h>

///color///
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"
#define RED_BOLD "\x1b[;31;1m"
#define BLU_BOLD "\x1b[;34;1m"
#define YEL_BOLD "\x1b[;33;1m"
#define GRN_BOLD "\x1b[;32;1m"
#define CYAN_BOLD_ITALIC "\x1b[;36;1;3m"
#define RESET "\x1b[0;m"

/*
全域變數，放解析過後的使用者指令（字串陣列）
*/
char* argVect[256];

//下列三個變數作為main和signal handler溝通用
sigjmp_buf jumpBuf;
volatile sig_atomic_t hasChild = 0;
pid_t childPid;
const long long nspersec = 1000000000;

long long timespec2sec(struct timespec ts) {
    long long ns = ts.tv_nsec;
    ns += ts.tv_sec * nspersec;
    return ns;
    //return (double)ns/1000000000.0;
}

double timeval2sec(struct timeval input) {
    long long us = input.tv_sec*1000000;
    us += input.tv_usec;
    //printf("%ldsec, %ld us\n", input.tv_sec, input.tv_usec);
    return (double)us/1000000.0;
}

/*signal handler專門用來處理ctr-c*/
void ctrC_handler(int sigNumber) {
    if (hasChild) {
        kill(childPid, sigNumber);
        hasChild = 0;
    } else {
        /*確認main function並不是剛好在處理字串，這裡使用一個隱含的同步方法*/
        /*藉由確認是否argVect[0]（即執行檔）是否為NULL保證main function不是在處理字串*/
        /*主程式的控制迴圈必須在一開始的地方將argVect[0]設為NULL*/
        if (argVect[0] == NULL) {
            ungetc('\n', stdin);ungetc('c', stdin);ungetc('^', stdin);
            siglongjmp(jumpBuf, 1);
        } else {
            /*極少發生，剛好在處理字串，忽略這個signal，印出訊息提示一下*/
            fprintf(stderr, "info, 處理字串時使用者按下ctr-c\n");
        }
    }
}

/*
parseString：將使用者傳進的命令轉換成字串陣列
str：使用者傳進的命令
cmd：回傳執行檔
*/
void parseString(char* str, char** cmd) {
    int idx=0;
    char* retPtr;
    //printf("%s\n", str);
    retPtr=strtok(str, " \n");
    while(retPtr != NULL) {
        //printf("token =%s\n", retPtr);
        //if(strlen(retPtr)==0) continue;
        argVect[idx++] = retPtr;
        if (idx==1)
            *cmd = retPtr;
        retPtr=strtok(NULL, " \n");
    }
    argVect[idx]=NULL;
}

int main (int argc, char** argv) {
    int pid, wstatus;
    struct rusage resUsage;     //資源使用率
    struct timespec statTime, endTime;
    /*底下程式碼註冊signal的處理方式*/
    signal(SIGINT, ctrC_handler);

    //printf("argc =%d\n", argc);
    for (int i=1; i<argc; i++) {
        argVect[i-1] = argv[i];
    }


    clock_gettime(CLOCK_MONOTONIC, &statTime);
    pid = fork();   //除了exit, cd，其餘為外部指令
    if (pid == 0) {
        /*
        產生一個child執行使用者的指令
        */
        if (execvp(argVect[0], argVect)==-1) {
            perror("myShell");
            exit(errno*-1);
        }
    } else {
        childPid = pid;/*通知singal handler，如果使用者按下ctr-c時，要處理這個child*/
        hasChild = 1;/*通知singal handler，正在處理child*/
        wait3(&wstatus, 0, &resUsage);
        clock_gettime(CLOCK_MONOTONIC, &endTime);
        //wait(&wstatus);
        //int ret=getrusage(RUSAGE_CHILDREN, &resUsage);
        //printf("ret = %d\n", ret);
        double sysTime = timeval2sec(resUsage.ru_stime);
        double usrTime = timeval2sec(resUsage.ru_utime);
        //printf("%ld\n", endTime.tv_nsec);
        //printf("%ld\n", statTime.tv_nsec);
        printf("\n");
        long long elapse = timespec2sec(endTime)-timespec2sec(statTime);
        printf(RED"經過時間:\t\t\t\t"YELLOW"%lld.%llds\n",elapse/nspersec, elapse%nspersec);
        printf(RED"CPU花在執行程式的時間:\t\t\t"YELLOW"%fs\n"
                RED"CPU於usr mode執行此程式所花的時間：\t"YELLOW"%fs\n"
                RED"CPU於krl mode執行此程式所花的時間：\t"YELLOW"%fs\n", sysTime+usrTime , usrTime, sysTime);
        printf(RED"Page fault，但沒有造成I/O：\t\t"YELLOW"%ld\n", resUsage.ru_minflt);
        printf(RED"Page fault，並且觸發I/O:\t\t"YELLOW"%ld\n", resUsage.ru_majflt);
        printf(RED"自願性的context switch：\t\t"YELLOW"%ld\n", resUsage.ru_nvcsw);
        printf(RED"非自願性的context switch:\t\t"YELLOW"%ld\n", resUsage.ru_nivcsw);
        /*
        printf(RED "return value of " YELLOW "%s" RED " is " YELLOW "%d\n", 
            argVect[0], WEXITSTATUS(wstatus));
        //printf("isSignaled? %d\n", WIFSIGNALED(wstatus));
        if (WIFSIGNALED(wstatus))
            printf(RED"the child process was terminated by a signal "YELLOW"%d"RED
                ", named " YELLOW "%s.\n",  WTERMSIG(wstatus), sys_siglist[WTERMSIG(wstatus)]);
        printf(NONE);
        */
    }
}