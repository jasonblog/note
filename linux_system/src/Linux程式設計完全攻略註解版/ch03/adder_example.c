#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#define SHW_ADR(ID,I) printf("the id %s \t is at adr:%8X\n",ID,&I);

extern etext, edata, end;

char* cptr = "Hello World\n";
char buffer1[25];
int main(void)
{
    void showit(char*);
    int i = 0;
    printf("Adr etext:%8x\tAdr edata:%8x \t Adr end :%8x \n\n", &etext, &edata,
           &end);
    SHW_ADR("main", main);
    SHW_ADR("showit", showit);
    SHW_ADR("cptr", cptr);
    SHW_ADR("buffer1", buffer1);
    SHW_ADR("i", i);
    strcpy(buffer1, "A demonstration\n");
    write(1, buffer1, strlen(buffer1) + 1);

    for (; i < 1; ++i) {
        showit(cptr);
    }

    return 0;
}

void showit(char* p)
{
    char* buffer2;
    SHW_ADR("buffer2", buffer2);

    if ((buffer2 = (char*)malloc((unsigned)(strlen(p) + 1))) != NULL) {
        strcpy(buffer2, p);
        printf("%s", buffer2);
        free(buffer2);
    } else {
        printf("Allocation error.\n");
        exit(1);
    }
}
