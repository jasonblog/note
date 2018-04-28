//open file try.txt with readonly mode;.on failure print "fail to open file!" then return; otherwise print "open file successfully” then close the opened file
#include<stdio.h>
int main(int argc, char* argv[])
{
    FILE* fp = NULL;
    fp = fopen("try.txt", "r");             //以只读形式打开文件try.txt

    if (fp == NULL) {               //不成功则输出"fail to open file!\n"
        printf("fail to open file!\n");
        return -1;
    }

    printf("open file successfully!\n");
    fclose(fp);                     //关闭已打开的文件
    return 0;
}
