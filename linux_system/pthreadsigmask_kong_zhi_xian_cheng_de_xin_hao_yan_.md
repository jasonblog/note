# pthread_sigmask 控制线程的信号掩码


当我们的程序里面想屏蔽某个信号的时候，我们可以使用pthread_sigmask函数进行信号掩码设置，从而屏蔽信号。

```c
#include <signal.h>
 
int pthread_sigmask(int how, const sigset_t *restrict set,
       sigset_t *restrict oset);
```


其中，how可以取值为：

- SIG_BLOCK
The resulting set shall be the union of the current set and the signal set pointed to by set.

- SIG_SETMASK
The resulting set shall be the signal set pointed to by set.

- SIG_UNBLOCK
The resulting set shall be the intersection of the current set and the complement of the signal set pointed to by set.

 

###示例一：屏蔽信号SIGINT

```c
#include <stdio.h>
#include <sys/signal.h>
#include <string.h>

int main(int argc, char** argv)
{
  pthread_t tid = pthread_self();

  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  pthread_sigmask(SIG_BLOCK, &mask, NULL);

  printf("[main][%lu] working hard ...\n", tid);

  sleep(60);

  printf("[main][%lu] good bye and good luck!\n", tid);
  return 0;
}
```

```sh
编译：gcc -o demo1 demo1.c -lpthread
```

运行后，你发现你按下CTRL+C，这个程序根本停不下来。因为SIGINT信号已经如我们所愿被屏蔽掉了。

 

###示例二：主进程创建出来的线程将继承主进程的掩码

在主线程里面pthread_create创建子线程，这个子线程会天生继承主线程对信号的“喜好”！比如主线程屏蔽SIGINT信号，子线程也天生屏蔽SIGINT信号。主线程可以处理SIGUSR1信号，子线程天生也可以处理SIGUSR1信号！


```c
#include <pthread.h>
#include <stdio.h>
#include <sys/signal.h>
#include <string.h>
 
void handler(int signo)
{
  pthread_t tid = pthread_self();
  printf("Thread[%lu] catch signo = %d\n", tid, signo);
  return;
}
 
void* run(void *param)
{
  pthread_t tid = pthread_self();
  printf(">>> Thread[%lu] Running ......\n", tid);
 
  int rc = sleep(300);
  printf("Thread[%lu] waitup(%d), and say good bye!\n", tid, rc);
  return NULL;
}
 
int main(int argc, char** argv)
{
  int ret = 0, i = 0;
  pthread_t tid = pthread_self();
  
  /* 注册SIGUSR1信号处理函数 */
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handler;
  sigaction(SIGUSR1, &sa, NULL);
 
  /* 屏蔽信号SIGINT */
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  pthread_sigmask(SIG_BLOCK, &mask, NULL);
 
  pthread_t threads[2];
  pthread_create(&threads[0], NULL, run, NULL);
  printf("[main][%lu] working hard ...\n", tid);
  sleep(1);
 
  /* 主进程创建出来的线程将继承主进程的掩码。所以子线程收不到SIGINT信号。 */
  pthread_kill(threads[0], SIGINT);
  printf("[main][%lu] send signal SIGINT ...\n", tid);
  sleep(5);
 
  /* 子线程可以收到SIGUSR1信号。 */
  pthread_kill(threads[0], SIGUSR1);
 
  pthread_join(threads[0], NULL);
 
  sleep(1);
  printf("[main][%lu] good bye and good luck!\n", tid);
  return 0;
}
```

```sh
编译：gcc -o demo2 demo2.c -lpthread
```

运行后，你可以发现，子线程果然接收不到pthread_kill发送给自己的SIGINT信号，但可以收到SIGUSR1信号！本来要休眠300s，但是收到了SIGUSR1信号，才休眠5秒就（提前295秒）醒来say goodbye了。

```sh
[main][47519069964448] working hard …
[main][47519069964448] send signal SIGINT …
>>> Thread[1100441920] Running ……
Thread[1100441920] catch signo = 10
Thread[1100441920] waitup(295), and say good bye!
[main][47519069964448] good bye and good luck!
```

### 实例三：子线程可以后天培养自己对信号的喜好


