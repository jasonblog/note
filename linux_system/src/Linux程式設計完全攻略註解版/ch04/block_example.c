#include<stdio.h>
int main(int argc, char* argv[])
{
    struct student {
        char name[10];
        int number;
    };
    FILE* fp = NULL;
    int i;
    struct student boya[2], boyb[2], *pp, *qq;

    if ((fp = fopen("aa.txt", "w+")) == NULL) { //
        printf("can not open!\n");
        return -1;
    }

    pp = boya;
    qq = boyb;
    printf("please input two students' name and number:\n");

    for (i = 0; i < 2; i++, pp++) {
        scanf("%s\t%d", pp->name, &pp->number);
    }

    pp = boya;
    fwrite(pp, sizeof(struct student), 2, fp);  //
    rewind(fp);                             //
    fread(qq, sizeof(struct student), 2, fp); //
    printf("name\t\t number\n");

    for (i = 0; i < 2; i++, qq++) {         //
        printf("%s\t\t %d\n", qq->name, qq->number);
    }

    fclose(fp);
    return 0;
}

