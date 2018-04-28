#include<stdio.h>
#include<stdlib.h>
int main(void)
{
    while (1) {
        fprintf(stderr, "\033[;\033[s"); //
        fprintf(stderr, "\033[47;31mhello world\033[5m");
        sleep(1);
        fprintf(stderr, "\033[;\033[u");
        fprintf(stderr, "\033[;\033[K");
        sleep(1);
    }

    return 0;
}
