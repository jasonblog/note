# SIGINT 後 KILL 自己


```c
/* signal1.c */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

struct product {
    char      str[100];
    double    price;
};

struct product p1;

void mess(int sig)
{
    printf("訊號代號: %d\n", sig);
    printf("品名為 %s\n", p1.str);
    printf("售價為 %.2lf\n", p1.price);
    printf("含稅價為 %.2lf\n", p1.price * 1.05);
    (void) signal(SIGINT, SIG_DFL);
    kill(getpid(), SIGINT);
}

int main()
{
    (void) signal(SIGINT, mess);
    printf("輸入品名: ");
    scanf("%s", &p1.str);
    printf("輸入售價: ");
    scanf("%lf", &p1.price);

    while (1) {
        printf("Press CTRL-C\n");
        sleep(2);
    }

    exit(0);
}

```