#include <stdio.h>
int main(int argc, char** argv) {
    #ifdef __STDC_NO_THREADS__ 
    printf("NO theading support\n");
    #else
    printf("have threading support\n");
    #endif
}
