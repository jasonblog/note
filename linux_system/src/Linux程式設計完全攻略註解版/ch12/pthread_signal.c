#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
void* sigone_program(void* arg);
void* sigtwo_program(void* arg);
void report(int);
pthread_t thread_one, thread_two;

int main(int argc, char* argv[])
{
    int i;
    void* status;

    if (pthread_create(&thread_one, NULL, sigone_program, NULL) != 0) {
        fprintf(stderr, "pthread_create failure\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&thread_two, NULL, sigtwo_program, NULL) != 0) {
        fprintf(stderr, "pthread_create failure\n");
        exit(EXIT_FAILURE);
    }

    sleep(1);

    printf("this is parent ,send SIGUSR1,SIGUSR2 to thread %u\n", thread_one);

    if (pthread_kill(thread_one, SIGUSR1) != 0) {
        perror("pthread_kill");
        exit(EXIT_FAILURE);
    }

    if (pthread_kill(thread_one, SIGUSR2) != 0) {
        perror("pthread_kill");
        exit(EXIT_FAILURE);
    }


    printf("this is parent ,send SIGUSR1,SIGUSR2  to thread %u\n", thread_two);

    if (pthread_kill(thread_two, SIGUSR1) != 0) {
        perror("pthread_kill");
        exit(EXIT_FAILURE);
    }

    if (pthread_kill(thread_two, SIGUSR2) != 0) {
        perror("pthread_kill");
        exit(EXIT_FAILURE);
    }

    sleep(1);

    if (pthread_kill(thread_one, SIGKILL) != 0) {
        perror("pthread_kill");
        exit(EXIT_FAILURE);
    }

    printf("the end\n");
    pthread_join(thread_two, NULL);
    pthread_join(thread_one, NULL);
    return 0;
}
void* sigone_program(void* arg)
{
    int i;
    __sigset_t set;
    signal(SIGUSR1, report);
    sigfillset(&set);
    sigdelset(&set, SIGUSR2);
    pthread_sigmask(SIG_SETMASK, &set, NULL);

    for (i = 0; i < 5; i++) {
        printf("this is set mask %u thread\n", pthread_self());
        pause();
    }
}
void report(int sig)
{
    printf("\nin signal ,the sig=%d\t,the thread id=%u\n", sig, pthread_self());
}

void* sigtwo_program(void* arg)
{
    int i;
    signal(SIGUSR2, report);

    for (i = 0; i < 5; i++) {
        printf("this is no set mask %u thread\n", pthread_self());
        pause();
    }
}
