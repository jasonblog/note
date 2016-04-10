/* gcc -fpic -shared -o libmyfunc.so myfunc.c */
#include <stdio.h>
#include <unistd.h>

void showMsg() {
    printf("showMsg pid=%d\n", getpid());
    printf("hello, this is mytest\n");
}

