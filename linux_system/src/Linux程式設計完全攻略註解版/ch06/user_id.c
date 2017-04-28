#include<stdio.h>
#include<pwd.h>
#include<stdlib.h>

int main(int argc, char* argv[])
{
    struct passwd* ptr;
    uid_t uid;
    uid = atoi(argv[1]);
    ptr = getpwuid(uid);
    printf("name:%s\n", ptr->pw_name);
    printf("passwd:%s\n", ptr->pw_passwd);
    printf("home_dir:%s\n", ptr->pw_dir);
}

