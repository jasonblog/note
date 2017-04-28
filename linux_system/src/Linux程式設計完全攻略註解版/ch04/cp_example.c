#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main(int argc, char* argv[])
{
    FILE* fp_src, *fp_des;
    char buf[128];
    int num;

    if (argc != 3) {          //
        printf("the format must be:cp file_src file_des\n");
        exit(EXIT_FAILURE);
    }

    if ((fp_src = fopen(argv[1], "r")) == NULL) { //
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    if ((fp_des = fopen(argv[2], "w")) == NULL) { //
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    do {
        num = fread(buf, 1, 128, fp_src);   //

        if (feof(fp_src) == 1) {    //
            break;
        }
    } while (1);

    fclose(fp_src);               //close file
    fclose(fp_des);
}


