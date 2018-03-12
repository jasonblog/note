---
title: Linux 线程同步
layout: post
comments: true
language: chinese
category: [program,linux]
keywords: linux,program,pthread
description: 线程的三个主要同步原语：互斥锁 (mutex)、信号量 (semaphore) 和条件变量 (cond)。其中 mutex 和 sem 都是对应 futex 进行了简单的封装，在不存在冲突的情况下就不用陷入到内核中进行仲裁；而且 pthread_join 也是借助 futex 来实现的。简单介绍下 Linux 中与线程相关的编程。
---

线程的三个主要同步原语：互斥锁 (mutex)、信号量 (semaphore) 和条件变量 (cond)。其中 mutex 和 sem 都是对应 futex 进行了简单的封装，在不存在冲突的情况下就不用陷入到内核中进行仲裁；而且 pthread_join 也是借助 futex 来实现的。

简单介绍下 Linux 中与线程相关的编程。

<!-- more -->

<!-- 而 cond 则是依靠 mutex 和信号来实现其语意，也就是说信号存在丢失的情况。-->

## 简介

Linux 线程在核内是以轻量级进程的形式存在的，拥有独立的进程表项，而所有的创建、同步、删除等操作都在核外的 `pthread` 库中进行。


## 锁

### 互斥锁


{% highlight text %}
----- 初始化一个互斥锁，可以通过PTHREAD_MUTEX_INITIALIZER宏进行初始化
int pthread_mutex_init(pthread_mutex_t* mutex, const thread_mutexattr_t* mutexattr);

----- 获取互斥锁，如果被锁定，当前线程处于等待状态；否则，本线程获得互斥锁并进入临界区
int pthread_mutex_lock(pthread_mutex_t* mutex);

----- 与lock操作不同的是，在尝试获得互斥锁失败后，不会进入阻塞状态，而是返回线程继续执行
int pthread_mutex_trylock(pthread_mutex_t* mutex);

----- 释放互斥锁，被阻塞的线程会获得互斥锁然后执行
int pthread_mutex_unlock(pthread_mutex_t* mutex);

----- 用来释放互斥锁的资源
int pthread_mutex_destroy(pthread_mutex_t* mutex);
{% endhighlight %}

使用方式如下。

{% highlight text %}
pthread_mutex_t mutex;
pthread_mutex_init(&mutex,NULL);

void pthread1(void* arg)
{
   pthread_mutex_lock(&mutex);
   ... ...  //临界区
   pthread_mutex_unlock(&mutex);
}

void pthread2(void* arg)
{
   pthread_mutex_lock(&mutex);
   ... ...  //临界区
   pthread_mutex_unlock(&mutex);
}
{% endhighlight %}

### 读写锁

读写锁与互斥量类似，不过读写锁允许更高的并行性，适用于读的次数大于写的次数的数据结构。一次只有一个线程可以占有写模式的读写锁，但是多个线程可以同时占有读模式的读写锁。

{% highlight text %}
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int pthread_rwlock_init(pthread_rwlock_t* rwlock, const pthread_rwlockattr_t* attr);

int pthread_rwlock_rdlock(pthread_rwlock_t* rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t* rwlock);

int pthread_rwlock_unlock(pthread_rwlock_t* rwlock);
int pthread_destroy(pthread_rwlock_t* rwlock);
{% endhighlight %}



### 自旋锁

互斥锁或者读写锁是靠阻塞线程的方式是使其进入休眠，自旋锁是让线程不断循判断自旋锁是否已经被解锁，而不会使其休眠，适用于占用自旋锁时间比较短的场景。

## 条件变量

信号量只有锁和不锁两种状态，当条件变量和信号量一起使用时，允许线程以无竞争的方式等待特定的条件发生。也就是说，条件本身是由互斥量保护，线程在改变条件状态之前必须先锁住互斥量。

{% highlight text %}
----- 动态初始化条件变量，也可以通过PTHREAD_COND_INITIALIZER宏进行初始化
int pthread_cond_init(pthread_cond_t* cond,const pthread_condattr_t* attr);

----- 阻塞等待条件变量为真，注意线程会被条件变量一直阻塞。
int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex);

----- 与wait类似，但是可以设置超时时间，在经历tspr时间后，即使条件变量不满足，阻塞也被解除
int pthread_cond_timedwait(pthread_cond_t* cond, pthread_mutex_t* mutex,const struct timespec* tspr);

----- 唤醒因为条件变量阻塞的线程，至少会唤醒一个等待队列上的线程
int pthread_cond_signal(pthread_cond_t* cond);

----- 唤醒等待该条件的所有线程
int pthread_cond_broadcast(pthread_cond_t* cond);

----- 销毁条件变量
int pthread_cond_destroy(pthread_cond_t* cond);
{% endhighlight %}

注意，如果有多个线程在等待信号的发生，那么在通过 `pthread_cond_signal()` 发送信号时，会根据调度策略选择等待队列中的一个线程。

### 示例

比较典型的应用是生产者和消费者模型。如下的示例中，两个线程共享 `x` 和 `y` 变量，条件变量用于表示 `x > y`  成立。

