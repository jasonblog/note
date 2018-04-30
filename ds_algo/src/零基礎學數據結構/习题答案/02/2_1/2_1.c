#include<stdio.h>
#include<math.h>
int HuiWen(char s[])
/*回文判断，如果是回文，返回1，否则返回0*/
{
    char* start = s, *end = s + strlen(s) -
                            1; /*初始化start和end，分别指向字符串的第一个字符和最后一个字符*/

    for (; end > start;
         end--, start++) /*将start和end指向的字符依次比较，如果*start=*end，则start向后移动，end向前移动*/
        if (*start != *end) {           /*如果*start≠*end，则停止比较并退出循环*/
            break;
        }

    return end <= start;            /*如果是回文返回1，否则返回0*/
}
void main()
{
    char* s[] = {"123321", "XYZAZYX", "1232"};
    int i;

    for (i = 0; i < sizeof s / sizeof(s[0]); i++) {
        if (HuiWen(s[i]) == 1) {
            printf("字符串%s是回文!\n", s[i]);
        } else {
            printf("字符串%s不是回文!\n", s[i]);
        }
    }
}
