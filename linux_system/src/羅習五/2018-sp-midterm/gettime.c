/*
gettime.c
print time
*/
#include <stdio.h>
#include <time.h>
 
int main(void)
{
    time_t t = time(NULL);
    struct tm *pTM = localtime(&t);
    int yyyy = pTM->tm_year + 1900;
    int mm = pTM->tm_mon + 1;
    int dd = pTM->tm_mday;
    int hh = pTM->tm_hour;
    int min = pTM->tm_min;
    int ss = pTM->tm_sec;
     
    printf("%d-%d-%d-%d:%d:%d\n", yyyy, mm, dd, hh, min, ss);
}