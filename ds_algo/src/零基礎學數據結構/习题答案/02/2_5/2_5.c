#include<stdio.h>
#include<malloc.h>
#include<string.h>
#define N 30
void CopyString(char** dest, char* source, int m);

void CopyString(char** dest, char* source, int m)
/*将字符串source中的第m个以后的字符复制到dest中*/
{
    int i, n;
    n = strlen(source);     /*将字符串source的长度*/
    *dest = (char*)malloc(sizeof(n - m + 2)); /*为字符串dest分配内存空间*/

    for (i = m - 1; source[i] != '\0';
         i++) { /*将字符串source的第m个以后的字符复制到dest中*/
        ((*dest)[i - m + 1]) = source[i];
    }

    *(*dest + n - m + 2) = '\0'; /*在字符串dest的最后添加结束标记*/

}

void main()
{
    int m = 3, a[N];
    char* source = "Hello DataSructure!", *dest;
    CopyString(&dest, source, m);
    printf("字符串source中的的内容是:%s\n", source);
    printf("将source中的第%d个以后的字符复制到dest后，dest的内容是:\n", m);
    printf("%s", dest);
    printf("\n");
}




