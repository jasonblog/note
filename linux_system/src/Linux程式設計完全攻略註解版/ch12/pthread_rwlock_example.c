#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/pthreadtypes.h>

static pthread_rwlock_t rwlock;

#define WORK_SIZE 1024
char work_area[WORK_SIZE];
int time_to_exit;

void* thread_function_read_o(void* arg);
void* thread_function_read_t(void* arg);
void* thread_function_write_o(void* arg);
void* thread_function_write_t(void* arg);


int main(int argc, char* argv[])
{
    int res;
    pthread_t a_thread, b_thread, c_thread, d_thread;
    void* thread_result;

    res = pthread_rwlock_init(&rwlock, NULL);

    if (res != 0) {
        perror("rwlock initialization failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_create(&a_thread, NULL, thread_function_read_o,
                         NULL);//create new thread

    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_create(&b_thread, NULL, thread_function_read_t,
                         NULL);//create new thread

    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_create(&c_thread, NULL, thread_function_write_o,
                         NULL);//create new thread

    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_create(&d_thread, NULL, thread_function_write_t,
                         NULL);//create new thread

    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_join(a_thread, &thread_result);

    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_join(b_thread, &thread_result);

    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_join(c_thread, &thread_result);

    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_join(d_thread, &thread_result);

    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }

    pthread_rwlock_destroy(&rwlock);
    exit(EXIT_SUCCESS);
}

void* thread_function_read_o(void* arg)
{
    printf("thread read one try to get lock\n");

    pthread_rwlock_rdlock(&rwlock);

    while (strncmp("end", work_area, 3) != 0) {
        printf("this is thread read one.");
        printf("the characters is %s", work_area);
        pthread_rwlock_unlock(&rwlock);
        sleep(2);
        pthread_rwlock_rdlock(&rwlock);

        while (work_area[0] == '\0') {
            pthread_rwlock_unlock(&rwlock);
            sleep(2);
            pthread_rwlock_rdlock(&rwlock);
        }
    }

    pthread_rwlock_unlock(&rwlock);
    time_to_exit = 1;
    pthread_exit(0);
}

void* thread_function_read_t(void* arg)
{
    printf("thread read one try to get lock\n");
    pthread_rwlock_rdlock(&rwlock);

    while (strncmp("end", work_area, 3) != 0) {
        printf("this is thread read two.");
        printf("the characters is %s", work_area);
        pthread_rwlock_unlock(&rwlock);
        sleep(5);
        pthread_rwlock_rdlock(&rwlock);

        while (work_area[0] == '\0') {
            pthread_rwlock_unlock(&rwlock);
            sleep(5);
            pthread_rwlock_rdlock(&rwlock);
        }
    }

    pthread_rwlock_unlock(&rwlock);
    time_to_exit = 1;
    pthread_exit(0);
}

void* thread_function_write_o(void* arg)
{
    printf("this is write thread one try to get lock\n");

    while (!time_to_exit) {
        pthread_rwlock_wrlock(&rwlock);
        printf("this is write thread one.\nInput some text. Enter 'end' to finish\n");
        fgets(work_area, WORK_SIZE, stdin);
        pthread_rwlock_unlock(&rwlock);
        sleep(15);
    }

    pthread_rwlock_unlock(&rwlock);
    pthread_exit(0);
}

void* thread_function_write_t(void* arg)
{
    sleep(10);

    while (!time_to_exit) {
        pthread_rwlock_wrlock(&rwlock);
        printf("this is write thread two.\nInput some text. Enter 'end' to finish\n");
        fgets(work_area, WORK_SIZE, stdin);
        pthread_rwlock_unlock(&rwlock);
        sleep(20);
    }

    pthread_rwlock_unlock(&rwlock);
    pthread_exit(0);
}
