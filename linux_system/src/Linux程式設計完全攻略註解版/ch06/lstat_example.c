#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    int i;
    struct stat buf;
    char* ptr;

    for (i = 1; i < argc; i++) {
        printf("%s: ", argv[i]);

        if (lstat(argv[i], &buf) < 0) {         //
            perror("lstat");
            continue;
        }

        if (S_ISREG(buf.st_mode)) {             //
            ptr = "regular file";
        } else if (S_ISDIR(buf.st_mode)) {  //
            ptr = "directory file";
        } else if (S_ISCHR(buf.st_mode)) {  //
            ptr = "character special file";
        } else if (S_ISBLK(buf.st_mode)) {  //
            ptr = "block special file";
        } else if (S_ISFIFO(buf.st_mode)) {     //
            ptr = "fifo file";
        }

#ifdef  S_ISLNK
        else if (S_ISLNK(buf.st_mode)) {    //
            ptr = "symbolic link";
        }

#endif
#ifdef  S_ISSOCK
        else if (S_ISSOCK(buf.st_mode)) {       //
            ptr = "socket";
        }

#endif
        else {
            ptr = "** unknown mode **";    //
        }

        printf("%s\n", ptr);
    }

    return 0;
}


