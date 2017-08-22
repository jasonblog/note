#include <string.h>
#include <stdio.h>

int main(void) {
    char s[10];
    /* memset(s, 0, 0); */
    memset(s, 0, 1);
    putchar('X');
    putchar('\n');
    return 0;
}
