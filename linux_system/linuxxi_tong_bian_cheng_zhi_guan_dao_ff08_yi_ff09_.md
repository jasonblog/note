# linux系统编程之管道（一）：匿名管道（pipe)


##一，什么是管道
管道是Linux支持的最初Unix IPC形式之一，具有以下特点：

`管道是半双工的，数据只能向一个方向流动；需要双方通信时，需要建立起两个管道`；
只能用于父子进程或者兄弟进程之间（具有亲缘关系的进程）；
单独构成一种独立的文件系统：管道对于管道两端的进程而言，就是一个文件，但它不是普通的文件，它不属于某种文件系统，而是自立门户，单独构成一种文件系统，并且只存在与内存中。
数据的读出和写入：一个进程向管道中写的内容被管道另一端的进程读出。写入的内容每次都添加在管道缓冲区的末尾，并且每次都是从缓冲区的头部读出数据。
管道的实现机制：

`管道是由内核管理的一个缓冲区`，相当于我们放入内存中的一个纸条。管道的一端连接一个进程的输出。这个进程会向管道中放入信息。管道的另一端连接一个进程的输入，这个进程取出被放入管道的信息。一个缓冲区不需要很大，它被设计成为环形的数据结构，以便管道可以被循环利用。当管道中没有信息的话，从管道中读取的进程会等待，直到另一端的进程放入信息。当管道被放满信息的时候，尝试放入信息的进程会等待，直到另一端的进程取出信息。当两个进程都终结的时候，管道也自动消失。

##二，管道的创建（pipe）
包含头文件<unistd.h>

功能:创建一无名管道

原型：

```c
int pipe(int fd[2]);
```

参数：

fd：文件描述符数组,其中fd[0]表示读端, fd[1]表示写端
返回值:成功返回0，失败返回错误代码

man帮助说明：

```c
DESCRIPTION       
       pipe() creates a pipe, a unidirectional data channel that can be used
       for interprocess communication.  The array pipefd is used to return
       two file descriptors referring to the ends of the pipe.  pipefd[0]
       refers to the read end of the pipe.  pipefd[1] refers to the write
       end of the pipe.  Data written to the write end of the pipe is
       buffered by the kernel until it is read from the read end of the
       pipe.  For further details, see pipe(7).

```


该函数创建的管道的两端处于一个进程中间，在实际应 用中没有太大意义，因此，一个进程在由pipe()创建管道后，一般再fork一个子进程，然后通过管道实现父子进程间的通信（因此也不难推出，只要两个 进程中存在亲缘关系，这里的亲缘关系指的是具有共同的祖先，都可以采用管道方式来进行通信）。父子进程间具有相同的文件描述符，且指向同一个管道pipe，其他没有关系的进程不能获得pipe（）产生的两个文件描述符，也就不能利用同一个管道进行通信。
创建管道后示意图：


