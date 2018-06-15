#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int *c;
void sighandler(int signumber) {
    printf("get a signal named '%d', '%s'\n", signumber, sys_siglist[signumber]);
    c=(int*)malloc(sizeof(int));
}
int main(int argc, char **argv) {
    assert(signal(SIGSEGV, sighandler) != SIG_ERR);
    *c = 0xC0FE;/*c沒有初始化就使用*/
    printf("press Enter to continue\n");
    getchar();
}