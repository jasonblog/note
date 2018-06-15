/*
name2id.c
usage: name2id username
*/
#include <pwd.h>
#include <stdio.h>
#include <sys/types.h>
#include <assert.h>
int main(int argc, char** argv)
{
    struct passwd *user;
    user= getpwnam(argv[1]);
    assert(user!=NULL);
    printf("name:%s\n", user->pw_name);
    printf("uid:%d\n", user->pw_uid);
    printf("gid:%d\n", user->pw_gid);
}