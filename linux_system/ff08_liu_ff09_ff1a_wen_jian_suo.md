# （六）：文件锁


当一个系统中存在多个进程同时操作同一个文件时，为了保证数据的正确， 一般会将文件上锁来避免共享文件产生的竞争状态。在linux系统下文件上锁可以使用fcntl函数来实现。
函数fcntl原型如下：


```c
#include <unistd.h>  
#include <fcntl.h>  
int fcntl(int fd, int cmd, ... /* arg */ ); 
```

函数对所打开的文件描述符fd，根据不同的cmd命令执行不同的操作，针对文件锁的命令有如下：
F_GTELK, F_SETLK, F_SETLKW三种，分别为获取锁，设置锁，同步获取锁。该函数在使用文件锁时第三个参数为一个指向结构体struct flock的指针，该结构体定义大致如下：

```c
struct flock {  
    ...  
    short l_type;    /* Type of lock: F_RDLCK, F_WRLCK, F_UNLCK */   
    short l_whence;  /* How to interpret l_start:  SEEK_SET, SEEK_CUR, SEEK_END */   
    off_t l_start;   /* Starting offset for lock */  
    off_t l_len;     /* Number of bytes to lock */  
    pid_t l_pid;     /* PID of process blocking our lock (F_GETLK only) */   
    ...  
};  
```


成员l_whence、l_start、l_len指定了我们期望加锁的文件范围；<br>
l_whence可取值：SEEK_SET、SEEK_CUR、SEEK_END，一般设为SEEK_SET。<br>
l_start为文件加锁开始的偏移处，一般设置为0，为开始处。<br>
l_len为指定加锁的字节数，如果为0则表示从开始处一直到文件末尾。<br>
l_pid为持有锁的进程ID号。<br>
l_type为要加锁的类型，可以取值F_RDLCK（读锁）、F_WRLCK（写锁）、F_UNLCK （释放锁）。<br>

任何数量的进程都可以对同一个文件区域持有一个读锁，但只能有一个进程对其持有写锁（排它锁）。单个进程只能对同一文件区域持有一种锁，如果新的锁应用在同一片已经使用了所得文件区域，那么之前存在的锁被覆盖。

F_STELK在l_type为F_RDLCK或F_WRLCK时请求一个锁，在l_type为F_UNLCK是释放一个锁，如果此时有另外的进程已经提前和获取到锁时，该调用立即返回-1。

F_SETLKW和F_STELK类似，去别在于当之前存在锁时，该函数调用将阻塞直到锁释放为止。如果阻塞期被中断信号打断将立即返回。

F_GTELK针对指向struct flock结构体类型锁去试图加锁，如果该锁可以加上（实际没有做加锁的操作），fcntl()函数将结构体中l_type置为F_UNLCK，其余成员保持不变。如果存在不兼容的锁阻止该种类型的锁加锁，fcntl将已经存在锁的详细信息写入结构体中，其中l_pid为持有该锁的进程ID。 

如果想对文件区域加读锁，那么文件描述应当是以可读方式打开，类似地，如果将对文件夹写锁，文件应当是以可写的方式打开。

文件锁释放有三种途径，一种是显示的使用F_UNLCK类型加锁，第二种是进程终止，第三种是所有针对该文件打开的文件描述全部关闭。

文件锁不会被通过fork()产生的子进程继承。

下面是通过包裹fcntl函数实现的加锁、同步加锁、解锁、测试读写锁api：

