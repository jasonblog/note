# （一）：posix线程及线程间互斥


有了进程的概念，为何还要使用线程呢？

首先，回忆一下上一个系列我们讲到的IPC，各个进程之间具有独立的内存空间，要进行数据的传递只能通过通信的方式进行，这种方式不仅费时，而且很不方便。而同一个进程下的线程是共享全局内存的，所以一个线程的数据可以在另一个线程中直接使用，及快捷又方便。

其次，在Linux系统下，启动一个新的进程必须分配给它独立的地址空间，建立众多的数据表来维护它的代码段、堆栈段和数据段，这是一种"昂贵"的多任务工作方式。而运行于一个进程中的多个线程，它们彼此之间使用相同的地址空间，共享大部分数据，启动一个线程所花费的空间远远小于启动一个进程所花费的空间，而且，线程间彼此切换所需的时间也远远小于进程间切换所需要的时间。

但是，伴随着这些优点，线程却带来了同步与互斥的问题。下面先讲讲线程基本函数：

###1. 线程的创建pthread_create

```c
#include <pthread.h>
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
              void *(*start_routine) (void *), void *arg);
```

 一个线程由一个线程ID（参数thread）标识，新的线程创建成功，其值通过指针thread返回。

参数attr为线程属性（比如：优先级、初始栈大小等），通常我们使用默认设置，设为NULL。

参数start_routine为一个函数指针，指向线程执行的函数，最后参数arg为函数start_routine唯一参数，如果需要传递多个参数，需要打包为结构，然后将其地址传给该函数。

pthread_create成功时返回0，失败为非0值，这和其他linux系统调用的习惯不一样。

###2. pthread_join函数

```c
#include <pthread.h>  
int pthread_join(pthread_t thread, void **retval);  
```


通过调用该函数等待一个给定线程终止，类似于线程的waitpid函数。

该函数等待参数thread指定的线程终止，该函数会阻塞，直到线程thread终止，将线程返回的(void *)指针赋值为retval指向的位置，然后回收已经终止线程占用的所有存储器资源。


###3. pthread_self函数

```c
#include <pthread.h>  
pthread_t pthread_self(void);
```

该函数用于获取线程自身线程ID。类似于进程的getpid函数。


###4. pthread_detach函数

```c
#include <pthread.h>  
int pthread_detach(pthread_t thread);  
```

 该函数可分离可结合线程，线程可以通过以pthread_self()为参数的pthread_detach调用来分离他们自己。

一个分离线程是不能被其他线程回收或杀死的，他的存储器资源在他终止时由系统自动释放。一个可结合线程能够被其他线程收回其资源和杀死，在被其他线程收回之前，他的存储器资源是没有被释放的。在任何一个时间点上，线程是可结合的或者是可分离的。默认情况下，线程是被创建成可结合的。


为了避免存储器泄露，每个可结合线程都应该要么被其他线程现实的收回，要么通过调用pthread_detach函数被分离。


在现实的程序中，我们一般都使用分离的线程。

### 5. pthread_exit函数

```c
#include <pthread.h>  
void pthread_exit(void *retval);
```

 该函数作用就是终止线程。如果该线程未曾分离，他的线程ID和退出状态将一直保留到调用进程内某个其他线程对他调用pthread_join。

另外，当线程函数（pthread_create第三个参数）返回时，该线程将终止；当创建该线程的进程main函数返回时，该线程也将终止。

下面给一个简单的示例
 
```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* func_th(void* arg)
{
    unsigned int*     val = (unsigned int*)arg;

    printf("=======%s->%d==thread%d: %u====\n", __func__, __LINE__,
           *val, (unsigned int)pthread_self());
    return NULL;
}

int main(int argc, const char* argv[])
{
    pthread_t   tid1, tid2;
    int         a, b;

    a = 1;

    if (0 != pthread_create(&tid1, NULL, func_th, &a)) {
        printf("pthread_create failed!\n");
        return -1;
    }

    b = 2;

    if (0 != pthread_create(&tid2, NULL, func_th, &b)) {
        printf("pthread_create failed!\n");
        return -1;
    }


    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;
}
```

