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