```c
/**
 * \brief     acquire, release or test for the existence of record locks
 * \details   if a conficling is held on the file, then return -1.
 *
 * \param     fd - file descriptor
 * \param     locktype - lock type: F_RDLCK, F_WRLCK, F_UNLCK.
 *
 * \return    0 is success, < 0 is failed.
 */
static int sln_filelock_set(int fd, short locktype)
{
    struct flock    lock;
    lock.l_type = locktype;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLK, &lock) < 0) {
        fprintf(stderr, "fcntl: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}
/**
 * \brief     acquire, release or test for the existence of record locks
 * \details   if a conficling is held on the file, then wait for that
 *            lock to be release
 *
 * \param     fd - file descriptor
 * \param     locktype - lock type: F_RDLCK, F_WRLCK, F_UNLCK.
 *
 * \return    0 is success, < 0 is failed.
 */
int sln_filelock_wait_set(int fd, short locktype)
{
    struct flock    lock;
    lock.l_type = locktype;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) < 0) {
        fprintf(stderr, "fcntl: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}
int sln_file_wrlock(int fd)
{
    return sln_filelock_set(fd, F_WRLCK);
}
int sln_file_rdlock(int fd)
{
    return sln_filelock_set(fd, F_RDLCK);
}
int sln_file_wait_wrlock(int fd)
{
    return sln_filelock_wait_set(fd, F_WRLCK);
}
int sln_file_wait_rdlock(int fd)
{
    return sln_filelock_wait_set(fd, F_RDLCK);
}
int sln_file_unlock(int fd)
{
    return sln_filelock_set(fd, F_UNLCK);
}
/**
 * \brief     test for the existence of record locks on the file
 * \details   none
 *
 * \param     fd - file descriptor
 * \param     locktype - lock type: F_RDLCK, F_WRLCK.
 *
 * \return    0 is success, < 0 is failed.
 */
static int sln_filelock_test(int fd, short locktype)
{
    struct flock    lock;
    lock.l_type = locktype;
    lock.l_whence = 0;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = 0;

    if (fcntl(fd, F_GETLK, &lock) < 0) {
        fprintf(stderr, "fcntl: %s\n", strerror(errno));
        return -1;
    }

    if (lock.l_type != F_UNLCK) { //file is locked
        if (F_WRLCK == lock.l_type) {
            printf("write lock hold by process <%d>, lock_type: %d\n", lock.l_pid,
                   lock.l_type);
        } else if (F_RDLCK == lock.l_type) {
            printf("read lock hold by process <%d>, lock_type: %d\n", lock.l_pid,
                   lock.l_type);
        }

        return lock.l_pid; //return the pid of process holding that lock
    } else {
        printf("Unlock, lock type: %d\n", lock.l_type);
        return 0;
    }
}
int sln_file_wrlock_test(int fd)
{
    return sln_filelock_test(fd, F_WRLCK);
}
int sln_file_rdlock_test(int fd)
{
    return sln_filelock_test(fd, F_RDLCK);
}
```

下面来实现三个进程，三个进程针对同一个文件进行读、写、测试锁操作：
写进程：

```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "filelock.h"
int main(int argc, const char* argv[])
{
    int             fd;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <write_str>\n", argv[0]);
        return -1;
    }

    fd = open("filelock.txt", O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    if (sln_file_wait_wrlock(fd) < 0) {
        printf("lock write failed!\n");
        close(fd);
        return -1;
    }

    printf("process <%d> holding write lock ok!\n", getpid());
    write(fd, argv[1], strlen(argv[1]));
    sleep(10);
    sln_file_unlock(fd);
    printf("release lock!\n");
    close(fd);
    return 0;
}
```

读进程：

```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "filelock.h"

int main(int argc, const char* argv[])
{
    int             fd;
    char            buf[1024];

    fd = open("filelock.txt", O_RDONLY | O_CREAT, 0644);

    if (fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    if (sln_file_wait_rdlock(fd) < 0) {
        printf("lock read failed!\n");
        close(fd);
        return -1;
    }

    printf("process <%d> holding read lock ok!\n", getpid());
    sleep(10);
    read(fd, buf, sizeof(buf));
    printf("read buf: %s\n", buf);
    sln_file_unlock(fd);
    printf("release lock!\n");

    close(fd);
    return 0;
}
```

测试读写锁进程：

```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "filelock.h"

int main(int argc, const char* argv[])
{
    int             fd, pid;
    fd = open("filelock.txt", O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    pid = sln_file_wrlock_test(fd);

    if (pid > 0) {
        printf("write locked!\n");
    } else {
        printf("write unlock!\n");
    }

    pid = sln_file_rdlock_test(fd);

    if (pid > 0) {
        printf("read locked!\n");
    } else {
        printf("read unlock!\n");
    }

    close(fd);
    return 0;
}
```


