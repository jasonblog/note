#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main()
{
    int uid, euid, suid;
    getresuid(&uid, &euid, &suid);
    printf("uid=%d,euid=%d,suid=%d\n", uid, euid, suid);
    printf("after setuid(501)\n");
    printf("return=%d\n", setuid(501));
    uid = -1;
    euid = -1;
    suid = -1;
    getresuid(&uid, &euid, &suid);
    printf("uid=%d,euid=%d,suid=%d\n", uid, euid, suid);
    return 0;
}
