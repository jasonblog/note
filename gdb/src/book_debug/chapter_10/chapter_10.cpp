#include <stdio.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <stdlib.h>

int main()
{
       pid_t child;
       long orig_rax;
       child=fork();
       if(child==0){
          ptrace(PTRACE_TRACEME,0,NULL,NULL);
          execl("/bin/ls","ls",NULL);
       }
       else
       {
            wait(NULL);
            orig_rax = ptrace(PTRACE_PEEKUSER,child,8*ORIG_RAX,NULL);
            printf("The child made a system call %ld\n",orig_rax);
            ptrace(PTRACE_CONT,child,NULL,NULL);
       }

  	return 0;     
}
