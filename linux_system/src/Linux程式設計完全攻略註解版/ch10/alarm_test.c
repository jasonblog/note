#include<signal.h>
#include<stdio.h>
int main(void)
{
    printf("first time return:%d\n", alarm(4));
    sleep(1);
    printf("after sleep(1),remain:%d\n", alarm(2));
    printf("renew alarm,remain:%d\n", alarm(1));
}
