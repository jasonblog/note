#include<stdio.h>
#include<string.h>
#include<malloc.h>
#define MaxLength 60

char* strcpy2(char* strDest, const char* strSrc)
/*字符串拷贝函数的实现*/
{
    char* strDestCopy;

    if ((strDest == '\0') || (strSrc == '\0')) {
        return;
    }

    strDestCopy = strDest;          /*保存字符串的首地址*/

    while ((*strDest++ = *strSrc++) != '\0');

    return strDestCopy;
}
int strcmp2(const char* dest, const char* source)
/*字符串比较函数的实现*/
/*如果dest的值大于source,则返回值大于0；如果dest的值等于source,则返回值等于0；如果dest的值小于source ,则返回值小于0*/
{
    if ((dest != NULL) && (source != NULL)) {
        return;
    }

    while (*dest != '\0' && *source != '\0' &&
           (*dest == *source)) { /*如果两个字符串不为空且值相等，则继续比较下一个字符*/
        source++;
        dest++;
    }

    return *dest - *source;
}
void main()
{
    char strSrc[] = "acaabccaabcca";
    char strDest[MaxLength];
    char dest[] = "Hello", source[] = "Hello";
    int n, f;
    /*拷贝函数的测试*/
    strcpy2(strDest, strSrc);
    printf("strDest字符串为:", n);
    puts(strDest);
    n = strlen(strcpy2(strDest, strSrc));
    printf("strDest的长度是:%2d\n", n);
    /*比较函数的测试*/
    printf("dest字符串为:");
    puts(dest);
    printf("source字符串为");
    puts(source);
    f = strcmp2(dest, source);

    if (f == 0) {
        printf("dest的值等于source\n");
    } else if (f < 0) {
        printf("dest的值等于source\n");
    } else {
        printf("dest的值等于source\n");
    }

}

