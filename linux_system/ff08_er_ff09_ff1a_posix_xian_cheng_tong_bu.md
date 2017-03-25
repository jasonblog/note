# （二）：posix线程同步


上一节说到线程的互斥锁，互斥锁适合防止访问某个共享变量，这一节我们来看看两个线程如何实现同步。互斥锁也可以实现线程同步，当该值满足某种条件时当前线程继续执行，否则继续轮询，不过这样相当浪费cpu时间。我们需要的是让某个线程进入睡眠，当满足该线程执行条件时，另外线程主动通知它，这就是这一节我们要说的条件变量，它常和互斥锁一起使用。

如同信号量，线程可以对一个条件变量执行等待操作。如果如果线程 A 正在等待一个条件变量，它会被阻塞直到另外一个线程B，向同一个条件变量发送信号以改变其状态。不同于信号量，条件变量没有计数值，也不占据内存空间，线程 A 必须在 B 发送信号之前开始等待。如果 B 在 A 执行等待操作之前发送了信号，这个信号就丢失了，同时 A会一直阻塞直到其它线程再次发送信号到这个条件变量。

条件变量将允许你实现这样的目的：在一种情况下令线程继续运行，而相反情况下令线程阻塞。只要每个可能涉及到改变状态的线程正确使用条件变量，Linux 将保证当条件改变的时候由于一个条件变量的状态被阻塞的线程均能够被激活。

GNU/Linux 刚好提供了这个机制，每个条件变量都必须与一个互斥体共同使用，以防止这种竞争状态的发生。这种设计下，线程函数应遵循以下步骤： 

thread_function首先锁定互斥体并且读取标志变量的值。 
如果标志变量已经被设定，该线程将互斥体解锁然后执行工作函数
如果标志没有被设置，该线程自动锁定互斥体并开始等待条件变量的信号

这里最关键的特点就在第三条。这里，GNU/Linux系统允许你用一个原子操作完成解除互斥体锁定和等待条件变量信号的过程而不会被其它线程在中途插入执行。这就避免了在thread_function中检测标志和等待条件变量的过程中其它线程修改标志变量并对条件变量发送信号的可能性。

互斥锁是类型为pthread_mutex_t的变量，和互斥锁类似，初始化也两种方法：
一种是静态初始化，给锁变量赋值PTHREAD_COND_INITIALIZER，

```c
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;  
```

一种动态初始化，使用函数pthread_cond_init，

```c
int pthread_cond_init(pthread_cond_t *restrict cond,  const pthread_condattr_t *restrict attr);
```

其中第一个参数是一个指向pthread_cond_t变量的指针。第二个参数是一个指向条件变量属性对象的指针；这个参数在 GNU/Linux 系统中是被忽略的。

函数pthread_cond_wait（）使线程阻塞在一个条件变量上。函数原型如下：

```c
#include <pthread.h>  
int pthread_cond_wait(pthread_cond_t *restrict cond,  pthread_mutex_t *restrict mutex);  
```


第一个参数是指向一个 pthread_cond_t 类型变量的指针，第二个参数是指向一个pthread_mutex_t类型变量的指针。当调用 pthread_cond_wait 的时候，互斥体对象必须已经被调用线程锁定。这个函数以一个原子操作解锁互斥体并锁定条件变量等待信号。当信号到达且调用线程被解锁之后，pthread_cond_wait自动申请锁定互斥体对象。

函数传入的参数mutex用于保护条件，因为我们在调用pthread_cond_wait时，如果条件不成立我们就进入阻塞，但是进入阻塞这个期间，如果条件变量改变了的话，那我们就漏掉了这个条件。因为这个线程还没有放到等待队列上，所以调用pthread_cond_wait前要先锁互斥量，即调用pthread_mutex_lock(),pthread_cond_wait在把线程放进阻塞队列后，自动对mutex进行解锁，使得其它线程可以获得加锁的权利。这样其它线程才能对临界资源进行访问并在适当的时候唤醒这个阻塞的进程。当pthread_cond_wait返回的时候又自动给mutex加锁。

函数pthread_cond_signal（）用来释放被阻塞在条件变量cond上的一个线程。多个线程阻塞在此条件变量上时，哪一个线程被唤醒是由线程的调度策略所决定的。要注意的是，必须用保护条件变量的互斥锁来保护这个函数，否则条件满足信号又可能在测试条件和调用pthread_cond_wait函数之间被发出，从而造成无限制的等待。函数原型如下：


```c
#include <pthread.h>  
int pthread_cond_signal(pthread_cond_t *cond);  
```

下面给一个示例：

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int gval = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
void* func1_th(void* arg)
{
    pthread_mutex_lock(&mutex);

    if (5 != gval) {
        printf("=======%s->%d===thread 1 wait start!====\n", __func__, __LINE__);
        pthread_cond_wait(&cond,
                          &mutex); //当条件不满足时，此时线程1会自动对mutex进行解锁，使得其它线程可以获得加锁的权利，当收到苏醒时会在执行加锁操作
    }

    /**
     * do something here
     */
    printf("=======%s->%d====thread 1 do something!===\n", __func__, __LINE__);
    pthread_mutex_unlock(&mutex);
    return NULL;
}
void* func2_th(void* arg)
{
    int i;
    pthread_mutex_lock(&mutex);

    for (i = 0; i < 10; i++) {
        printf("=======%s->%d=======\n", __func__, __LINE__);
        sleep(1);
        gval++;

        if (5 == gval) {
            pthread_cond_signal(
                &cond); //当条件满足时，线程2发送信号，线程1收到信号会解除阻塞状态。
            printf("=======%s->%d===signal====\n", __func__, __LINE__);
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}
int main(int argc, const char* argv[])
{
    pthread_t tid1, tid2;

    if (0 != pthread_create(&tid1, NULL, func1_th, NULL)) {
        printf("pthread_create failed!\n");
        return -1;
    }

    sleep(1);     //让线程1先执行，进入等待状态

    if (0 != pthread_create(&tid2, NULL, func2_th, NULL)) {
        printf("pthread_create failed!\n");
        return -1;
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;
}
```
另一个用来阻塞线程的函数是pthread_cond_timedwait（）。函数原型如下：

```c
#include <pthread.h>  
int pthread_cond_timedwait(pthread_cond_t *restrict cond,  pthread_mutex_t *restrict mutex, const struct timespec *restrict abstime);   
```

pthread_cond_timedwait 和pthread_cond_wait 一样，自动解锁互斥量及等待条件变量， 但它还限定了等待时间。如果在 abstime 指定的时间内 cond 未触发，互斥量 mutex 被重 新加锁，并返回错误 ETIMEDOUT 。


。函数pthread_cond_broadcast（pthread_cond_t *cond）用来唤醒所有被阻塞在条件变量cond上的线程。这些线程被唤醒后将再次竞争相应的互斥锁，所以必须小心使用这个函数。函数原型如下：


```c
#include <pthread.h>  
int pthread_cond_broadcast(pthread_cond_t *cond);  
```

http://download.csdn.net/detail/gentleliu/8280393

