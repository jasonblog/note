#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
int main(int argc, char* argv[])
{
    unlink("hard_test");                        //
    printf("befor create hard link\n");
    system("ls -l hard_test");                  //

    if ((link(argv[1], "hard_test")) == -1) {   //
        perror("link");
        exit(EXIT_FAILURE);
    }

    printf("after create hard link for argv[1]\n");
    system("ls -l hard_test");                  //

    if ((unlink(argv[1])) == -1) {              //
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    printf("after runlink the hard link\n");
    system("ls -l hard_test");                  //
}

