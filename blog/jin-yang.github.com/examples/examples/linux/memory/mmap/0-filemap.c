#include <fcntl.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

/*
 * mmap: map a file to memmory, and change it through memmory.
 *
 * just open a file and close, then change a byte in memory
 * and flush back in the end, which equal to write file.
 */
#define BUF_SIZE 100

int main(int argc, char **argv)
{
    int fd;
    struct stat sb;
    char *mapped;

    if ((fd = open("file.txt", O_RDWR)) < 0) { // open the file
        perror("open");
        exit(EXIT_FAILURE);
    }

    if ((fstat(fd, &sb)) < -1) {    // get file status
        perror("fstat");
        exit(EXIT_FAILURE);
    }

    if ((mapped = (char *)mmap(NULL, sb.st_size, PROT_READ |
            PROT_WRITE, MAP_SHARED, fd, 0)) == (void *)-1) {
            // PROT_WRITE, MAP_PRIVATE, fd, 0)) == (void *)-1) { // 私有映射无法修改文件
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(fd); // 映射完后, 关闭文件也可以操纵内存
    printf("%s", mapped);

    // 修改一个字符,同步到磁盘文件
    mapped[2] = '9';
    if ((msync((void *)mapped, sb.st_size, MS_SYNC)) == -1) {
        perror("msync");
        exit(EXIT_FAILURE);
    }

    // 释放存储映射区
    if ((munmap((void *)mapped, sb.st_size)) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    return 0;
}
