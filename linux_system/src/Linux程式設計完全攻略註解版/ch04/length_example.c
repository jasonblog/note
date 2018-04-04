#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
int main(int argc, char* argv[])
{
    int n = 0;
    FILE* fp;

    if ((fp = fopen(argv[1], "r")) == NULL) { //
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    if (fseek(fp, 0, SEEK_END) != 0) {        //
        perror("fseek");
        exit(EXIT_FAILURE);
    }

    if ((n = ftell(fp)) == -1) {              //
        perror("ftell");
        exit(EXIT_FAILURE);
    }

    printf("the size count by fseek/ftell of the file is %d\n", n);   //
    printf("this is ls output:\n");
    execl("/bin/ls", "ls", "-l", argv[1], (char*)0); //
    fclose(fp);
}


