---
title: Linux 信号机制
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,signal,信号
description: 软中断信号 (简称为信号) 是用来通知进程发生了异步事件，是在软件层次上是对中断机制的一种模拟，在原理上，一个进程收到一个信号与处理器收到一个中断请求可以说是一样的。信号是进程间通信机制中唯一的异步通信机制，一个进程不必通过任何操作来等待信号的到达，事实上，进程也不知道信号到底什么时候到达。进程之间可以互相通过系统调用 kill 发送软中断信号，内核也可以因为内部事件而给进程发送信号，通知进程发生了某个事件。信号机制除了基本通知功能外，还可以传递附加信息。
---

软中断信号 (简称为信号) 是用来通知进程发生了异步事件，是在软件层次上是对中断机制的一种模拟，在原理上，一个进程收到一个信号与处理器收到一个中断请求可以说是一样的。

信号是进程间通信机制中唯一的异步通信机制，一个进程不必通过任何操作来等待信号的到达，事实上，进程也不知道信号到底什么时候到达。

进程之间可以互相通过系统调用 kill 发送软中断信号，内核也可以因为内部事件而给进程发送信号，通知进程发生了某个事件。

信号机制除了基本通知功能外，还可以传递附加信息。

<!-- more -->

## 信号处理

每个信号在 `signal.h` 头文件中通过宏进行定义，对于 CentOS 来说，实际是在 `/usr/include/asm-generic/signal.h` 中定义，对于编号以及信号名的映射关系可以通过 `kill -l` 命令查看。

其中，[1, 31] 是普通信号 而 [34, 64] 是实时信号。

对于信号，通常有如下的几种处理方式：

1. 忽略。大部分信号都可以通过这种方式处理，不过 `SIGKILL` 和 `SIGSTOP` 两个信号有特殊用处，不能被忽略。
2. 默认动作。大多数信号的系统默认动作终止该进程。
3. 捕捉信号。也就是在收到信号时，执行一些用户自定义的函数。

其中信号可以简单通过 `signal()` 函数指定。


### 简单处理信号

其中 `signal()` 函数的声明如下：

