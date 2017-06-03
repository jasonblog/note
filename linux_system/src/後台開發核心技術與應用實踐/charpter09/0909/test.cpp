#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
int total_ticket_num = 20;
void* sell_ticket(void* arg)
{
    for (int i = 0; i < 20; i++) {
        if (total_ticket_num > 0) {
            sleep(1);
            printf("sell the %dth ticket\n", 20 - total_ticket_num + 1);
            total_ticket_num--;
        }
    }

    return 0;
}
int main()
{
    int iRet;
    pthread_t tids[4];
    int i = 0;

    for (i = 0; i < 4; i++) {
        int iRet = pthread_create(&tids[i], NULL, &sell_ticket, NULL);

        if (iRet) {
            printf("pthread_create error, iRet=%d\n", iRet);
            return iRet;
        }
    }

    sleep(20);
    void* retval;

    for (i = 0; i < 4; i++) {
        iRet = pthread_join(tids[i], &retval);

        if (iRet) {
            printf("tid=%d join error, iRet=%d\n", tids[i], iRet);
            return iRet;
        }

        printf("retval=%ld\n", (long*)retval);
    }

    return 0;
}
