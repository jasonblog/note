#include<stdio.h>
int main(int argc, char* argv[])
{
    struct student {
        char name[10];
        int number;
    };
    FILE* fp = NULL;
    int i;
    int rc;
    struct student student[100], *qq;

    if ((fp = fopen("aa.txt", "r+")) == NULL) { //
        printf("can not open file!\n");
        return -1;
    }

    rc = fread(student, sizeof(struct student), 100, fp);
    printf("rc=%d\n", rc);

    if (rc != 100) {                        //
        if (feof(fp)) {                     //
            printf("read end of file!\n");
        } else {
            printf("read file error!\n");
        }
    }

    qq = student;

    for (i = 0; i < rc; i++, qq++) {
        printf("%s\t\t %d\n", qq->name, qq->number);
    }

    fclose(fp);
    return 0;
}

