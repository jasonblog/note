#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define __NR_shallnet 337

int main(int argc, const char* argv[])
{
    int     ret;

    ret = syscall(337, 99);
    //    ret = shallnet();

    printf("shallnet() return: %d\n", ret);
    return 0;
}
