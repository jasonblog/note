#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

const int K=1024;
const int M=1024*1024;
const int G=1024*1024*1024;

int main(int argc, char** argv) {
    int mmapFd = open("./mmapFile", O_RDWR);
    char* adrs = mmap(NULL, 400*M, PROT_READ| PROT_WRITE, MAP_PRIVATE, mmapFd, 0);
    for (random())
    assert(adrs!=MAP_FAILED);
    printf("%p\n", adrs);
}