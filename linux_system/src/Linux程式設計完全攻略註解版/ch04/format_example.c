#include<stdio.h>
#include<stdlib.h>
int main(int argc, char* argv[])
{
    char a_buf[256], b_buf[256];
    FILE* fp;

    if ((fp = fopen("./tmp", "w+")) == NULL) { //
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    printf("input a string(<256):\n");      //
    scanf("%s", a_buf);             //
    fprintf(fp, "%s", a_buf);           //
    rewind(fp);                 //
    fscanf(fp, "%s", b_buf);            //
    printf("%s\n", b_buf);              //
    fclose(fp);
    return 0;
}


