#include<stdio.h>
#include<stdlib.h>
int main(int argc, char* argv[])
{
    int i = 0;
    system("clear");

    for (i = 0; i < argc; i++) {
        printf("\033[2J\033[5;10H%s\n", argv[i]);
        sleep(1);
    }
}
