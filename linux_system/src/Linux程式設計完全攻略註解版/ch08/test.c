#include<stdio.h>

int main()
{
    int ruid, euid, suid;
    getresuid(&ruid, &euid, &suid);
    printf("ruid=%d,euid=%d,suid=%d\n", ruid, euid, suid);
}