{% highlight c %}
#include <signal.h>
typedef void(*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
{% endhighlight %}

其中，`signal()` 用于对该进程的某个特定信号 (signum) 注册一个相应的处理函数，也就是修改对该信号的默认处理动作。

注意，`signal()` 会堵塞当前正在处理的信号，不过不能阻塞其它信号，比如正在处理 `SIG_INT`，再来一个 `SIG_INT` 则会堵塞，但如果是 `SIG_QUIT` 则会被其中断，在处理完 `SIG_QUIT` 信号之后，`SIG_INT` 才会接着刚才处理。

{% highlight c %}
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handler(int signum)
{
	printf("Got a signal %d\n", signum);
}

int main()
{
	signal(SIGINT, handler);

	while(1) {
		sleep(1);
		printf(".");
		fflush(stdout);
	}
	return 0;
}
{% endhighlight %}

也可以通过如下方式忽略某一个信号。

{% highlight c %}
signal(SIGHUP, SIG_IGN);
{% endhighlight %}

这里表示忽略 `SIGHUP` 这个信号，该信号与控制台有关，当控制台被关闭时，操作系统会向拥有控制台 SessionID 的所有进程发送 HUP 信号，而默认 HUP 信号的处理是退出程序。当远程登陆启动某个服务进程并在程序运行时关闭终端的话会导致服务进程退出，所以一般服务进程都会用 nohup 工具启动或写成一个 daemon。

## 信号处理过程

进程收到一个信号后不会被立即处理，而是在恰当时机进行处理！一般是在中断返回的时候，或者内核态返回用户态的时候 (这个情况出现的比较多)。

也就是说，信号不一定会被立即处理，操作系统不会为了处理一个信号而把当前正在运行的进程挂起，因为这样的话资源消耗太大了，如果不是紧急信号，是不会立即处理的，操作系统多选择在内核态切换回用户态的时候处理信号。

因为进程有可能在睡眠的时候收到信号，操作系统肯定不愿意切换当前正在运行的进程，于是就得把信号储存在进程唯一的 PCB(task_struct) 当中。

### 信号触发

一般信号的触发大致可以分为如下的几类：

1. 用户在终端中触发信号，终端驱动程序会发送信号给前台程序。例如 `Ctrl-C(SIGINT)`、`Ctrl-\(SIGQUIT)`、`Ctrl-Z(SIGTSTP)` 。
2. 硬件异常产生信号。通常由硬件检测到并通知内核，然后内核向当前进程发送适当的信号。例如，除 0 异常 (CPU运算单元触发，内核通过`SIGFPE`信号发送给当前进程)；内存非法地址访问 (MMU触发异常，内核通过`SIGSEGV`信号发送给当前进程)。
3. 一个进程调用 `kill()` 函数可以发送信号给另一个进程，默认会发送 `SIGTERM` 信号，该信号的默认处理动作是终止进程。

当 CPU 正在执行某个进程时，通过终端驱动程序发送了一个 SIGINT 信号给该进程，该信号会记录在响应进程的 PCB 中，则该进程的用户空间代码暂停执行，CPU 从用户态切换到内核态处理硬件中断。

从内核态回到用户态之前，会先处理 PCB 中记录的信号，发现有一个 `SIGINT` 信号待处理，而这个信号的默认处理动作是终止进程，所以直接终止进程而不再返回它的用户空间代码执行。

#### 函数调用

通过 `raise()` 可以给当前进程发送指定的信号；`kill()` 函数向指定进程发送信号；而 `abort()` 函数使当前进程接收到 `SIGABRT` 信号，其函数声明如下：

{% highlight c %}
#include<signal.h>
int kill(pid_t pid,int signo);
int raise(int signo);

#include<stdlib.h>
void abort(void);
{% endhighlight %}

类似于 `exit()` 函数，`abort()` 函数总是会成功的，所以没有返回值。

### 信号阻塞

信号在内核中的表示大致分为如下几类：

1. 信号递达 (delivery) 实际执行信号处理信号的动作。
2. 信号未决 (pending) 信号从产生到抵达之间的状态，信号产生了但是未处理。
3. 忽略，抵达之后的一种动作。
4. 阻塞 (block) 收到信号不立即处理，被阻塞的信号将保持未决状态，直到进程解除对此信号的阻塞，才执行抵达动作。

每个信号都由两个标志位分别表示阻塞和未决，以及一个函数指针表示信号的处理动作。

![signal task structure]({{ site.url }}/images/linux/kernel/signal-task-struct-stuff.png "signal task structure"){: .pull-center }

在上图的例子中，其状态信息解释如下：

* `SIGHUP` 未阻塞也未产生过，当它递达时执行默认处理动作。
* `SIGINT` 信号产生过，但正在被阻塞，所以暂时不能递达。虽然它的处理动作是忽略，但在没有解除阻塞之前不能忽略这个信号，因为进程仍有机会改变处理动作之后再解除阻塞。
* `SIGQUIT` 信号未产生过，一旦产生 `SIGQUIT` 信号将被阻塞，它的处理动作是用户自定义函数 sighandler。

信号产生但是不立即处理，前提条件是要把它保存在 pending 表中，表明信号已经产生。

### 信号集操作函数

信号集用来描述信号的集合，每个信号占用一位，总共 64 位，Linux 所支持的所有信号可以全部或部分的出现在信号集中，主要与信号阻塞相关函数配合使用。

执行信号的处理动作称为信号递达 (Delivery)，信号从产生到递达之间的状态，称为信号未决 (Pending)，进程可以选择阻塞 (Block) 某个信号，被阻塞的信号产生时将保持在未决状态，直到进程解除对此信号的阻塞，才执行递达的动作。

注意，阻塞和忽略是不同的，信号被阻塞就不会递达，而忽略是在递达之后可选的一种处理动作。

每个进程都有一个用来描述哪些信号递送到进程时将被阻塞的信号集，该信号集中的所有信号在递送到进程后都将被阻塞，信号在内核中的表示可以看作是这样的：

如下是常见的信号集的操作函数：

{% highlight c %}
#include <signal.h>
int sigemptyset(sigset_t *set);                  /* 所有信号的对应位清0 */
int sigfillset(sigset_t *set);                   /* 设置所有的信号，包括系统支持的所有信号 */
int sigaddset(sigset_t *set, int signo);         /* 在该信号集中添加有效信号 */
int sigdelset(sigset_t *set, int signo);         /* 在该信号集中删除有效信号 */
int sigismember(const sigset_t *set, int signo); /* 用于判断一个信号集的有效信号中是否包含某种信号 */
{% endhighlight %}

如下是一个操作示例。

{% highlight c %}
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

void print_sigset(sigset_t *set)
{
	int i;
	for(i = 1; i < NSIG; ++i){
		if(sigismember(set, i))
			putchar('1');
		else
			putchar('0');
	}
	putchar('\n');
}

int main(void)
{
	sigset_t foobar;

	sigemptyset(&foobar);
	sigaddset(&foobar, SIGINT);
	sigaddset(&foobar, SIGQUIT);
	sigaddset(&foobar, SIGUSR1);
	sigaddset(&foobar, SIGRTMIN);

	print_sigset(&foobar);

	return 0;
}
{% endhighlight %}

调用 `sigprocmask()` 函数可以读取或更改进程的信号屏蔽字：

{% highlight c %}
#include <signal.h>
int sigprocmask(int how, const sigset_t *set, sigset_t *oset);
{% endhighlight %}

一个进程的信号屏蔽字规定了当前阻塞而不能递送给该进程的信号集，如果调用该函数解除了对当前若干个未决信号的阻塞，则在该函数返回前，至少将其中一个信号递达。

### 实时信号

实时信号支持队列，可以保证信号不会丢失，其中 1-31 个信号为非实时信号，32-64 为实时信号，当一信号在阻塞状态下产生多次信号，当解除该信号的阻塞后，非实时信号仅传递一次信号，而实时信号会传递多次。

对于非实时信号，内核会为每个信号维护一个信号掩码，并阻塞信号针对该进程的传递。如果将阻塞的信号发送给某进程，对该信号的传递将延时，直至从进程掩码中移除该信号为止。当从进程掩码中移除该信号时该信号将传递给该进程。如果信号在阻塞期间传递过多次该信号，信号解除阻塞后仅传递一次。

对于实时信号，实时信号采用队列化处理，一个实时信号的多个实例发送给进程，信号将会传递多次。可以制定伴随数据，用于产生信号时的数据传递。不同实时信号的传递顺序是固定的，优先传递信号编号小的。


## Signal VS. Sigaction

实际上，上述的 `signal()` 是最早的函数，现在大多系统，包括 Linux 都用 `sigaction()` 重新实现了 `signal()`，其区别如下：

1. `signal()` 注册的回调函数，会在调用前先清除掉，所以需要在回调函数中重新注册；而 `sigaction()` 函数如果要删除需要显示调用。
2. `signal()` 处理不能阻塞信号，而 `sigaction()` 则可以阻塞指定的信号。

这也就意味着，`signal()` 函数可能会丢失信号。

如下是两个函数的声明。

{% highlight c %}
#include <signal.h>
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);

