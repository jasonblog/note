# linux系统编程之文件与IO（四）：目录访问相关系统调用


- 1. 目录操作相关的系统调用 
    - 1.1 mkdir和rmdir系统调用 
    - 1.1.1 实例 
    - 1.2 chdir, getcwd系统调用 
    - 1.2.1 实例 
    - 1.3 opendir, closedir, readdir, 
    - 1.3.1 实例:递归便利目录
    

## 1. 目录操作相关的系统调用

### 1.1 mkdir和rmdir系统调用

```c
filename: mk_rm_dir.c 
#include <sys/stat.h> 
int mkdir(const char *path, mode_t mode); 

return: 
    S    0 
    F    -1 
note: 
    mode权限至少要有执行权限。 
```
```c
#include <unistd.h> 
int rmdir(const char *pathname); 
return: 
    S    0 
    F    -1 
note: 
    pathname目录必须是空目录。 
```

###   1.1.1 实例
```c
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <assert.h>
#define MODE    (S_IRUSR | S_IWUSR | S_IXUSR | S_IXGRP | S_IXOTH)
int main(int argc, char *argv[])
{
    char    *pname;
    assert(argc == 2);
    pname = argv[1];
    assert(mkdir(pname, MODE) == 0);
    printf("create %s successful!\n", pname);
    assert(rmdir(pname) == 0);
    return 0;
}
```
###  1.2 chdir, getcwd系统调用

```c
#include <unistd.h> 
int chdir(const char *pathname); 
return: 
    S    0 
    F    -1 

#include <unistd.h> 
char *getpwd(char *buf, size_t size); 
return: 
    S    buf 
    F    NULL 

    buf是缓冲地址，size是buf的长度。该缓冲必须有足够的长度以容纳绝对路径名加上一个null终止符。
    
```

###     1.2.1 实例

- filename:ch_get_dir.c

```c
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#define BUFSIZE    (50)
int main(void)
{
    char buf[BUFSIZE];
    memset((void *)buf, '\0', sizeof buf);
    assert(chdir("/tmp") == 0);
    printf("chdir to /tmp successful\n");
    assert(getcwd(buf, BUFSIZE) != NULL);
    printf("now the directory is %s\n", buf);
    return 0;
}
```
测试：

```c
[qtlldr@qtldr editing]$ ./ch_get_dir 
chdir to /tmp successful 
now the directory is /tmp 
[qtlldr@qtldr editing]$  
```

### 1.3 opendir, closedir, readdir


```c
#include <sys/type.s> 
#include <dirent.h> 
DIR *opendir(const char *dirname); 
return: 
    S    DIR指针 
    F    NULL 
note: 
    DIR是一种目录结构，类似FILE。 

#include <sys/types.h> 
#include <dirent.h> 
struct dirent *readir(DIR *dirp); 
return: 
    S    一个指向保存目录流下一个目录项的dirent指针 
    F    NULL 
note: 
    struct dirent { 
        char    d_name[NAME + 1]; /* \0结尾的文件名 */ 
    } 
    到达目录尾或出错返回NULL，但是到达目录尾不会设置errno，出错则设置。 
    如果在readir的同时有其他进程在目录中创建或者删除文件爱你，readdir不保证能列处该目录中所有文件。

#include <sys/types.h> 
#include <dirent.h> 
int closedir(DIR *dirp); 
return: 
    S    0 
    F    -1 
```

###   1.3.1 实例:递归便利目录

```c
filename:help.txt 帮助文档 
            本程序只为学习linux目录操作而写 
printdir 
    输出目录文件或者统计目录中的文件数目 
语法： 
    printdir [option] <files...> 
选项： 
    -l 
        输出目录下的文件名 
    -c 
        统计目录下的文件 
    -d n 
        指定最大层次，最大为30 
默认行为： 
    如果没有指定选项，那么只输出该目录下的文件名 
BUG: 
    -l与 -c选项不能同时使用，如果同时使用统计出错。(以后会修正） 

            本程序只为学习linux目录操作而写 
```

- filename:printdir.c

