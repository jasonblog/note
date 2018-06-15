/*
listDir.c
usage listDir .
*/

#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <linux/limits.h>

int level = 0;

void printName(char* type, char* name) {
    printf("%s", type);
    for (int i=0; i < level; i++)
        printf("  ");
    if (strcmp("d", type)==0)
        printf("+");
    else
        printf("|");
    printf("%s\n", name);
}

void listDir(char* pathName)
{
    level++;
    DIR* curDir = opendir(pathName);
    assert(curDir!=NULL);
    char* newPathName = (char*)malloc(PATH_MAX);
    struct dirent entry;
    struct dirent* result;
    int ret;
    ret = readdir_r(curDir, &entry, &result);
    while(result != NULL) {
        if (strcmp(entry.d_name, ".") == 0 || strcmp(entry.d_name, "..") == 0) {
            ret = readdir_r(curDir, &entry, &result);
            assert(ret == 0);
            continue;
        }
        assert(ret == 0);
        if (entry.d_type == DT_LNK)
            printName("l", entry.d_name);
        if (entry.d_type == DT_REG)
            printName("f", entry.d_name);
        if (entry.d_type == DT_DIR) {
            printName("d", entry.d_name);
            sprintf(newPathName, "%s/%s", pathName, entry.d_name);
            printf("%s\n", newPathName);
            listDir(newPathName);
        }
        ret = readdir_r(curDir, &entry, &result);
        assert(ret == 0);        
    }
    closedir(curDir);
    level--;
}

int main(int argc, char** argv) {
    listDir(argv[1]);
}