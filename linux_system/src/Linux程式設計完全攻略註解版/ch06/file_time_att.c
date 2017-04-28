#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<utime.h>
#include<time.h>
#include<sys/types.h>
int main(int argc, char* argv[])
{
    struct utimbuf buf;
    char* ptr;
    time_t tm;
    time(&tm);              //
    buf.actime = tm - 1000;
    buf.modtime = tm - 5000;
    ptr = ctime(&tm);               //
    printf("now,the time is:%s\n", ptr);
    ptr = ctime(&buf.actime);
    printf("modify the %s access time is:%s\n", argv[1], ptr);
    ptr = ctime(&buf.modtime);
    printf("modify the %s content mod time is:%s\n", argv[1], ptr);
    utime(argv[1], &buf);           //
    printf("pls run ls -l to check\n");
}


