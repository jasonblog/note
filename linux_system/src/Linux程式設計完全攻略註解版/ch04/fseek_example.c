#include<stdio.h>

int main(int argc, char* argv[])
{
    struct student {
        char name[10];
        int number;
    };
    FILE* fp = NULL;
    struct student student[1], *qq;

    if ((fp = fopen("aa.txt", "r")) == NULL) {
        printf("can not open file!\n");
        return -1;
    }

    fseek(fp, sizeof(struct student), SEEK_SET); //
    fread(student, sizeof(struct student), 1, fp);  //
    printf("name\t\t number\n");
    qq = student;
    printf("%s\t\t %d\n", qq->name, qq->number); //
    fclose(fp);                                 //
    return 0;
}