【CSDN 日报】| 2.17-3.17 上榜作者排行出炉      同步博客至 CSDN ，让更多开发者看到你的文章      看微博技术大咖解析互联网应用架构实战
Linux互斥与同步应用（六）：文件锁
标签： mutexfcntl
2015-01-06 23:43 952人阅读 评论(0) 收藏 举报
分类：
linux同步与互斥应用系列（6）

版权声明：本文为博主原创文章，未经博主允许不得转载。如果您觉得文章对您有用，请点击文章下面“顶”。
        【版权声明：尊重原创，转载请保留出处：blog.csdn.net/shallnet 或 .../gentleliu，文章仅供学习交流，请勿用于商业用途】
            当一个系统中存在多个进程同时操作同一个文件时，为了保证数据的正确， 一般会将文件上锁来避免共享文件产生的竞争状态。在linux系统下文件上锁可以使用fcntl函数来实现。
函数fcntl原型如下：
[cpp] view plain copy
在CODE上查看代码片派生到我的代码片

    #include <unistd.h>  
    #include <fcntl.h>  
    int fcntl(int fd, int cmd, ... /* arg */ );  

函数对所打开的文件描述符fd，根据不同的cmd命令执行不同的操作，针对文件锁的命令有如下：
F_GTELK, F_SETLK, F_SETLKW三种，分别为获取锁，设置锁，同步获取锁。该函数在使用文件锁时第三个参数为一个指向结构体struct flock的指针，该结构体定义大致如下：
[cpp] view plain copy
在CODE上查看代码片派生到我的代码片

    struct flock {  
        ...  
        short l_type;    /* Type of lock: F_RDLCK, F_WRLCK, F_UNLCK */   
        short l_whence;  /* How to interpret l_start:  SEEK_SET, SEEK_CUR, SEEK_END */   
        off_t l_start;   /* Starting offset for lock */  
        off_t l_len;     /* Number of bytes to lock */  
        pid_t l_pid;     /* PID of process blocking our lock (F_GETLK only) */   
        ...  
    };  

成员l_whence、l_start、l_len指定了我们期望加锁的文件范围；
l_whence可取值：SEEK_SET、SEEK_CUR、SEEK_END，一般设为SEEK_SET。
l_start为文件加锁开始的偏移处，一般设置为0，为开始处。
l_len为指定加锁的字节数，如果为0则表示从开始处一直到文件末尾。
l_pid为持有锁的进程ID号。
l_type为要加锁的类型，可以取值F_RDLCK（读锁）、F_WRLCK（写锁）、F_UNLCK （释放锁）。

任何数量的进程都可以对同一个文件区域持有一个读锁，但只能有一个进程对其持有写锁（排它锁）。单个进程只能对同一文件区域持有一种锁，如果新的锁应用在同一片已经使用了所得文件区域，那么之前存在的锁被覆盖。

F_STELK在l_type为F_RDLCK或F_WRLCK时请求一个锁，在l_type为F_UNLCK是释放一个锁，如果此时有另外的进程已经提前和获取到锁时，该调用立即返回-1。
F_SETLKW和F_STELK类似，去别在于当之前存在锁时，该函数调用将阻塞直到锁释放为止。如果阻塞期被中断信号打断将立即返回。
F_GTELK针对指向struct flock结构体类型锁去试图加锁，如果该锁可以加上（实际没有做加锁的操作），fcntl()函数将结构体中l_type置为F_UNLCK，其余成员保持不变。如果存在不兼容的锁阻止该种类型的锁加锁，fcntl将已经存在锁的详细信息写入结构体中，其中l_pid为持有该锁的进程ID。 

如果想对文件区域加读锁，那么文件描述应当是以可读方式打开，类似地，如果将对文件夹写锁，文件应当是以可写的方式打开。

文件锁释放有三种途径，一种是显示的使用F_UNLCK类型加锁，第二种是进程终止，第三种是所有针对该文件打开的文件描述全部关闭。

