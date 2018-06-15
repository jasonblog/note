/*執行方法：
myShell
*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
///color///
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"
#define RED_BOLD "\x1b[;31;1m"
#define BLU_BOLD "\x1b[;34;1m"
#define YEL_BOLD "\x1b[;33;1m"
#define GRN_BOLD "\x1b[;32;1m"
#define CYAN_BOLD_ITALIC "\x1b[;36;1;3m"
#define RESET "\x1b[0;m"

/*
全域變數，放解析過後的使用者指令（字串陣列）
*/
char* argVect[256];
volatile sig_atomic_t hasChild = 0;
pid_t childPid;

void ctrC_handler(int sigNumber) {
    if (hasChild) kill(childPid, sigNumber);
    else {
        ungetc("^", stdin);
        ugetc("c", stdin);
    }
}


/*
parseString：將使用者傳進的命令轉換成字串陣列
str：使用者傳進的命令
cmd：回傳執行檔
*/
void parseString(char* str, char** cmd) {
    int idx=0;
    char* retPtr;
    //printf("%s\n", str);
    retPtr=strtok(str, " \n");
    while(retPtr != NULL) {
        //printf("token =%s\n", retPtr);
        //if(strlen(retPtr)==0) continue;
        argVect[idx++] = retPtr;
        if (idx==1)
            *cmd = retPtr;
        retPtr=strtok(NULL, " \n");
    }
    argVect[idx]=NULL;
}

int main (int argc, char** argv) {
    char cmdLine[4096];
    char hostName[256];
    char cwd[256];
    char* exeName;
    int pid, pos, wstatus;
    while(1) {
        char* showPath;
        char* loginName = getlogin();
        int homeLen = 0;
        gethostname(hostName, 256);
        /*
        底下程式碼製造要顯示的路徑字串，
        會參考"home"將"home"路徑取代為~
        */
        getcwd(cwd, 256);
        pos=strspn(getenv("HOME"), cwd);
        homeLen = strlen(getenv("HOME"));
        //printf("cwd=%s, home=%s, pos=%d, prompt=%s", cwd, getenv("HOME"), pos, &cwd[pos]);
        if(pos>=homeLen) {
            cwd[pos-1]='~';
            showPath=&cwd[pos-1];
        }
        else
            showPath=cwd;
        /*
        底下程式碼負責印出提示符號
        */
        printf(LIGHT_GREEN"%s@%s:", loginName, hostName);
        printf(BLU_BOLD"%s>> ", showPath);
        printf(NONE);
        /*
        接收使用者命令，除了cd, exit以外，其他指令呼叫對應的執行檔
        */
        fgets(cmdLine, 4096, stdin);
        if (strlen(cmdLine)>1)
            parseString(cmdLine, &exeName);
        else
            continue;
        if (strcmp(exeName), "^C") == 0)
            printf("^c\n");
        if (strcmp(exeName, "exit") == 0)
            break;
        if (strcmp(exeName, "cd") == 0) {
            if (strcmp(argVect[1], "~")==0)
                chdir(getenv("HOME"));
            else
                chdir(argVect[1]);
            continue;
        }
        pid = fork();
        if (pid == 0) {
            /*
            產生一個child執行使用者的指令
            */
            if (execvp(exeName, argVect)==-1) {
                perror("myShell");
                exit(errno*-1);
            }
        } else {
            /*
            parent(myShell)等待child完成命令
            完成命令後，parent將child的結束回傳值印出來
            */
            wait(&wstatus);
            printf(RED "return value of " YELLOW "%s" RED " is " YELLOW "%d\n", 
                exeName, WEXITSTATUS(wstatus));
            printf(NONE);
        }
    }
}