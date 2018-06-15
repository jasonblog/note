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

int cont = 1;
void sig_alarm(int signNo) {
    cont = 0;   //讓main跳出loop
}

int main(int argc, char **argv) {
	int pipefd[2];
	int child_pid=-1, nloop=0, nsize=0, wstatus, alarmSec=10;
	int buf[100000];
    int dest[100000];
    struct timespec start, end;
    long elapseTime;
    float timeSec;

    if (argc != 3) exit(-1);
    sscanf(argv[2], "%d", &alarmSec);
    sscanf(argv[1], "%d", &nsize);

    for(int i=0; i < 100000; i++)    //初始化buffer
        buf[i]=random();

    signal(SIGALRM, sig_alarm); //註冊alarm
    signal(SIGPIPE, SIG_IGN);   //避免有人離開造成pipe中斷，同學們可以註解掉這段程式碼看看
	
	pipe(pipefd);
	child_pid = fork();
	if (child_pid==0) {     //child;
        printf("child's pid is %d\n", getpid());
		close(pipefd[0]);	/*for read*/
        alarm(alarmSec);   //於"alarmSec"sec以後發出SIGALRM這個訊號 fork並不會繼承alarm，因此要對parent和child各設定一次
        while(cont) {
		    write(pipefd[1], buf, nsize);
            nloop++;
        }
        close(pipefd[1]);
        printf("child write: throughput\t%.2fMB/s\n", (((float)nsize* nloop)/ alarmSec/(1024*1024)));
        //getchar();
	} else {                //parent
        printf("parent's pid is %d\n", getpid());
		close(pipefd[1]);
        alarm(alarmSec);   //於"alarmSec"sec以後發出SIGALRM這個訊號, fork並不會繼承alarm，因此要對parent和child各設定一次
        while(cont) {   //一直執行，直到程式收到alarm這個signal
		    read(pipefd[0], buf, nsize);
            nloop++;
        }
        close(pipefd[0]);   //要關掉，不然child收不到EOF
        wait(&wstatus);
        printf("parent read: throughput\t%.2fMB/s\n", (((float)nsize* nloop)/ alarmSec/(1024*1024)));
        //測試單純記憶體複製的速度
        cont = 1;
        nloop = 0;
        alarm(alarmSec);
        //printf("starting test memory r/w speed...\n");
        while(cont) {
            memcpy(dest, buf, nsize);
            nloop++;
        }
        printf("speed of memcpy is \t%.2fMB/s\n", (((float)nsize* nloop)/ alarmSec/(1024*1024)));
        //getchar();
        return 0;
    }
}