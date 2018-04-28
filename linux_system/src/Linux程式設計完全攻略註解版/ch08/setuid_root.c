#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
int main(int argc, char* argv[])
{

    printf("uid=%d\teuid=%d\n", getuid(), geteuid());
    printf("setreuid 0,500");
    setreuid(0, 500);
    printf("uid=%d\teuid=%d\n", getuid(), geteuid());
    setuid(0);
    printf("uid=%d\teuid=%d\n", getuid(), geteuid());
    system("passwd");
}