执行2次输出为：
 
```sh
# ./target_bin  
=======func_th->9==thread1: 3077856064====  
=======func_th->9==thread2: 3069463360====  
# ./target_bin  
=======func_th->9==thread2: 3069315904====  
=======func_th->9==thread1: 3077708608====  
```

 类似于进程，线程的调度随机的。


在前面开始我们说到同一个进程内的线程是共享全局内存的，那么当多个线程同时去修改一个全局变量的时候就会出问题，如果一个线程在修改某个变量时中途被挂起，操作系统去调度另外一个线程执行，那就可能导致错误。我们无法保证操作系统对这些操作都是原子的。

在我们在现在的例子中这样去复现这种问题：一个线程对一个全局变量（100）进行读-加1-读操作，另个变量对该全局进行减1操作，我们通过sleep来实现加线程先执行，减线程在加线程的加和读之间进行，最后来查看加操作是否是我们期望的结果（101）。例子如下：

（该示例只是为了强化运行时出错，并对这种错误有一个宏观的了解而写）

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int gval = 100;

void* func_add_th(void* arg)
{
    int*     val = (int*)arg;

    printf("==do %s==thread%d: %u====\n", __func__,
           *val, (unsigned int)pthread_self());

    printf("before add 1, gval=%d\n", gval);
    gval += 1;

    sleep(4);//此时add线程挂起，sub线程执行键操作

    printf("after add 1, gval=%d\n", gval);
    return NULL;
}

void* func_sub_th(void* arg)
{
    int*     val = (int*)arg;

    printf("==do %s==thread%d: %u====\n", __func__,
           *val, (unsigned int)pthread_self());

    gval -= 1;

    return NULL;
}

int main(int argc, const char* argv[])
{
    pthread_t   tid1, tid2;
    int         a, b;

    a = 1;

    if (0 != pthread_create(&tid1, NULL, func_add_th, &a)) {
        printf("pthread_create failed!\n");
        return -1;
    }

    sleep(1);  //保证add线程先被调度
    b = 2;

    if (0 != pthread_create(&tid2, NULL, func_sub_th, &b)) {
        printf("pthread_create failed!\n");
        return -1;
    }


    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;
}
```
执行结果如下：


```sh
# ./target_bin  
==do func_add_th==thread1: 3078355776====  
before add 1, gval=100  
==do func_sub_th==thread2: 3069963072====  
after add 1, gval=100  
```

通过输出我们可以看到sub操作在加1和读之间操作，最终读取出来的值仍然是100，不是我们期望的101。

这就是两个线程不是互斥带来的结果，所以我们希望在某某一线程一段代码执行期间，只有一个线程在运行，当运行完成之后，下一个线程运行该部分代码，所以我们需要将该部分代码加锁。这就是线程编程，也是并发编程需要考虑的问题。


解决多线程共享的问题就是使用互斥锁（mutex，即mutual exliusion）来保护共享数据。在执行某一段代码是首先要持有该互斥锁，执行完成之后再释放该锁。互斥锁是类型为pthread_mutex_t的变量。使用如下方法来加锁和解锁操作。


派生到我的代码片

```c
#include <pthread.h>  
int pthread_mutex_lock(pthread_mutex_t *mutex);  
int pthread_mutex_trylock(pthread_mutex_t *mutex);  
int pthread_mutex_unlock(pthread_mutex_t *mutex);  
```

首先我们需要初始化锁，初始化方法有两种，一种是静态初始化，给锁变量赋值PTHREAD_MUTEX_INITIALIZER，一种动态初始化，使用函数pthread_mutex_init。

我们使用静态方法初始化：

```c
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;
```

 当试图使用pthread_mutex_lock（）获得一个已经被另外线程加锁的锁时，本线程将阻塞，直到互斥锁被解锁为止。函数pthread_mutex_trylock为获取锁的非阻塞版本，当获取失败时会立即返回。

我们修改add和sub线程函数分别如下：

```c
void* func_add_th(void* arg)
{
    int*     val = (int*)arg;

    pthread_mutex_lock(&mutex);//此处加锁
    printf("==do %s==thread%d: %u====\n", __func__,
           *val, (unsigned int)pthread_self());

    printf("before add 1, gval=%d\n", gval);
    gval += 1;

    sleep(4);

    printf("after add 1, gval=%d\n", gval);
    pthread_mutex_unlock(&mutex);//此处释放锁
    return NULL;
}
void* func_sub_th(void* arg)
{
    int*     val = (int*)arg;

    pthread_mutex_lock(&mutex);
    printf("==do %s==thread%d: %u====\n", __func__,
           *val, (unsigned int)pthread_self());

    gval -= 1;
    pthread_mutex_unlock(&mutex);

    return NULL;
}
```

执行结果为：

```sh
# ./target_bin  
==do func_add_th==thread1: 3077614400====  
before add 1, gval=100  
after add 1, gval=101  
==do func_sub_th==thread2: 3069221696====  
```

 通过结果输出可以看到，sub操作是在add操作执行完成之后才执行的，而add线程输出结果也是我们预期的，所以我们的加锁是成功的。但是如果add线程要执行很久的话，sub线程就要阻塞很久，我们可以将sub线程加锁函数改为非阻塞版本，当加锁失败时，立即返回。

修改后的sub线程函数：

```c
void* func_sub_th(void* arg)
{
    int*     val = (int*)arg;

    if (0 != pthread_mutex_trylock(&mutex)) {
        printf("failed to lock!\n");
        return NULL;
    }

    printf("==do %s==thread%d: %u====\n", __func__,
           *val, (unsigned int)pthread_self());

    gval -= 1;
    pthread_mutex_unlock(&mutex);

    return NULL;
}
```
运行输出为：

```sh
# ./target_bin  
==do func_add_th==thread1: 3077638976====  
before add 1, gval=100  
failed to lock!  
after add 1, gval=101  
```

当多个线程同时需要多个相同锁时，可能会出现死锁的情况。比如两个线程同时需要互斥锁1和互斥锁2，线程a先获得锁1，线程b获得锁2，这是线程a、b分别还需要锁2和锁1，但此时两个锁都被加锁了，都阻塞在那里等待对方释放锁，这样死锁就出现了。我们来实现一下死锁的情况，将之前两个例子的线程函数修改如下：

```c
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     mutex_sec = PTHREAD_MUTEX_INITIALIZER;