{% highlight text %}
int x, y;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

//----- 等待x>y成立后执行
pthread_mutex_lock(&mut);
while (x <= y) {
        pthread_cond_wait(&cond, &mut);
}
/* operate on x and y */
pthread_mutex_unlock(&mut);

//----- 修改x,y，可能会导致x>y，如果是那么就触发条件变量
pthread_mutex_lock(&mut);
/* modify x and y */
if (x > y)
        pthread_cond_broadcast(&cond);
pthread_mutex_unlock(&mut);
{% endhighlight %}

这个处理逻辑看起来还比较简单，但是为什么在被唤醒后还要进行条件判断，也就是为什么要使用 `while` 循环来判断条件。实际上更多的是为了跨平台的兼容，由于对于调度策略没有明确的规定，那么不同的实现就可能会导致 "惊群效应"。

例如，多个线程在等待信号量，同时被唤醒，那么只有一个线程会进入临界区进行处理，那么此时就必须要再次条件判断。

### 原理

首先重点解释下 `pthread_cond_wait()` 函数，该函数功能可以直接通过 `man 3p pthread_cond_wait` 查看；简单来说，其实现的功能是，释放 `mutex` 并阻塞到 `cond` ，更重要的是，这两步是 **原子操作** 。

在解释之前，为了防止与 NTPL 中的实现混淆，暂时去掉 `pthread` 头，仅为方便说明。

#### 1. 基本条件变量

{% highlight text %}
----- 如上，在等待事件时，通常会通过while()进行条件检测
while(pass == 0)
    cond_wait(...);
 
----- 当条件被改变时，通过signal()函数通知到其它线程
pass = 1;
cond_signal(...)
{% endhighlight %}

#### 2. 互斥量保护

注意，如上的两个线程中都涉及到了共享变量 `pass` ，那么为了防止竞态条件的发生，需要通过加锁进行保护，也就是如下的内容：

{% highlight text %}
mutex_lock(mtx);
while(pass == 0)
    cond_wait(...);
/* more operations on pass */
mutex_unlock(mtx);

mutex_lock(mtx);
pass = 1;
/* more operations on pass */
cond_signal(...);
mutex_unlock(mtx); // 必须要在cond_signal之后，否则同样存在竞态条件
{% endhighlight %}

#### 3. 条件变量语义

实际在上述的示例中，隐含了 `cond_wait()` 必须要自动释放锁，也就是说如果只是等待条件变量，实际的内容如下：

{% highlight text %}
mutex_lock(mtx);
while(pass == 0) {
 mutex_unlock(mtx);
 cond_just_wait(cv);
 mutex_lock(mtx);
}
mutex_unlock(mtx);

mutex_lock(mtx);
pass = 1;
cond_signal(...);
mutex_unlock(mtx);
{% endhighlight %}

<!--
久而久之，程序员发现unlock, just_wait, lock这三个操作始终得在一起。于是就提供了一个pthread_cond_wait()函数来同时完成这三个函数。另外一个证据是，signal()函数是不需要传递mutex参数的，所以关于mutex参数是用于同步wait()和signal()函数的说法更加站不住脚。所以我的结论是：传递的mutex并不是为了防止wait()函数内部的Race Condition！而是因为调用wait()之前你总是获得了某个mutex（例如用于解决此处pass变量的Race Condition的mutex），并且这个mutex在你调用wait()之前必须得释放掉，调用wait()之后必须得重新获取。所以，pthread_cond_wait()函数不是一个细粒度的函数，却是一个实用的函数。

### 竞态条件讨论

其中第一行和第二行的两个操作必须是 "原子化" 的操作，第三行可以分离出去。之所以要求第一、二行必须是原子操作，是因为要保证：如果线程 A 先进入 wait 函数 (可能还在释放mtx)，那么必须保证其它线程在其之后调用 broadcast 时能够将线程 A 唤醒。

对于上述的操作：

mutex_lock(mtx);        // a1
while(pass == 0) {      // a2
 mutex_unlock(mtx);  // a3
 cond_just_wait(cv); // a4
 mutex_lock(mtx);    // a5
}
mutex_unlock(mtx);

mutex_lock(mtx);    // b1
pass = 1;           // b2
cond_signal(...);   // b3
mutex_unlock(mtx);  // b4

如果执行序列是：a1, a2, a3, b1, b2, b3, b4, a4，那么线程 A 将不会被唤醒，也就是说 wait() 是在 signal() 之前调用的，所以不满足上文提到的保证。


先将线程附加到等待队列
释放mutex
进入等待

感兴趣的同学的可以看下源码（pthread_cond_wait.c），附加到等待队列这个操作是加锁的，所以可以保证之前发起的signal不会错误得唤醒本线程，而之后发起的signal必然唤醒本线程。因此，下面的代码是绝对不会出错的：// 线程A，条件测试
pthread_mutex_lock(mtx);        // a1
while(pass == 0) {              // a2 
    pthread_cond_wait(cv, mtx); // a3
}
pthread_mutex_unlock(mtx);      // a4