文件锁不会被通过fork()产生的子进程继承。

下面是通过包裹fcntl函数实现的加锁、同步加锁、解锁、测试读写锁api：
[cpp] view plain copy
在CODE上查看代码片派生到我的代码片

    /** 
     * \brief     acquire, release or test for the existence of record locks 
     * \details   if a conficling is held on the file, then return -1. 
     * 
     * \param     fd - file descriptor 
     * \param     locktype - lock type: F_RDLCK, F_WRLCK, F_UNLCK. 
     * 
     * \return    0 is success, < 0 is failed. 
     */  
    static int sln_filelock_set(int fd, short locktype)  
    {  
        struct flock    lock;  
        lock.l_type = locktype;  
        lock.l_whence = SEEK_SET;  
        lock.l_start = 0;  
        lock.l_len = 0;  
        lock.l_pid = getpid();  
        if (fcntl(fd, F_SETLK, &lock) < 0) {  
            fprintf(stderr, "fcntl: %s\n", strerror(errno));  
            return -1;  
        }  
        return 0;  
    }  
    /** 
     * \brief     acquire, release or test for the existence of record locks 
     * \details   if a conficling is held on the file, then wait for that 
     *            lock to be release 
     * 
     * \param     fd - file descriptor 
     * \param     locktype - lock type: F_RDLCK, F_WRLCK, F_UNLCK. 
     * 
     * \return    0 is success, < 0 is failed. 
     */  
    int sln_filelock_wait_set(int fd, short locktype)  
    {  
        struct flock    lock;  
        lock.l_type = locktype;  
        lock.l_whence = SEEK_SET;  
        lock.l_start = 0;  
        lock.l_len = 0;  
        lock.l_pid = getpid();  
        if (fcntl(fd, F_SETLKW, &lock) < 0) {  
            fprintf(stderr, "fcntl: %s\n", strerror(errno));  
            return -1;  
        }  
        return 0;  
    }  
    int sln_file_wrlock(int fd)  
    {  
        return sln_filelock_set(fd, F_WRLCK);  
    }  
    int sln_file_rdlock(int fd)  
    {  
        return sln_filelock_set(fd, F_RDLCK);  
    }  
    int sln_file_wait_wrlock(int fd)  
    {  
        return sln_filelock_wait_set(fd, F_WRLCK);  
    }  
    int sln_file_wait_rdlock(int fd)  
    {  
        return sln_filelock_wait_set(fd, F_RDLCK);  
    }  
    int sln_file_unlock(int fd)  
    {  
        return sln_filelock_set(fd, F_UNLCK);  
    }  
    /** 
     * \brief     test for the existence of record locks on the file 
     * \details   none 
     * 
     * \param     fd - file descriptor 
     * \param     locktype - lock type: F_RDLCK, F_WRLCK. 
     * 
     * \return    0 is success, < 0 is failed. 
     */  
    static int sln_filelock_test(int fd, short locktype)  
    {  
        struct flock    lock;  
        lock.l_type = locktype;  
        lock.l_whence = 0;  
        lock.l_start = 0;  
        lock.l_len = 0;  
        lock.l_pid = 0;  
        if (fcntl(fd, F_GETLK, &lock) < 0) {  
            fprintf(stderr, "fcntl: %s\n", strerror(errno));  
            return -1;  
        }  
        if (lock.l_type != F_UNLCK) { //file is locked  
            if (F_WRLCK == lock.l_type) {  
                printf("write lock hold by process <%d>, lock_type: %d\n", lock.l_pid, lock.l_type);  
            } else if (F_RDLCK == lock.l_type) {  
                printf("read lock hold by process <%d>, lock_type: %d\n", lock.l_pid, lock.l_type);  
            }  
            return lock.l_pid; //return the pid of process holding that lock  
        } else {  
            printf("Unlock, lock type: %d\n", lock.l_type);  
            return 0;  
        }  
    }  
    int sln_file_wrlock_test(int fd)  
    {  
        return sln_filelock_test(fd, F_WRLCK);  
    }  
    int sln_file_rdlock_test(int fd)  
    {  
        return sln_filelock_test(fd, F_RDLCK);  
    }  


