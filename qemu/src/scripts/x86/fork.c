#include <unistd.h>
#include <stdio.h>

int main()
{
   fork();
   printf("%d\n",getpid());
   return 0;
}
