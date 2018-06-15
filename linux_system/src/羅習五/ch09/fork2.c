/*
usage: fork2
*/
#include <stdio.h>
#include <unistd.h>
int main()
{
    int var = 0;
    pid_t pid;
    printf("<<before fork>>");
    pid = fork();
    if(pid == 0) {          /* child 執行 */
        var = 1;
    } else if (pid > 0) {   /* parent 執行 */
        var = 2;
    }
    printf("<<%d>>", var);
    return 0;
}