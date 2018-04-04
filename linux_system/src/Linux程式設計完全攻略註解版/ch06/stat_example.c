#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#define N_BITS 3
int main(int argc, char* argv[])
{
    unsigned int i, mask = 0700;
    struct stat buff;
    static char* perm[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};

    if (argc > 1) {
        if ((stat(argv[1], &buff) != -1)) {     //
            printf("permissions for %s\t", argv[1]);

            for (i = 3; i; --i) {           //
                printf("%3s", perm[(buff.st_mode & mask) >> (i - 1)*N_BITS]);
                mask >>= N_BITS;
            }

            putchar('\n');
        } else {
            perror(argv[1]);
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Usage:%s file_name\n", argv[0]);
    }

    return 0;
}


