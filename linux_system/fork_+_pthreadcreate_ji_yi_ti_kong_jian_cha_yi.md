# fork + pthread_create 記憶體空間差異


```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>

/**************************************
 * fork-pthread.c
 *
 * pthread_join一般是主线程来调用，用来等待子线程退出，因为是等待，所以是阻塞的，一般主线程会依次join所有它创建的子线程。
 * pthread_exit一般是子线程调用，用来结束当前线程。
 * 一个线程对应一个pthread_join()调用，对同一个线程进行多次pthread_join()调用是逻辑错误。 
 * -1: fork() 發生錯誤, 無 child process 產生.  
 *  0: 在child process裡 
 * >0: 在parent process裡, fork() 回傳值, 即是 child process 的 PID
 *************************************/

#define DELAY 10

int g = 100;

pid_t gettid()
{
    pid_t tid;
    tid = syscall(SYS_gettid);
    return tid;
}

class B
{
private:
    int a;

public :
    B()
    {
        a = 100;
    }

    void print() {}
};


void* func(void* param)
{
    int slept = DELAY;
    printf("thread pid=%d, tid=%d, g=%d, g address=%p[+]\n", getpid(), gettid(), g,
           (void*)&g);
    g = 88;
    printf("thread pid=%d, tid=%d, g=%d, g address=%p[-]\n", getpid(), gettid(), g,
           (void*)&g);
    fflush(stdout);

    while ((slept = sleep(slept)) != 0) ;

    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    int err;
    pid_t pid;
    int status;
    pthread_t thread;
    int slept = DELAY;
    printf("main pid=%d tid=%d\n", getpid(), gettid());
    fflush(stdout);

    g = 100;

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // fils
        printf("child pid=%d tid=%d, g=%d, g address=%p[+]\n", getpid(), gettid(), g,
               (void*)&g);
        g = 50;
        printf("child pid=%d tid=%d, g=%d, g address=%p[-]\n", getpid(), gettid(), g,
               (void*)&g);

        fflush(stdout);

        while ((slept = sleep(slept)) != 0) ;

        return (EXIT_SUCCESS);
    } else {
        // père
        err = pthread_create(&(thread), NULL, &func, NULL);

        if (err != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        printf("parent pid=%d tid=%d, g=%d, g address=%p[+]\n", getpid(), gettid(), g,
               (void*)&g);

        // 主thread等到子thread結束
        err = pthread_join(thread, NULL);
        printf("parent pid=%d tid=%d, g=%d, g address=%p[-]\n", getpid(), gettid(), g,
               (void*)&g);

        if (err != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
		
		printf("waitpid=%d\n",pid);

        // 父行程等子行程結束
        int fils = waitpid(pid, &status, 0);

        if (fils == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }

        if (!WIFEXITED(status)) {
            fprintf(stderr, "Erreur de waitpid\n");
            exit(EXIT_FAILURE);
        }
    }

    return (EXIT_SUCCESS);
}
```