#include <signal.h>
struct sigaction {
	void     (*sa_handler)(int);                        /* 信号处理方式 */
	void     (*sa_sigaction)(int, siginfo_t *, void *); /* 实时信号的处理方式 */
	sigset_t   sa_mask;   /* 额外屏蔽的信号 */
	int        sa_flags;
	void     (*sa_restorer)(void);
};
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
{% endhighlight %}

对于 `sigaction()` 函数，如果 `act` 非空，则会根据 `act` 结构体中的信号处理函数来修改该信号的处理动作；如果 `oldact` 非空则会通过该变量将信号原来的处理动作返回。

其中，`sa_handler` 变量用于指定信号的处理函数，有三种方式：

1. SIG_IGN 忽略信号；
2. SIG_DFL 执行系统默认动作；
3. 赋值为函数指针表示用自定义函数捕捉信号。

当某个信号的处理函数被调用时，内核自动将当前信号加入进程的信号屏蔽字，当信号处理函数返回时自动恢复原来的信号屏蔽字，这样就保证了在处理某个信号时，如果这种信号再次产生，那么它会被阻塞到当前处理结束为止。


## 常用程序

### Pause

该函数用于挂起进程，直到有信号触发，其声明如下：

{% highlight c %}
#include <unistd.h>
int pause(void);
{% endhighlight %}

