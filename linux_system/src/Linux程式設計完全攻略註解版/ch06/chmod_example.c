#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char* argv[])
{
    struct stat statbuf;

    if (stat("test01", &statbuf) < 0) {                //
        perror("stat");
        exit(EXIT_FAILURE);
    }

    if (chmod("test02", (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0) { //
        perror("stat");
        exit(EXIT_FAILURE);
    }

    if (chmod("test03", S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH) < 0) { //
        perror("stat");
        exit(EXIT_FAILURE);
    }

    return 0;
}


