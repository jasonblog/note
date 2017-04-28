#include<stdio.h>
#include<time.h>
#include<string.h>
int main()
{
    time_t timep;
    time(&timep);            //
    printf("ctime return:%s\n", ctime(&timep));  //

    time_t timep1, timep2;
    time(&timep1);           //
    time(&timep2);           //
    printf("%s\n", asctime(gmtime(&timep1))); //
    printf("%s\n", asctime(localtime(&timep2))); //

    char buff[128];
    memset(buff, '\0', 128);
    printf("globle time:");
    strftime(buff, 128, "%Z", gmtime(&timep1)); //
    printf("TZ=%s\n", buff);

    printf("local time:");
    strftime(buff, 128, "%Z", localtime(&timep2)); //
    printf("TZ=%s\n", buff);
    return 0;
}

