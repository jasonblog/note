#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
int main(void)
{
    DIR* dirp;
    struct dirent* dp1 = malloc(sizeof(struct dirent)); //
    struct dirent* dp2 = malloc(sizeof(struct dirent)); //
    dirp = opendir(".");

    while (1) {
        if ((readdir_r(dirp, dp1, &dp2)) != 0) { //
            perror("readdir");
            exit(EXIT_FAILURE);
        }

        if (dp2 == NULL) {              //NULL
            break;
        }

        if (dp2->d_name[0] == '.') {        //hidden
            continue;
        }

        printf("inode=%d\t", dp2->d_ino);
        printf("reclen=%d\t", dp2->d_reclen);
        printf("name=%s\n", dp2->d_name);

    }

    closedir(dirp);                     //close stream
    free(dp1);                      //free
    free(dp2);
    return 0;
}


