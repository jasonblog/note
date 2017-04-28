#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
int output(sigset_t set);

int main()
{
    sigset_t set;
    printf("after empty the set:\n");
    sigemptyset(&set);
    output(set);

    printf("after add signo=2:\n");
    sigaddset(&set, 2);
    output(set);
    printf("after add signo=10:\n");
    sigaddset(&set, 10);
    output(set);

    sigfillset(&set);
    printf("after  fill all:\n");
    output(set);
    return 0;
}

int output(sigset_t set)
{
    int i = 0;

    for (i = 0; i < 1; i++) { //can test i<32
        printf("0x%8x\n", set.__val[i]);

        if ((i + 1) % 8 == 0) {
            printf("\n");
        }
    }
}