针对不同的处理行为，其表现不同：A) 终止进程(一般默认)，则进程终止，`pause()` 没有机会返回；B) 忽略，进程继续处于挂起状态，`pause()` 不返回；C) 有信号处理函数，则调用了信号处理函数之后 `pause()` 返回 -1，其 `errno` 设置为 `EINTR` 。

{% highlight c %}
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sig_alarm(int signum)
{
	printf("I am a custom handler! signal number is %d\n", signum);
}

int main(void)
{
	struct sigaction sig, old;

	/* register a alarm signal handler */
	sig.sa_handler = sig_alarm;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGALRM, &sig, &old);

	printf("A SIGALRM handler has registered\n");
	alarm(3);
	pause();

	printf("Raise another alarm signal, in 1 second later\n");
	alarm(2);

	printf("Try to sleep 10 seconds\n");
	sleep(10);
	alarm(0); /* Cancel the alarm */

	sigaction(SIGALRM, &old, NULL); /* restore */
	alarm(1);
	sleep(2);

	return 0;
}
{% endhighlight %}

如上代码，其处理过程为：

1. 注册一个 `SIGALRM` 信号的处理函数，并在 3s 后内核向该进程发送一个 `SIGALRM` 信号；
2. 调用 `pause()` 将进程挂起，内核切换到别的进程运行；
3. 内核向该进程发送 `SIGALRM` 信号，发现其处理动作是一个自定义函数，于是切回用户态执行该自定义处理函数；
4. 进入处理函数后会自动屏蔽 `SIGALRM` 信号，返回后自动解除屏蔽。

注意，`SIGALRM` 处理会自动取消 `sleep` 的阻塞。

### SIGKILL VS. SIGSTOP

这两个信号比较特殊，无法在程序中进行屏蔽，用于一些特殊的用途。

#### SIGKILL

也就是直接的 `kill -9` 操作，为 root 提供了一种使进程强制终止方法，此时将会有操作系统直接回收该进程占用的资源，对于一些保存状态的应用就可能会导致异常。

#### SIGSTOP

对于前台运行的程序，可以通过 `Ctrl-Z` 终止程序，切换到后台，此时进程处于 `TASK_STOPPED` 状态，`ps` 命令显示处于 `T` 状态。如果要恢复运行，应该使用 `fg JOB-ID` 恢复运行，如果直接发送 `SIGCONT` 将会使进程退出。

