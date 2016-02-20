# linux系统编程之文件与IO（六）：实现ls -l功能


本文利用以下系统调用实现ls -l命令的功能：

 

1，lstat：获得文件状态,

2，getpwuid：

```c
#include <pwd.h>

struct passwd *getpwuid(uid_t uid);

描述：

The getpwuid() function returns a pointer to a structure containing the broken-out fields of the record in the password database  that  matches the user ID uid.

返回值： 
The passwd structure is defined in <pwd.h> as follows:

    struct passwd { 
        char   *pw_name;       /* username */ 
        char   *pw_passwd;     /* user password */ 
        uid_t   pw_uid;        /* user ID */ 
        gid_t   pw_gid;        /* group ID */ 
        char   *pw_gecos;      /* real name */ 
        char   *pw_dir;        /* home directory */ 
        char   *pw_shell;      /* shell program */ 
    };
```

示例：


```c
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>

#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)

int main(void)
{
    uid_t uid;
    struct passwd *pw;
    uid = getuid();
    printf("current user id :%d\n",uid);
    if((pw = getpwuid(uid)) == NULL)
        ERR_EXIT("getpwuid error");
    printf("username:%s\n",pw->pw_name);
    printf("user passwd:%s\n",pw->pw_passwd);
    printf("user ID:%d\n",pw->pw_uid);
    printf("group ID:%d\n",pw->pw_gid);
    //printf("real name:%s\n",pw->pw_gecos);
    printf("home directory:%s\n",pw->pw_dir);
    printf("shell program:%s\n",pw->pw_shell);
    return 0;
}
```
运行结果：


