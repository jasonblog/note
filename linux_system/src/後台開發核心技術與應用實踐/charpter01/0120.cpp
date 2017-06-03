#include<stdio.h>
int main()
{
#define TO_LITERAL(text) TO_LITERAL_(text)
#define TO_LITERAL_(text) #text
#ifndef __cplusplus
    /* this translation unit is being treated as a C one */
    printf("a C program\n");
#else
    /*this translation unit is being treated as a C++ one*/
    printf("a C++ program\n__cplusplus expands to \""
           TO_LITERAL(__cplusplus) "\"\n");
#endif
    return 0;
}
