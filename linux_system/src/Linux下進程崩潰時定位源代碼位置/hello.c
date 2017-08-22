#include <stdio.h>
void func() {
    char* p = "hello world.\n";
    p[2] = 'k';  /* sigsegv segmentation fault */
    printf("%s\n", p);
}