void* func_add_th(void* arg)
{
    int*     val = (int*)arg;

    pthread_mutex_lock(&mutex);//1. add线程先加第一个锁mutex
    printf("==do %s==thread%d: %u====\n", __func__,
           *val, (unsigned int)pthread_self());
    sleep(2);//等待2秒，让sub线程加第二个锁mutex_sec
    pthread_mutex_lock(&mutex_sec);//4. add线程加锁mutex_sec失败

    printf("before add 1, gval=%d\n", gval);
    gval += 1;

    sleep(4);

    printf("after add 1, gval=%d\n", gval);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_unlock(&mutex_sec);
    return NULL;
}

void* func_sub_th(void* arg)
{
    int*     val = (int*)arg;

    pthread_mutex_lock(&mutex_sec);//2. Sub线程比add线程先加锁mutex_sec
    pthread_mutex_lock(&mutex);//3. Sub线程加锁mutex失败

    printf("==do %s==thread%d: %u====\n", __func__,
           *val, (unsigned int)pthread_self());

    gval -= 1;
    pthread_mutex_unlock(&mutex);
    pthread_mutex_unlock(&mutex_sec);

    return NULL;
}
```

上面两个线程按照函数注释中1-2-3-4顺序执行，运行时程序就卡在那里出现了死锁。

可以使用非阻塞版本的加锁函数来加锁，不过也要注意在第二个锁加锁不成功情况下，需要释放第一个锁再返回，不然其他线程仍然得不到第一个锁。有时在线程需要多个互斥锁时，让线程按照指定的同样顺序进行加锁也可以避免死锁。程序死锁出现时很难定位，所以程序猿在编程（尤其是在设计）时需要注意避免这个问题。


本节示例代码下载：

http://download.csdn.net/detail/gentleliu/8270863
 