#include<dirent.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char* argv[])
{
    char* p;
    p = getcwd(NULL, 128);
    printf("current path:%s\n", p);
    free(p);
    chdir("/home");
    printf("new path:%s\n", get_current_dir_name());
}

