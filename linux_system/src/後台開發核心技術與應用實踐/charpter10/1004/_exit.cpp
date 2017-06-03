#include <stdio.h>
#include <unistd.h>
int main()
{
    printf("using _exit.\n");
    printf("This is the content in buffer");
    _exit(0);
}