由此可见，linux里的每个线程有自己的信号掩码，所以使用pthread_kill给指定线程发送信号时，一定谨慎设置好线程的信号掩码。

当然，用kill发送信号，在多线程环境下，kill所产生的信号时传递到整个进程的，并且所有线程都有机会收到这个信号，但具体是哪个线程处理这个信号，就不一定。一般情况下，都是主线程处理这个信号。

```c
#include <pthread.h>
#include <stdio.h>
#include <sys/signal.h>
#include <string.h>
 
void handler(int signo)
{
  pthread_t tid = pthread_self();
  printf("[%u]Thread[%lu] catch signo = %d ...\n", time(NULL), tid, signo);
  sleep(20);
  printf("[%u]Thread[%lu] catch signo = %d ... done\n", time(NULL), tid, signo);
  return;
}
 
void* run(void *param)
{
  pthread_t tid = pthread_self();
  sigset_t mask;
#if 1
  /* 这种情况下，本线程屏蔽所有的信号 */
  sigfillset(&mask);
#endif
 
#if 0
  /* 这种情况下，本线程不屏蔽任何信号 */
  sigemptyset(&mask);
#endif
 
#if 0 
  /* 这种情况，本线程屏蔽以下的指定信号 */
  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  sigaddset(&mask, SIGQUIT);
  sigaddset(&mask, SIGHUP);
  sigaddset(&mask, SIGTERM);
#endif 
 
  pthread_sigmask(SIG_SETMASK, &mask, NULL);
  
  printf(">>> [%u]Thread[%lu] Running ......\n", time(NULL), tid);
 
  int rc = sleep(300);
 
  printf("[%u]Thread[%lu] waitup(%d), and say good bye!\n", time(NULL), tid, rc);
  return NULL;
}
 
int main(int argc, char** argv)
{
  pthread_t tid = pthread_self();
  
  /* 注册SIGUSR1信号处理函数 */
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handler;
  sigaction(SIGUSR1, &sa, NULL);
 
  pthread_t threads[1];
  pthread_create(&threads[0], NULL, run, NULL);
  printf("[main][%lu] working hard ...\n", tid);
  sleep(5);
 
  /* 子线程屏蔽了SIGUSR1信号，所以子线程收不到SIGUSR1信号。 */
  pthread_kill(threads[0], SIGUSR1);
  printf("[main][%lu] send signal SIGUSR1 ...\n", tid);
  sleep(5);
 
  pthread_join(threads[0], NULL);
  sleep(1);
 
  printf("[main][%lu] good bye and good luck!\n", tid);
  return 0;
}
```
```sh
编译：gcc -o demo3 demo3.c -lpthread
```

运行程序：
```sh
[amcool@leoox signal]$ ./demo3
[main][47634313767072] working hard …
>>> [1426338343]Thread[1101465920] Running ……
[main][47634313767072] send signal SIGUSR1 …
```

可见子线程果然通过后天的努力，屏蔽了SIGUSR1信号。pthread_kill发来SIGUSR1信号，子线程仍然高枕无忧的睡大觉！

这时候在另外一个终端，通过kill命令给demo3进程发送多个SIGUSR1信号，

```sh
[amcool@leoox ~]$ kill -10 360
[amcool@leoox ~]$ kill -10 360
```

你会看到主线程会排队处理这些信号，子线程收不到任何这些信号。

```sh
[main][47634313767072] working hard …
>>> [1426338343]Thread[1101465920] Running ……
[main][47634313767072] send signal SIGUSR1 …

[1426338375]Thread[47634313767072] catch signo = 10 …
[1426338395]Thread[47634313767072] catch signo = 10 … done
[1426338395]Thread[47634313767072] catch signo = 10 …
[1426338415]Thread[47634313767072] catch signo = 10 … done
[1426338643]Thread[1101465920] waitup(0), and say good bye!
[main][47634313767072] good bye and good luck!
```

### 实例四：主线程收到信号，没处理完，又来一个信号，子线程会处理吗?

