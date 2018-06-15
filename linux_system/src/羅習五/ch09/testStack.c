/*
usage: testStack
*/
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>

long long i;
int main(int argc, char** argv) {
    char a;
    struct rlimit rlim;

    getrlimit(RLIMIT_STACK, &rlim);
    printf("soft=%dK, hard=%dK\n", 
        (int)rlim.rlim_cur/1024, (int)rlim.rlim_max/1024);
    printf("pid = %d", getpid());
    getchar();
    for (i=0; i<(8192 * 1024); i++) {
        if (i%1024 ==0)
            printf("%8lld kb\n",i/1024);
        *(&a - i) =1;
    }
    printf("sucess, i = %lld\n", i);
}