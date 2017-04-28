#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char* argv)
{
    umask(0);           //

    if (creat("test01", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) <
        0) {
        //
        perror("creat");
        exit(EXIT_FAILURE);
    }

    umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);   //

    if (creat("test02", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) <
        0) {
        //
        perror("creat");
        exit(EXIT_FAILURE);
    }

    return 0;
}

