#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <alloca.h>

extern void afunc(void);
extern etext, edata, end;

int bss_var;
int data_var = 42;
#define SHW_ADR(ID,I) printf("the %8s\t is at adr:%8x\n",ID,&I);

int main(int argc, char* argv[])
{
    char* p, *b, *nb;
    printf("Adr etext:%8x\t Adr edata %8x\t Adr end %8x\t\n", &etext, &edata, &end);
    printf("\ntext Location:\n");
    SHW_ADR("main", main);
    SHW_ADR("afunc", afunc);
    printf("\nbss Location:\n");
    SHW_ADR("bss_var", bss_var);
    printf("\ndata location:\n");
    SHW_ADR("data_var", data_var);
    printf("\nStack Locations:\n");
    afunc();
    p = (char*)alloca(32);

    if (p != NULL) {
        SHW_ADR("start", p);
        SHW_ADR("end", p + 31);
    }

    b = (char*)malloc(32 * sizeof(char));
    nb = (char*)malloc(16 * sizeof(char));
    printf("\nHeap Locations:\n");
    printf("the Heap start: %p\n", b);
    printf("the Heap end:%p\n", (nb + 16 * sizeof(char)));
    printf("\nb and nb in Stack\n");
    SHW_ADR("b", b);
    SHW_ADR("nb", nb);
    free(b);
    free(nb);
}

void afunc(void)
{
    static int long level = 0;
    int      stack_var;

    if (++level == 5) {
        return;
    }

    printf("stack_var is at:%p\n", &stack_var);
    //          SHW_ADR("stack_var in stack section",stack_var);
    //          SHW_ADR("Level in data section",level);
    afunc();
}


