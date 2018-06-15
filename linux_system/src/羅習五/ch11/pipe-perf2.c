/*
使用方法： pipe-perf 讀寫pipe的長度 測試時間
*/

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <assert.h>
#include <sys/resource.h> 

int cont = 1;
const int MB = 1024*1024;
void sig_alarm(int signNo) {
    cont = 0;   //讓main跳出loop
}

double timeval2sec(struct timeval input) {
    long long us = input.tv_sec*1000000;
    us += input.tv_usec;
    //printf("%ldsec, %ld us\n", input.tv_sec, input.tv_usec);
    return (double)us/1000000.0;
}

int main(int argc, char **argv) {
	int pipefd[2];
	int child_pid=-1, nloop=0, nsize=0, wstatus, alarmSec=10;
	int buf[100000];
    int dest[100000];
    struct timespec start, end;
    long elapseTime;
    float timeSec;
    struct itimerval iTime={0};
    struct itimerval virTime={0};
    struct itimerval profTime={0};
    struct rusage resUsage;

    if (argc != 3) exit(-1);
    sscanf(argv[2], "%d", &alarmSec);
    iTime.it_value.tv_sec = alarmSec;
    virTime.it_value.tv_sec = 100;     //設定一個很大的值，讓系統不會發生signal SIGVTALRM
    profTime.it_value.tv_sec = 100;     //設定一個很大的值，讓系統不會發生signal SIGPROF
    sscanf(argv[1], "%d", &nsize);

    for(int i=0; i < 100000; i++)    //初始化buffer
        buf[i]=random();

    signal(SIGALRM, sig_alarm); //註冊alarm
    signal(SIGPIPE, SIG_IGN);   //避免有人離開造成pipe中斷，同學們可以註解掉這段程式碼看看
	
	pipe(pipefd);
	child_pid = fork();
	if (child_pid==0) {     //child;
        //printf("child's pid is %d\n", getpid());
		close(pipefd[0]);	/*for read*/
        //alarm(alarmSec);   //於"alarmSec"sec以後發出SIGALRM這個訊號 fork並不會繼承alarm，因此要對parent和child各設定一次
        assert(setitimer(ITIMER_REAL, &iTime, NULL)==0);
        assert(setitimer(ITIMER_VIRTUAL, &virTime, NULL)==0);
        assert(setitimer(ITIMER_PROF, &profTime, NULL)==0);
        while(cont) {
		    write(pipefd[1], buf, nsize);
            nloop++;
        }
        close(pipefd[1]);
        printf("\nchild write: throughput\t%.2fMB/s\n", (((float)nsize* nloop)/ alarmSec/MB));
        getitimer(ITIMER_VIRTUAL,&virTime);
        getitimer(ITIMER_PROF,&profTime);
        printf("在user space花費\t= %fsec\n", 100.0-timeval2sec(virTime.it_value));
        printf("整個時間花費\t\t= %fsec\n", 100.0-timeval2sec(profTime.it_value));
        //getchar();
	} else {                //parent
        //printf("parent's pid is %d\n", getpid());
		close(pipefd[1]);
        //alarm(alarmSec);   //於"alarmSec"sec以後發出SIGALRM這個訊號, fork並不會繼承alarm，因此要對parent和child各設定一次
        assert(setitimer(ITIMER_REAL, &iTime, NULL)==0);
        assert(setitimer(ITIMER_VIRTUAL, &virTime, NULL)==0);
        assert(setitimer(ITIMER_PROF, &profTime, NULL)==0);
        while(cont) {   //一直執行，直到程式收到alarm這個signal
		    read(pipefd[0], buf, nsize);
            nloop++;
        }
        close(pipefd[0]);   //要關掉，不然child收不到EOF
        //wait(&wstatus);
        printf("\nparent read: throughput\t%.2fMB/s\n", (((float)nsize* nloop)/ alarmSec/MB));
        getitimer(ITIMER_VIRTUAL,&virTime);
        getitimer(ITIMER_PROF,&profTime);
        printf("在user space花費\t= %fsec\n", 100.0-timeval2sec(virTime.it_value));
        printf("整個時間花費\t\t= %fsec\n", 100.0-timeval2sec(profTime.it_value));
        wait3(&wstatus, 0, &resUsage);
        //wait(&wstatus);
        //int ret=getrusage(RUSAGE_CHILDREN, &resUsage);
        //printf("ret = %d\n", ret);
        double sysTime = timeval2sec(resUsage.ru_stime);
        double usrTime = timeval2sec(resUsage.ru_utime);
        printf("total: \t%fs\n"
               "user:\t%fs\n"
               "sys:\t%fs\n", sysTime+usrTime , usrTime, sysTime);
        return 0;
    }
}