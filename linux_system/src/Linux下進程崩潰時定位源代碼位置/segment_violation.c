#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "hello.h"

char* FindBaseAddress(const char* SoName) {
    static char BaseAddr[20];
    int pid = getpid();
    char file_name[50];
    sprintf(file_name, "/proc/%d/maps", pid);
    char cmd[100];
    sprintf(cmd, "grep %s %s | head -1 | awk -F- \'{print $1}\'", SoName, file_name);
    /*printf("cmd=%s",cmd);*/
    FILE* file = popen(cmd, "r");
    int n = fread(BaseAddr, 1, 20, file);
    BaseAddr[n - 1] = '\0';
    printf("The Base address of the %s is:%s\n", SoName, BaseAddr);
#if 0
    while(1) {}
#endif
    return BaseAddr;
}

int PrintLineNumber(char* So_Name, int OffsetAddress) {
    char cmd[128];
    sprintf(cmd, "addr2line -e %s 0x%x", So_Name, OffsetAddress);
    system(cmd);
}

char* FindSoName() {
    static char p[50] = {'\0'};
    char* SoName = NULL;
    int fd = open(".backtrace_file", O_RDONLY);

    if (!fd) {
        perror("No such file: .backtrace_file or you have not permission to access it\n");
    } else {
        char cmd_so[100] = "awk -F\"(\" '{if(NR==3) print$1}' .backtrace_file";
        FILE* file =  popen(cmd_so, "r");
        int n =    fread(p, 1, 50, file);
        p[n - 1] = 0;
        SoName = p;
        printf("The .so name is:%s\n", SoName);
    }

    close(fd);
    return SoName;
}

void OutputBacktrace(int sig) {
    char* bt[128];
    int backtrace_file = open(".backtrace_file", O_CREAT | O_RDWR);
    int nentries = backtrace(bt, sizeof(bt) / sizeof(bt[0]));
    backtrace_symbols_fd(bt, nentries, backtrace_file);
    //backtrace_symbols_fd(bt, nentries, fileno(stdout));
    char* So_Name = FindSoName();
    char* BaseAddress = FindBaseAddress(So_Name);
    printf("The Stack Address is:%p\n", bt[2]);
    char Stack[50];
    sprintf(Stack, "%p", bt[2]);
    int OffsetAddress = strtol(Stack, NULL, 16) - strtol(BaseAddress, NULL, 16);
    printf("OffsetAddress:0x%x\n", OffsetAddress);
    PrintLineNumber(So_Name, OffsetAddress);
    exit(-1);
}

int main() {
    int pid = getpid();
    printf("The pid is:%d\n", pid);
    signal(SIGSEGV, OutputBacktrace);
    func();
    return 0;
}
