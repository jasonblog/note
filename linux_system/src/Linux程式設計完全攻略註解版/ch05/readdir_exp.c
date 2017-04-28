#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
int main(int argc, char* argv[])
{
    DIR* dirp;
    struct dirent* dp;
    dirp = opendir(argv[1]);        //open directory stream

    while ((dp = readdir(dirp)) != NULL) {
        if (dp->d_name[0] == '.') { //skip if it is hidden
            continue;
        }

        printf("inode=%d\t", dp->d_ino);    //print inode
        printf("reclen=%d\t", dp->d_reclen); //print size
        printf("name=%s\n", dp->d_name);    //print file name
    }

    closedir(dirp);             //close directory stream
    return 0;
}