可以参考 [WikiPedia SIGSTOP](http://en.wikipedia.org/wiki/SIGSTOP) 中的介绍，抄录如下：

{% highlight text %}
When SIGSTOP is sent to a process, the usual behaviour is to pause that process in its
current state. The process will only resume execution if it is sent the SIGCONT signal.
SIGSTOP and SIGCONT are used for job control in the Unix shell, among other purposes.
SIGSTOP cannot be caught or ignored.
{% endhighlight %}

也就是说，这个信号是用于 Shell 的任务管理，不能被用户屏蔽。其中常用的是 rsync 的同步任务，例如要清理一些空间，可以暂停运行，清理完成后重新启动运行。

{% highlight text %}
# kill -s STOP `pidof rsync`
# kill -s CONT `pidof rsync`
{% endhighlight %}



<!--
#define SIG_DFL ((void(*)(int))0)
#define SIG_IGN ((void(*)(int))1)


/* 打印那个进程发送的信号 */

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>


static int int_count = 0;
static int max_int = 5;
static int max_term = 10;

static void multi_handler(int sig, siginfo_t *siginfo, void *context)
{
	pid_t sender_pid = siginfo->si_pid;

	if(sig == SIGINT) {
		printf("INT(%d), from [%d]\n", int_count++, (int)sender_pid);
	} else if(sig == SIGQUIT) {
		printf("Quit, bye, from [%d]\n", (int)sender_pid);
		exit(0);
	} else if(sig == SIGTERM) {
		printf("TERM(%d), from [%d]\n", int_count++, (int)sender_pid);
	}

	return;
}


int main(void)
{
	printf("process [%d] started.\n", (int)getpid());

	static struct sigaction siga;
	siga.sa_sigaction = *multi_handler;
	siga.sa_flags |= SA_SIGINFO; /* get detail info */

	/* Ctrl + C */
	if (sigaction(SIGINT, &siga, NULL) != 0) {
		printf("error sigaction()");
		return errno;
	}

	/* Ctrl + \ */
	if (sigaction(SIGQUIT, &siga, NULL) != 0) {
		printf("error sigaction()");
		return errno;
	}

	if (sigaction(SIGTERM, &siga, NULL) != 0) {
		printf("error sigaction()");
		return errno;
	}


	int sig;
	while (1) {
		if (int_count < max_int) {
			sig = SIGINT;
		} else if (int_count >= max_int && int_count < max_term) {
			sig = SIGTERM;
		} else {
			sig = SIGQUIT;
		}

		raise(sig); /* send signal to itself */
		sleep(1);    // sleep a while, note that: SIGINT will interrupt this, and make program wake up,
	}

	return 0;
}




-->



















<!--
EINTR

## EAGAIN

实际上，在 Linux 中 EAGAIN 和 EWOULDBLOCK 相同。

以非阻塞方式打开文件或者 Sokcet 时，如果连续调用 read() 函数，而当没有数据可读时，又不会阻塞等待数据，那么此时就会返回 EAGAIN 错误，用来提示应用程序现在没有数据可读请稍后再试。

其它的，如当一个系统调用因为没有足够的资源而执行失败，返回 EAGAIN 提示其再调用一次，也许下次就能成功；例如 fork() 调用时可能内存不足。

## EINTR

当阻塞于某个系统调用时，如果进程捕获了某个信号，且相应信号处理函数返回后，该系统调用可能会返回一个 EINTR 错误。例如，在服务器端等待客户端链接，可能会获取到 EINTR 。

那么，为什么会有这个 EINTR 信号，如果出现了如何处理。首先，通常的处理方式是再次调用被中断的函数，而为什么有，还比较复杂一些。

假设有如下程序，主循环一直在读取数据，然后注册了一个中断函数用于标识退出程序，同时在退出前执行一些清理操作。

volatile int stop = 0;
void handler(int)
{
    stop++;
}

void event_loop (int sock)
{
    signal (SIGINT, handler);

    while (1) {
        if (stop) {
            printf ("do cleanup\n");
            return;
        }
        char buf [1];
        int rc = recv (sock, buf, 1, 0);
        if (rc == -1 && errno == EINTR)
            continue;
        printf ("perform an action\n");
    }
}

如上，当程序阻塞到 recv() 时，如果收到了 Ctrl-C 信号，那么在处理完之后实际上还会阻塞到 recv() 从而形成了死锁，如要要程序退出只能在接收到数据后进行处理，显然我们无法判断到底什么时候可能会收到数据。

通过返回的 EINTR 错误，让我们可以有机会进行处理，也就是如上的代码。

http://blog.csdn.net/hs794502825/article/details/52577622

http://blog.csdn.net/hzhsan/article/details/23650697
http://youbingchenyoubing.leanote.com/post/%E8%87%AA%E5%B7%B1%E8%B6%9F%E8%BF%87epoll%E7%9A%84%E5%9D%91
-->

{% highlight text %}
{% endhighlight %}
