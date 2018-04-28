#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

void* thread_function(void* arg);

pthread_mutex_t work_mutex;

#define WORK_SIZE 1024
char work_area[WORK_SIZE];
int time_to_exit = 0;

int main(int argc, char* argv[])
{
    int res;
    pthread_t a_thread;
    void* thread_result;
    res = pthread_mutex_init(&work_mutex, NULL); //init mutex

    if (res != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_create(&a_thread, NULL, thread_function, NULL);//create new thread

    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&work_mutex);            //lock the mutex
    printf("Input some text. Enter 'end' to finish\n");

    while (!time_to_exit) {
        fgets(work_area, WORK_SIZE, stdin);     //get a string from stdin
        pthread_mutex_unlock(&work_mutex);      //unlock the mutex

        while (1) {
            pthread_mutex_lock(&work_mutex);    //lock the mutex

            if (work_area[0] != '\0') {
                pthread_mutex_unlock(&work_mutex);  //unlock the mutex
                sleep(1);
            } else {
                break;
            }
        }
    }

    pthread_mutex_unlock(&work_mutex);
    printf("\nWaiting for thread to finish...\n");
    res = pthread_join(a_thread, &thread_result);

    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }

    printf("Thread joined\n");
    pthread_mutex_destroy(&work_mutex);
    exit(EXIT_SUCCESS);
}

void* thread_function(void* arg)
{
    sleep(1);
    pthread_mutex_lock(&work_mutex);

    while (strncmp("end", work_area, 3) != 0) {
        printf("You input %d characters\n", strlen(work_area) - 1);
        printf("the characters is %s", work_area);
        work_area[0] = '\0';
        pthread_mutex_unlock(&work_mutex);
        sleep(1);
        pthread_mutex_lock(&work_mutex);

        while (work_area[0] == '\0') {
            pthread_mutex_unlock(&work_mutex);
            sleep(1);
            pthread_mutex_lock(&work_mutex);
        }
    }

    time_to_exit = 1;
    work_area[0] = '\0';
    pthread_mutex_unlock(&work_mutex);
    pthread_exit(0);
}