下面来实现三个进程，三个进程针对同一个文件进行读、写、测试锁操作：
写进程：
[cpp] view plain copy
在CODE上查看代码片派生到我的代码片

    #include <stdio.h>  
    #include <sys/types.h>  
    #include <unistd.h>  
    #include <fcntl.h>  
    #include <string.h>  
    #include <errno.h>  
    #include "filelock.h"  
    int main(int argc, const char *argv[])  
    {  
        int             fd;  
        if (argc != 2) {  
            fprintf(stderr, "Usage: %s <write_str>\n", argv[0]);  
            return -1;  
        }  
        fd = open("filelock.txt", O_RDWR | O_CREAT, 0644);  
        if (fd < 0) {  
            fprintf(stderr, "open: %s\n", strerror(errno));  
            return -1;  
        }  
        if (sln_file_wait_wrlock(fd) < 0) {  
            printf("lock write failed!\n");  
            close(fd);  
            return -1;  
        }  
        printf("process <%d> holding write lock ok!\n", getpid());  
        write(fd, argv[1], strlen(argv[1]));  
        sleep(10);  
        sln_file_unlock(fd);  
        printf("release lock!\n");  
        close(fd);  
        return 0;  
    }  


读进程：
[cpp] view plain copy
在CODE上查看代码片派生到我的代码片

    #include <stdio.h>  
    #include <sys/types.h>  
    #include <unistd.h>  
    #include <fcntl.h>  
    #include <string.h>  
    #include <errno.h>  
      
    #include "filelock.h"  
      
    int main(int argc, const char *argv[])  
    {  
        int             fd;  
        char            buf[1024];  
      
        fd = open("filelock.txt", O_RDONLY | O_CREAT, 0644);  
        if (fd < 0) {  
            fprintf(stderr, "open: %s\n", strerror(errno));  
            return -1;  
        }  
      
        if (sln_file_wait_rdlock(fd) < 0) {  
            printf("lock read failed!\n");  
            close(fd);  
            return -1;  
        }  
        printf("process <%d> holding read lock ok!\n", getpid());  
        sleep(10);  
        read(fd, buf, sizeof(buf));  
        printf("read buf: %s\n", buf);  
        sln_file_unlock(fd);  
        printf("release lock!\n");  
      
        close(fd);  
        return 0;  
    }  

测试读写锁进程：
[cpp] view plain copy
在CODE上查看代码片派生到我的代码片

    #include <stdio.h>  
    #include <sys/types.h>  
    #include <unistd.h>  
    #include <fcntl.h>  
    #include <string.h>  
    #include <errno.h>  
    #include "filelock.h"  
    int main(int argc, const char *argv[])  
    {  
        int             fd, pid;  
        fd = open("filelock.txt", O_RDWR | O_CREAT, 0644);  
        if (fd < 0) {  
            fprintf(stderr, "open: %s\n", strerror(errno));  
            return -1;  
        }  
        pid = sln_file_wrlock_test(fd);  
        if (pid > 0) {  
            printf("write locked!\n");  
        } else {  
            printf("write unlock!\n");  
        }  
        pid = sln_file_rdlock_test(fd);  
        if (pid > 0) {  
            printf("read locked!\n");  
        } else {  
            printf("read unlock!\n");  
        }  
        close(fd);  
        return 0;  
    }  


编译运行三个程序。
为了测试效果，在进程读和写sleep 10秒钟。

```sh
1. 在写锁存在是，读锁是无法持有的。测试：先运行写进程，在运行读进程，读进程将在写进程释放锁时获得锁。
2. 在读锁存在时，写锁是无法持有的。测试：先运行读进程，再运行写进程，写进程将在读进程释放锁时获得锁。
3. 在读锁存在时，另外的读锁可以继续持有。测试：先后运行两个读进程，后运行的读进程可以立即获得读锁。
4.上面的测试也可可以通过测试读写锁进程得到结果。
```

http://download.csdn.net/detail/gentleliu/8332973