```c
#include <pthread.h>
#include <stdio.h>
#include <sys/signal.h>
#include <string.h>
 
void handler(int signo)
{
  pthread_t tid = pthread_self();
  printf("[%u]Thread[%lu] catch signo = %d ...\n", time(NULL), tid, signo);
  /*
   * 信号处理函数休眠20秒，这期间再发送同一个信号，观察子线程的表现。
   */
  sleep(20);
  printf("[%u]Thread[%lu] catch signo = %d ... done\n", time(NULL), tid, signo);
  return;
}
 
void* run(void *param)
{
  pthread_t tid = pthread_self();
  printf(">>> [%u]Thread[%lu] Running ......\n", time(NULL), tid);
 
  int rc = sleep(300);
 
  printf(">>> [%u]Thread[%lu] waitup(%d), and say good bye!\n", time(NULL), tid, rc);
  return NULL;
}
 
int main(int argc, char** argv)
{
  pthread_t tid = pthread_self();
  
  /* 注册SIGUSR1信号处理函数 */
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handler;
  sigaction(SIGUSR1, &sa, NULL);
 
  pthread_t threads[1];
  pthread_create(&threads[0], NULL, run, NULL);
  printf("[main][%lu] working hard ...\n", tid);
 
  pthread_join(threads[0], NULL);
  sleep(1);
 
  printf("[main][%lu] good bye and good luck!\n", tid);
  return 0;
}
```


编译运行，运行后，其他终端发送4个SIGUSR1信号。
```sh
[amcool@leoox signal]$ gcc -o demo4 demo4.c -lpthread
[amcool@leoox signal]$ ./demo4
```

```sh
[amcool@leoox ~]$ kill -10 464
[amcool@leoox ~]$ kill -10 464
[amcool@leoox ~]$ kill -10 464
[amcool@leoox ~]$ kill -10 464
```

程序运行的结果如下：
<p>
[main][47848475240608] working hard …<br>
&gt;&gt;&gt; [1426341588]Thread[1089280320] Running ……<br>
[<span style="color: rgb(255, 0, 0);">1426341600</span>]Thread[<span style="color: rgb(255, 0, 0);">47848475240608</span>] catch signo = 10 …<br>
[<span style="color: rgb(0, 128, 0);">1426341606</span>]Thread[<span style="color: rgb(0, 128, 0);">1089280320</span>] catch signo = 10 …<br>
[<span style="color: rgb(255, 0, 0);">1426341620</span>]Thread[<span style="color: rgb(255, 0, 0);">47848475240608</span>] catch signo = 10 … done<br>
[<span style="color: rgb(0, 0, 255);">1426341620</span>]Thread[<span style="color: rgb(0, 0, 255);">47848475240608</span>] catch signo = 10 …<br>
[<span style="color: rgb(0, 128, 0);">1426341626</span>]Thread[<span style="color: rgb(0, 128, 0);">1089280320</span>] catch signo = 10 … done<br>
&gt;&gt;&gt; [1426341626]Thread[1089280320] waitup(282), and say good bye!<br>
[<span style="color: rgb(0, 0, 255);">1426341640</span>]Thread[<span style="color: rgb(0, 0, 255);">47848475240608</span>] catch signo = 10 … done<br>
[main][47848475240608] good bye and good luck!</p>


##【分析结果】

可见，当子线程天生的继承主线程的信号掩码的时候，子线程也是可以收到SIGUSR1信号，并进行处理的。

红色的日志，表明第一个SIGUSR1信号，主线程捕获，并且执行。

因为这个信号执行过程比较久（需要20秒），所以下一个SIGUSR1信号，就由子线程捕获并进行处理了（绿色日志）。这也就说明了，kill指令发送的信号是送达整个进程的，而多线程环境里面，只要没屏蔽这个信号的线程都是有机会收到信号并进入信号处理函数进行信号处理的。

子线程处理完这个信号后，之前的sleep(300)被打扰了，只能提前282秒结束了，然后子线程就退出了。

 

主线程处理完第一个信号后，立马接受第三个SIGUSR1信号，并进行处理（蓝色日志）。需要20秒。这20秒期间，子线程处理完第二个SIGUSR1信号退出了。

当主线程处理完第三个SIGUSR1信号，整个程序就结束了，所以第四个SIGUSR1信号是得不到处理的。