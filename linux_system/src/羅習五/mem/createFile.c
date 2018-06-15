#define  _LARGEFILE64_SOURCE
#define  _FILE_OFFSET_BITS 64
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

const int K=1024;
const int M=1024*1024;
const int G=1024*1024*1024;

int main (int argc, char** argv) {
    int fd=open("./mmapFile", O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
    off_t off = lseek(fd, 400*M-6, SEEK_SET);
    perror("lseek");
    printf("off =%ld\n", off);
    write(fd, "hello", 6);
}