```c

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define INDENT_DEPTH    (4) /* 列举文件时的缩进数 */
#define DEPTH_MAX (30)    /* 递归便利的最大层次 */
#define HELPFILE ("help.txt")
typedef int count_t;
struct nfiletype {
    count_t ndir;
    count_t nreg;
    count_t nchr;
    count_t    nfifo;
    count_t    nsock;
    count_t nchar;
    count_t nblock;
    count_t    nlink;
    count_t ntotol;
    count_t nunknow;
};/*记录各个类型文件的数目*/
int    DEPTH = 20; /* 递归层级限制 */
int idepth_count = 1;
int idepth_print = 1;

static struct nfiletype *count_files(const char *pathname,
            struct nfiletype *nfile);

static void  printdir(const char *pathname, int indent);
int main(int argc, char **argv)
{
    int opt;
    int    depth_opt;
    int    count_flag = 0;
    int    print_flag = 0;
    char *parg = NULL;
    struct nfiletype nfiles = {0};
    int    fd_help;
    char buf_help[BUFSIZ];
    int nread_help;
    char *filename_help = HELPFILE;
    while ((opt = getopt(argc, argv, "lhd:c")) != -1) {
        switch (opt) {
            case 'l':
                print_flag = 1;
                break;
            case 'c':
                count_flag = 1;
                break;
            case 'd':
                depth_opt = strtol(optarg, NULL, 10);
                DEPTH = depth_opt <= DEPTH_MAX ? depth_opt: DEPTH;
                break;
            case ':':
                printf("option needs a value\n");
                break;
            case '?':
                printf("unknown option :%c\n", optopt);
                break;
            case 'h':
                fd_help = open(filename_help, O_RDONLY);
                if (fd_help != -1) {
                    while ((nread_help = read(fd_help, buf_help, BUFSIZ)) > 0) {
                        write(1, buf_help, nread_help);
                    }
                    close(fd_help);
                } else {
                    fprintf(stderr, "open %s failed!\n", filename_help);
                }
                return 0;
        }
    }
    /* 如果没有选项，那么默认是打印目录 */
    if (!print_flag && !count_flag)
        print_flag = 1;
    for( ; optind < argc; optind++) {
        parg = argv[optind];
        if (print_flag) {
            //printf("DEBUG-- printdir --%s\n", parg);
            printdir(parg, 4);
        }
        if (count_flag) {
            memset((void *)&nfiles, '\0', sizeof nfiles);
            //printf("DEBUG-- count_files--%s\n", parg);
            count_files(parg, &nfiles);
            printf("In the %s there are :\n", parg);
            printf("     directory %d\n", nfiles.ndir);
            printf("     regular file %d\n", nfiles.nreg);
            printf("     specal character file %d\n", nfiles.nchr);
            printf("     special block file %d\n", nfiles.nblock);
            printf("     fifo file %d\n", nfiles.nfifo);
            printf("     sock file %d\n", nfiles.nsock);
            printf("     link file %d\n", nfiles.nlink);
            printf("     unknown file %d\n", nfiles.nunknow);
            printf("Total %d\n", nfiles.ntotol);
        }
    }
    return 0;
}
/*
 *function: 对该目录下的文件类型进行统计
 * input arg:
 *        pathname:目录名指针
 *        nfile:记录文件类型数目的结构体指针
 * return：
 *        记录文件类型数目的结构体指针
 */

static struct nfiletype *count_files(const char *pathname,
            struct nfiletype *nfile)
{
    DIR *dp;
    struct dirent *entry;
    struct stat    statbuf;
    //printf("DEBUG-- in count_files -- %s\n", pathname);
    /* 层次控制 */    
    if (idepth_count > DEPTH)
        return NULL;
    idepth_count++;
    if ((dp = opendir(pathname)) == NULL) {
        fprintf(stderr, "can not open %s\n", pathname);
        return NULL;
    }
    chdir(pathname);
    while ((entry = readdir(dp)) != NULL) {
        /* 跳过 . 和 .. */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        /* 取得文件信息 */
        if (lstat(entry->d_name, &statbuf) == -1) {
            fprintf(stderr, "can not test the %s's type\n", entry->d_name);
            return NULL;
        }
        /* 统计文件数目 */
        if (S_ISDIR(statbuf.st_mode)) { /* 是目录就递归吧 */
            //printf("DEBUG -- directory %s\n", entry->d_name);
            count_files(entry->d_name, nfile);
            nfile->ndir++;
        }
        else if (S_ISREG(statbuf.st_mode)) {
            //printf("DEBUG -- regular file %s\n", entry->d_name);
            nfile->nreg++;
        }
        else if (S_ISCHR(statbuf.st_mode))
            nfile->nchr++;
        else if (S_ISBLK(statbuf.st_mode))
            nfile->nblock++;
        else if (S_ISLNK(statbuf.st_mode))
            nfile->nlink++;
        else if (S_ISFIFO(statbuf.st_mode))
            nfile->nfifo++;
        else if (S_ISSOCK(statbuf.st_mode))
            nfile->nsock++;
        else nfile->nunknow++;
        nfile->ntotol++;
    }
    chdir("..");
    closedir(dp);
    return nfile;
 }
 /*
nblock; *function:列出目录中的文件
nlink;  *input arg:
ntotol; *        pathname: 目录名
 *return:
 *        void
 */

static void   printdir(const char *pathname, int indent)
{
    DIR *dp;
    struct dirent *entry;
    struct stat    statbuf;
    /* 层次控制 */    
    if (idepth_print > DEPTH)
        return ;
    idepth_print++;
    if ((dp = opendir(pathname)) == NULL) {
        fprintf(stderr, "can not open %s\n", pathname);
        return ;
    }
    chdir(pathname);
    while ((entry = readdir(dp)) != NULL) {
        /* 跳过 . 和 .. */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (lstat(entry->d_name, &statbuf) == -1) {
            fprintf(stderr, "can not test the %s's type\n", entry->d_name);
            return ;
        }
        if (S_ISDIR(statbuf.st_mode)) { /* 是目录就递归吧 */
            printf("%*s%s/\n", indent," ",  entry->d_name);
            printdir(entry->d_name, indent + INDENT_DEPTH);
        }
        else {
            printf("%*s%s\n", indent," ", entry->d_name);
            }
    }
    chdir("..");
    closedir(dp);
}
```