// 线程B，条件发生修改，对应的signal代码
pthread_mutex_lock(mtx);   // b1
pass = 1;                  // b2
pthread_mutex_unlock(mtx); // b3
pthread_cond_signal(cv);   // b4
如果线程A先运行，那么执行序列必然是：a1, a2, a3, b1, b2, b3, b4, a4。如果线程B先运行，那么执行序列可能是：b1, b2, b3, b4, a1, a2, a4也可能是：b1, b2, b3, a1, a2, a3, b4, a4

http://blog.sina.com.cn/s/blog_967817f20101bsf0.html
另外需要了解Memory Barrier和Automatic Operation，无锁编程
pthread_cleanup_push 
?????pthread_cancel 和取消点
-->


## 信号量


{% highlight text %}
----- 初始化信号量，pshared为0只能在当前进程的线程间共享，否则可以进程间共享，value给出了信号量的初始值
int sem_init(sem_t* sem, int pshared,unsigned int value);
----- 阻塞线程，直到信号量的值大于0，解除阻塞后同时会将sem的值减一
sem_wait(sem_t* sem);
----- 是wait的非阻塞版本
sem_trywait(sem_t* sem);
----- 增加信号量的值，会利用线程的调度策略选择一个已经被阻塞的线程
sem_post(sem_t* sem);
----- 释放信号量所占用的资源
sem_destroy(sem_t* sem);
{% endhighlight %}


<!--
pthread_mutex_t mutex;
sem_t full,empty;

void producer(void* arg){
    while(1){
    sem_wait(&empty);//need to produce. the the empty of resource need minus 1
    pthread_mutex_lock(&mutex);
    ...//produce a resource
    pthread_mutex_unlock(&mutex);
    sem_post(&full); //have produced a resource, the the full of resource need add 1
    }
}
void consumer(void* arg){
    while(1){
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    ...//consume a resource
    pthread_mutex_unlock(&mutex);
    sem_post(&empty); 
    }
}
-->

## 取消点

一个线程可以通过 `pthread_cancel()` 向另外一个线程发送结束请求，接收此请求的线程可以通过线程的两个属性来决定是否取消，以及是同步 (延时) 取消还是异步 (立即) 取消；而且即使该函数调用成功返回，也不代表线程就结束了。

可以通过如下的两个函数设置线程的属性：

{% highlight text %}
int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);

PTHREAD_CANCEL_ENABLE
  可取消，不过有两种取消方式，一种是同步取消，另外一种是异步取消。
PTHREAD_CANCEL_DISABLE
  不可取消，那么接收取消请求后，它依然继续执行。
PTHREAD_CANCEL_DEFERRED
  可取消，会在下一个取消点退出；简单来说，取消点就是程序在运行的时候检测是否收到取消请求，常见的有如下函数：
    pthread_join() pthread_cond_wait() pthread_cond_timedwait() pthread_testcancel() sem_wait() sigwait()
PTHREAD_CANCEL_ASYNCHRONOUS
  异步取消，也就是说线程在收到取消请求之后，立即取消退出。
{% endhighlight %}

默认情况下，一个线程是可取消的并且是同步取消的。对于可以取消的线程，需要考虑退出时该线程后续的资源清理，例如，线程执行到一个锁内时，已经获得锁了，这时异常退出了，那么此时就是一个死锁的问题。

此时需要使用如下两个函数：

{% highlight text %}
#include <pthread.h>
void pthread_cleanup_push(void (*routine)(void *), void *arg);
void pthread_cleanup_pop(int execute);
{% endhighlight %}

如果函数没有异常退出，那么可以通过 `phread_cleanup_pop()` 弹出这些栈里的函数，此时的参数要为 0，如果非 0 的话，弹出的同时也会执行这些函数的。例如，对于死锁问题可以做如下处理：

{% highlight text %}
pthread_cleanup_push(pthread_mutex_unlock, (void *) &mutex);
pthread_mutex_lock(&mutex);
/* do some work */
pthread_mutex_unlock(&mutex);
pthread_cleanup_pop(0);
{% endhighlight %}


<!--
## futex

Futex 可以简单理解为保存锁的状态以及一个等待该锁的任务等待队列。

### 源码解析

内核中的实现在 `kernel/futex.c` 文件中，

初始化时会创建一个 hash 表，
http://blog.csdn.net/npy_lp/article/details/7331245

当非竞态时，只需要在用户空间处理即可，只有在出现竞态时才会进入到内核空间中；而且针对非竞态进行了优化。

其中地址是整数对齐的，而且只能通过原子的汇编指令操作。

futex_init()
 |-alloc_large_system_hash()  分配名为futex的hash表
 |-futex_detect_cmpxchg()

-->

## 参考

<!--
http://www.cnblogs.com/zhaoyl/p/3620204.html
http://blog.csdn.net/zmxiangde_88/article/details/7997874

http://www.cnblogs.com/blueclue/archive/2010/06/11/1754899.html
https://yangwenbo.com/articles/linux-implementation-of-posix-thread-cancellation-points.html
-->


{% highlight text %}
{% endhighlight %}
