# linux系统编程之管道（二）：管道读写规则


##一，管道读写规则
当没有数据可读时

- O_NONBLOCK disable：read调用阻塞，即进程暂停执行，一直等到有数据来到为止。
- O_NONBLOCK enable：read调用返回-1，errno值为EAGAIN。

当管道满的时候

- O_NONBLOCK disable： write调用阻塞，直到有进程读走数据
- O_NONBLOCK enable：调用返回-1，errno值为EAGAIN
如果所有管道写端对应的文件描述符被关闭，则read返回0

如果所有管道读端对应的文件描述符被关闭，则write操作会产生信号SIGPIPE

当要写入的数据量不大于PIPE_BUF时，linux将保证写入的原子性。

当要写入的数据量大于PIPE_BUF时，linux将不再保证写入的原子性。

##二，验证示例
示例一：O_NONBLOCK disable：read调用阻塞，即进程暂停执行，一直等到有数据来到为止。

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h> 

int main(void)
{
    int fds[2];
    if(pipe(fds) == -1){
        perror("pipe error");
        exit(EXIT_FAILURE);
    }
    pid_t pid;
    pid = fork();
    if(pid == -1){
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    if(pid == 0){
        close(fds[0]);//子进程关闭读端
        sleep(10);
        write(fds[1],"hello",5);
        exit(EXIT_SUCCESS);
    }

    close(fds[1]);//父进程关闭写端
    char buf[10] = {0};
    read(fds[0],buf,10);
    printf("receive datas = %s\n",buf);
    return 0;
}
```

结果：



![](./images/mickole/15235629-af26450d2c354dc28c2398c4b3d87c86.png)


说明：管道创建时默认打开了文件描述符，且默认是阻塞（block）模式打开

所以这里，我们让子进程先睡眠10s，父进程因为没有数据从管道中读出，被阻塞了，直到子进程睡眠结束，向管道中写入数据后，父进程才读到数据

示例二：O_NONBLOCK enable：read调用返回-1，errno值为EAGAIN。

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h> 

int main(void)
{
    int fds[2];
    if(pipe(fds) == -1){
        perror("pipe error");
        exit(EXIT_FAILURE);
    }
    pid_t pid;
    pid = fork();
    if(pid == -1){
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    if(pid == 0){
        close(fds[0]);//子进程关闭读端
        sleep(10);
        write(fds[1],"hello",5);
        exit(EXIT_SUCCESS);
    }

    close(fds[1]);//父进程关闭写端
    char buf[10] = {0};
    int flags = fcntl(fds[0], F_GETFL);//先获取原先的flags
    fcntl(fds[0],F_SETFL,flags | O_NONBLOCK);//设置fd为阻塞模式
    int ret;
    ret = read(fds[0],buf,10);
    if(ret == -1){

        perror("read error");
        exit(EXIT_FAILURE);
    }

    printf("receive datas = %s\n",buf);
    return 0;
}
```

结果：

![](./images/mickole/15235630-29b0897f5c774705a2ca3dedc16f7880.png)

示例三：如果所有管道`写端对应的文件描述符被关闭`，则read返回0

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h> 

int main(void)
{
    int fds[2];
    if(pipe(fds) == -1){
        perror("pipe error");
        exit(EXIT_FAILURE);
    }
    pid_t pid;
    pid = fork();
    if(pid == -1){
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    if(pid == 0){
        close(fds[1]);//子进程关闭写端
        exit(EXIT_SUCCESS);
    }

    close(fds[1]);//父进程关闭写端
    char buf[10] = {0};

    int ret;
    ret = read(fds[0],buf,10);
    printf("ret = %d\n", ret);

    return 0;
}
```

结果：


![](./images/mickole/15235630-2f814961e2f043a7bd735bde67c94e00.png)


可知确实返回0，表示读到了文件末尾，并不表示出错

示例四：如果所有管道`读端对应的文件描述符被关闭`，则write操作会产生信号SIGPIPE


```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

void sighandler(int signo);
int main(void)
{
    int fds[2];
    if(signal(SIGPIPE,sighandler) == SIG_ERR)
    {
        perror("signal error");
        exit(EXIT_FAILURE);
    }
    if(pipe(fds) == -1){
        perror("pipe error");
        exit(EXIT_FAILURE);
    }
    pid_t pid;
    pid = fork();
    if(pid == -1){
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    if(pid == 0){
        close(fds[0]);//子进程关闭读端
        exit(EXIT_SUCCESS);
    }

    close(fds[0]);//父进程关闭读端
    sleep(1);//确保子进程也将读端关闭
    int ret;
    ret = write(fds[1],"hello",5);
    if(ret == -1){
        printf("write error\n");
    }
    return 0;
}

void sighandler(int signo)
{
    printf("catch a SIGPIPE signal and signum = %d\n",signo);
}
```

结果：



![](./images/mickole/15235631-9f3c0544f74c41c2a10804d89813ad5d.png)

可知当所有读端都关闭时，write时确实产生SIGPIPE信号

示例五：O_NONBLOCK disable： write调用阻塞，直到有进程读走数据


```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h> 

int main(void)
{
    int fds[2];
    if(pipe(fds) == -1){
        perror("pipe error");
        exit(EXIT_FAILURE);
    }
    int ret;
    int count = 0;
    while(1){
        ret = write(fds[1],"A",1);//fds[1]默认是阻塞模式
        if(ret == -1){
            perror("write error");
            break;
        }
        count++;
    }

    return 0;
}
```

结果：


![](./images/mickole/15235632-2296c3a37df94495ba3acdae0249381c.png)

说明：fd打开时默认是阻塞模式，当pipe缓冲区满时，write操作确实阻塞了，等待其他进程将数据从管道中取走

示例六：O_NONBLOCK enable：调用返回-1，errno值为EAGAIN


```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h> 

int main(void)
{
    int fds[2];
    if(pipe(fds) == -1){
        perror("pipe error");
        exit(EXIT_FAILURE);
    }
    int ret;
    int count = 0;
    int flags = fcntl(fds[1],F_GETFL);
    fcntl(fds[1],F_SETFL,flags|O_NONBLOCK);
    while(1){
        ret = write(fds[1],"A",1);//fds[1]默认是阻塞模式
        if(ret == -1){
            perror("write error");
            break;
        }
        count++;
    }
    printf("the pipe capcity is = %d\n",count);

    return 0;
}
```

结果：

![](./images/mickole/15235632-5010fa00a1e343dabb480f505c6fceb9.png)

可知也出现EGIN错误，管道容量是65536字节

man 7 pipe说明：

```c
Pipe capacity
       A pipe has a limited capacity.  If the pipe is full, then a write(2)
       will block or fail, depending on whether the O_NONBLOCK flag is set
       (see below).  Different implementations have different limits for the
       pipe capacity.  Applications should not rely on a particular
       capacity: an application should be designed so that a reading process
       consumes data as soon as it is available, so that a writing process
       does not remain blocked.

       In Linux versions before 2.6.11, the capacity of a pipe was the same
       as the system page size (e.g., 4096 bytes on i386).  Since Linux
       2.6.11, the pipe capacity is 65536 bytes.

```

## 三，管道写与PIPE_BUF关系

man帮助说明：
```c
PIPE_BUF
       POSIX.1-2001 says that write(2)s of less than PIPE_BUF bytes must be
       atomic: the output data is written to the pipe as a contiguous
       sequence.  Writes of more than PIPE_BUF bytes may be nonatomic: the
       kernel may interleave the data with data written by other processes.
       POSIX.1-2001 requires PIPE_BUF to be at least 512 bytes.  (On Linux,
       PIPE_BUF is 4096 bytes.)  The precise semantics depend on whether the
       file descriptor is nonblocking (O_NONBLOCK), whether there are
       multiple writers to the pipe, and on n, the number of bytes to be
       written:

       O_NONBLOCK disabled, n <= PIPE_BUF
              All n bytes are written atomically; write(2) may block if
              there is not room for n bytes to be written immediately
       阻塞模式时且n<PIPE_BUF:写入具有原子性，如果没有足够的空间供n个字节全部写入，则阻塞直到有足够空间将n个字节全部写入管道       
       O_NONBLOCK enabled, n <= PIPE_BUF
              If there is room to write n bytes to the pipe, then write(2)
              succeeds immediately, writing all n bytes; otherwise write(2)
              fails, with errno set to EAGAIN.
      非阻塞模式时且n<PIPE_BUF：写入具有原子性，立即全部成功写入，否则一个都不写入，返回错误
       O_NONBLOCK disabled, n > PIPE_BUF
              The write is nonatomic: the data given to write(2) may be
              interleaved with write(2)s by other process; the write(2)
              blocks until n bytes have been written.
      阻塞模式时且n>PIPE_BUF：不具有原子性，可能中间有其他进程穿插写入，直到将n字节全部写入才返回，否则阻塞等待写入
       O_NONBLOCK enabled, n > PIPE_BUF
              If the pipe is full, then write(2) fails, with errno set to
              EAGAIN.  Otherwise, from 1 to n bytes may be written (i.e., a
              "partial write" may occur; the caller should check the return
              value from write(2) to see how many bytes were actually
              written), and these bytes may be interleaved with writes by
              other processes.
   非阻塞模式时且N>PIPE_BUF：如果管道满的，则立即失败，一个都不写入，返回错误，如果不满，则返回写入的字节数为1~n，即部分写入，写入时可能有其他进程穿插写入
当要写入的数据量不大于PIPE_BUF时，linux将保证写入的原子性。
当要写入的数据量大于PIPE_BUF时，linux将不再保证写入的原子性。
注：管道容量不一定等于PIPE_BUF

```

示例：当写入数据大于PIPE_BUF时

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>


#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

#define TEST_SIZE 68*1024

int main(void)
{
    char a[TEST_SIZE];
    char b[TEST_SIZE];
    char c[TEST_SIZE];

    memset(a, 'A', sizeof(a));
    memset(b, 'B', sizeof(b));
    memset(c, 'C', sizeof(c));

    int pipefd[2];

    int ret = pipe(pipefd);
    if (ret == -1)
        ERR_EXIT("pipe error");

    pid_t pid;
    pid = fork();
    if (pid == 0)//第一个子进程
    {
        close(pipefd[0]);
        ret = write(pipefd[1], a, sizeof(a));
        printf("apid=%d write %d bytes to pipe\n", getpid(), ret);
        exit(0);
    }

    pid = fork();

    
    if (pid == 0)//第二个子进程
    {
        close(pipefd[0]);
        ret = write(pipefd[1], b, sizeof(b));
        printf("bpid=%d write %d bytes to pipe\n", getpid(), ret);
        exit(0);
    }

    pid = fork();

    
    if (pid == 0)//第三个子进程
    {
        close(pipefd[0]);
        ret = write(pipefd[1], c, sizeof(c));
        printf("bpid=%d write %d bytes to pipe\n", getpid(), ret);
        exit(0);
    }


    close(pipefd[1]);
    
    sleep(1);
    int fd = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    char buf[1024*4] = {0};
    int n = 1;
    while (1)
    {
        ret = read(pipefd[0], buf, sizeof(buf));
        if (ret == 0)
            break;
        printf("n=%02d pid=%d read %d bytes from pipe buf[4095]=%c\n", n++, getpid(), ret, buf[4095]);
        write(fd, buf, ret);

    }
    return 0;    
}
```

结果：

![](./images/mickole/15235633-e3ddbcff513c4c9481942a5cf893004d.png)

![](./images/mickole/15235634-b80b49897f37442ca1c19bb88fbc8287.png)

可见各子进程间出现穿插写入，并没保证原子性写入，且父进程在子进程编写时边读。