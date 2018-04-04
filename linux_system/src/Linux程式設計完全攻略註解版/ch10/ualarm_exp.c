#include<unistd.h>
#include<signal.h>
#include<errno.h>
#include<stdio.h>
void handler()
{
    printf("int:hello\n");
}
int main()
{
    int i;
    signal(SIGALRM, handler);
    printf("%d\n", ualarm(50, 20));

    //alarm(2);
    while (1) {
        sleep(1);
        printf("test\n");
    }
}
