#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#define BUFSIZE 128
int main(int argc, char* argv[])
{
    char buf[BUFSIZE];
    memset(buf, '\0', BUFSIZE);
    unlink("sym_link_test");                        //

    if ((symlink(argv[1], "sym_link_test")) == -1) { //
        perror("symlink");
        exit(EXIT_FAILURE);
    }

    if ((readlink("sym_link_test", buf, BUFSIZE)) == -1) { //
        perror("readlink");
        exit(EXIT_FAILURE);
    }

    printf("sym_link_test is the symbol link of %s\n", argv[1]);
    system("ls -l  sym_link_test");             //
}

