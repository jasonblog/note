#include<stdio.h>
int main(int argc, char* argv[])
{
    FILE* fp = NULL;
    fp = fopen("try.txt", "r");     //

    if (fp == NULL) {       //
        printf("fail to open file!\n");
        return -1;
    }

    printf("open file successfully!\n");
    fclose(fp);             //
    return 0;
}


