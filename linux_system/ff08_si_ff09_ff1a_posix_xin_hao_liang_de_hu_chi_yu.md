# （四）：posix信号量的互斥与同步


在前面讲共享内存的IPC时曾说共享内存本身不具备同步机制，如果要实现同步需要使用信号量等手段来实现之，现在我们就来说说使用posix的信号量来实现posix多进程共享内存的同步。其实信号量也可以使用在同一进程的不同线程之间。

信号量是一个包含非负整数的变量，有两个原子操作，wait和post，也可以说是P操作和V操作，P操作将信号量减一，V操作将信号量加一。如果信号量大于0，P操作直接减一，否则如果等于0，P操作将调用进程（线程）阻塞起来，直到另外进程（线程）执行V操作。

信号量可以理解为一种资源的数量，通过这种资源的分配来实现同步或者互斥。当信号量的值为1时，可以实现互斥的功能，此时信号量就是二值信号量，如果信号量的值大于一时，可以实现进程（线程）并发执行。信号量和互斥锁条件变量之间的区别是：互斥锁必须由给它上锁的进程（线程）来解锁，而信号灯P操作不必由执行过它V操作的进程（线程）来执行；互斥锁类似于二值信号量，要么加锁，要么解锁；当向条件变量发信号时，如果此时没有等待在该条件变量上的线程，信号将丢失，而信号量不会。信号量主要用于之间同步，但也可以用在线程之间。互斥锁和条件变量主要用于线程同步，但也可以用于进程间的同步。

POSIX信号量是一个sem_t的变量类型，分有名信号量和无名信号量，无名信号量用在共享信号量内存的情况下，比如同一个进程的不同线程之间，或父子进程之间，有名信号量随内核持续的，所以我们可以跨多个程序操作它们。


### 1.  函数sem_open创建一个有名信号量或打开一个已存在信号量。函数原型如下：

```c
#include <fcntl.h> /* For O_* constants */  
#include <sys/stat.h> /* For mode constants */  
#include <semaphore.h>  
  
sem_t *sem_open(const char *name, int oflag);  
sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value);  
Link with -pthread  
```

参数name为posix IPC 名称， 参数oflag可以为0， O_CREAT 或O_CREAT | O_EXCL， 如果指定了O_CREAT，第三、四参数需要指定。mode为指定权限位，value参数指定信号量初始值，二值信号量的值通常为1，计数信号量的值通常大于1。
函数返回执行sem_t数据类型的指针，出错返回SEM_FAILED。


###2. 函数sem_close关闭有名信号量。

```c
#include <semaphore.h>  
int sem_close(sem_t *sem);  
Link with -pthread.
```

### 3. 函数sem_unlink删除有名信号量。

```c
#include <semaphore.h>  
int sem_unlink(const char *name);  
Link with -pthread.
```

###4.函数sem_wait和函数sem_trywait测试信号量的值，如果信号量值大于0，函数将信号量的值减一并立即返回，如果信号量的值等于0，sem_wait函数开始睡眠，直到信号量的值大于0，这是再减一并立即返回，而sem_trywait函数不睡眠直接返回，并返回EAGAIN错误。函数原型为：


```c
#include <semaphore.h>  
int sem_wait(sem_t *sem);  
int sem_trywait(sem_t *sem);  
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);  
Link with -pthread.  
```


### 5. 函数sem_post执行和sem_wait相反的操作。当一进程（线程）执行完操作之后，应该调用sem_post，将指定信号量加一，然后唤醒正在等待该信号量变为1的进程（线程）。函数原型：

```c
#include <semaphore.h>  
int sem_post(sem_t *sem);  
Link with -pthread.  
```

### 6. 函数sem_getvalue获取指定信号量的当前值，如果当前信号量已上锁，返回0，或者为负数，其绝对值为等待该信号量的解锁的线程数。

```c
#include <semaphore.h>  
int sem_getvalue(sem_t *sem, int *sval);  
Link with -pthread.  
```

###   7. 无名信号量使用sem_init()初始化，函数原型为：


```c
#include <semaphore.h>  
int sem_init(sem_t *sem, int pshared, unsigned int value);  
Link with -pthread.  
```

sem为无名信号量地址，value为信号量初始值，pshared指定信号量是在进程的各个线程之间共享还是在进程之间共享，如果该值为0，表示在进程的线程之间共享，如果非0则在进出之间共享，无名信号量主要用在有共同祖先的进程或线程之间。

这一节使用posix共享内存来实现各个进程之间的通信，并使用posix信号量来达到互斥与同步的目的。首先我们来看看使用信号量实现对共享内存段的互斥访问。

之前线程的访问使用如下方式来达到对公共的内存互斥访问：


```c
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;  
......  
pthread_mutex_lock(&mutex);//加锁  
......  
/*share memory handle*/  
......  
pthread_mutex_unlock(&mutex);//解锁  
......  
```

现在我们也使用类似方式来实现：

```c
sem_t *sem_mutex = NULL;  
......  
SLN_MUTEX_SHM_LOCK(SEM_MUTEX_FILE, sem_mutex);//加锁  
......  
/*share memory handle*/  
 ......  
SLN_MUTEX_SHM_UNLOCK(sem_mutex);//解锁  
...... 
```
其中SEM_MUTEX_FILE为sem_open函数需要的有名信号量名称。

其中两个加锁解锁的实现为：

```c
#define SLN_MUTEX_SHM_LOCK(shmfile, sem_mutex) do {\  
    sem_mutex = sem_open(shmfile, O_RDWR | O_CREAT, 0666, 1);\  
    if (SEM_FAILED == sem_mutex) {\  
        printf("sem_open(%d): %s\n", __LINE__, strerror(errno));\  
    }\  
    sem_wait(sem_mutex);\  
}while(0)  
  
#define SLN_MUTEX_SHM_UNLOCK(sem_mutex) do {sem_post(sem_mutex);} while(0)  
```

其实就是初始化一个二值信号量，其初始值为1，并执行wait操作，使信号量的值变为0，此时其它进程想要操作共享内存时也需要执行wait操作，但此时信号量的值为0，所以开始等待信号量的值变为1。当当前进程操作完共享内存后，开始解锁，执行post操作将信号量的值加一，此时其它进程的wait可以返回了。

下面为一个互斥访问共享内存的示例，posix共享内存实现请查看前面IPC的系列文章。
ser process：

```c
int nms_shm_get(char* shm_file, void** shm, int mem_len)
{
    int fd;

    fd = shm_open(shm_file, O_RDWR | O_CREAT, 0666);

    if (fd < 0) {
        printf("shm_pen <%s> failed: %s\n", shm_file, strerror(errno));
        return -1;
    }

    ftruncate(fd, mem_len);

    *shm = mmap(NULL, mem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (MAP_FAILED == *shm) {
        printf("mmap: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}
int main(int argc, const char* argv[])
{
    sem_t* sem_mutex = NULL;
    char* str = NULL;

    SLN_MUTEX_SHM_LOCK(SEM_MUTEX_FILE, sem_mutex); //加锁

    nms_shm_get(SHM_FILE, (void**)&str,
                SHM_MAX_LEN);  //下面三行互斥访问共享内存
    sleep(6);
    snprintf(str, SHM_MAX_LEN, "posix semphore server!");

    SLN_MUTEX_SHM_UNLOCK(sem_mutex); //解锁

    sleep(6);

    shm_unlink(SHM_FILE);

    return 0;
}
```

client process:

```c
int main(int argc, const char *argv[])  
{  
    sem_t *sem_mutex;  
    char *str = NULL;  
    SLN_MUTEX_SHM_LOCK(SEM_MUTEX_FILE, sem_mutex);  
    nms_shm_get(SHM_FILE, (void **)&str, SHM_MAX_LEN);  
    printf("client get: %s\n", str);  
    SLN_MUTEX_SHM_UNLOCK(sem_mutex);  
    return 0;  
}  
```

先启动服务进程首先加锁，创建共享内存并操作它，加锁中sleep 6秒，以便测试客户进程是否在服务进程未释放锁时处于等待状态。客户进程在服务进程启动之后马上启动，此时处于等待状态，当服务进程6秒之后解锁，客户进程获得共享内存信息。再过6秒之后，服务进程删除共享内存，客户进程再此获取共享内存失败。


```sh
# ./server &   
[1] 21690   
# ./client   
client get: posix semphore server!   
# ./client  
shm_open <share_memory_file> failed: No such file or directory  
client get: (null)  
[1]+ Done ./server  
```

posix有名信号量创建的信号量文件和共享内存文件在/dev/shm/目录下：

```sh
# ls /dev/shm/   
 sem.sem_mutex share_memory_file  
 ```
 
 在两个进程共享数据时，当一个进程向共享内存写入了数据后需要通知另外的进程，这就需要两个进程之间实现同步，这里我们给上面的程序在互斥的基础上加上同步操作。同步也是使用posix信号量来实现。
server process：


```c
int main(int argc, const char* argv[])
{
    sem_t* sem_mutex = NULL;
    sem_t* sem_consumer = NULL, *sem_productor = NULL;
    int semval;
    char* sharememory = NULL;

    sem_consumer = sem_open(SEM_CONSUMER_FILE, O_CREAT, 0666,
                            0); //初始化信号量sem_consumer ，并设置初始值为0

    if (SEM_FAILED == sem_consumer) {
        printf("sem_open <%s>: %s\n", SEM_CONSUMER_FILE, strerror(errno));
        return -1;
    }

    sem_productor = sem_open(SEM_PRODUCTOR_FILE, O_CREAT, 0666,
                             0);//初始化信号量sem_productor ，并设置初始值为0

    if (SEM_FAILED == sem_productor) {
        printf("sem_open <%s>: %s\n", SEM_PRODUCTOR_FILE, strerror(errno));
        return -1;
    }

    for (;;) {//服务进程一直循环处理客户进程请求
        sem_getvalue(sem_consumer, &semval);
        printf("%d waiting...\n", semval);

        if (sem_wait(sem_consumer) <
            0) {//如果sem_consumer为0，则阻塞在此，等待客户进程post操作使sem_consumer大于0，此处和客户进程同步
            printf("sem_wait: %s\n", strerror(errno));
            return -1;
        }

        printf("Get request...\n");

        SLN_MUTEX_SHM_LOCK(SEM_MUTEX,
                           sem_mutex);//此处开始互斥访问共享内存
        nms_shm_get(SHM_FILE, (void**)&sharememory, SHM_MAX_LEN);
        sleep(6);
        snprintf(sharememory, SHM_MAX_LEN, "Hello, this is server's message!");
        SLN_MUTEX_SHM_UNLOCK(sem_mutex);

        sem_post(sem_productor);//使信号量sem_productor加一，使阻塞的客户进程继续执行
        printf("Response request...\n");
    }

    sem_close(sem_consumer);
    sem_close(sem_productor);
    return 0;
}
```
client process：


```c
int main(int argc, const char* argv[])
{
    sem_t* sem_consumer = NULL, *sem_productor = NULL;
    struct timespec timeout;
    int ret;
    char* sharememory = NULL;
    sem_t* sem_mutex;
    sem_consumer = sem_open(SEM_CONSUMER_FILE,
                            O_RDWR);//获取信号量sem_consumer的值

    if (SEM_FAILED == sem_consumer) {
        printf("sem_open <%s>: %s\n", SEM_CONSUMER_FILE, strerror(errno));
        return -1;
    }

    sem_productor = sem_open(SEM_PRODUCTOR_FILE,
                             O_RDWR);//获取信号量sem_productor 的值

    if (SEM_FAILED == sem_productor) {
        printf("sem_open <%s>: %s\n", SEM_PRODUCTOR_FILE, strerror(errno));
        return -1;
    }

    //clear_exist_sem(sem_productor);

    SLN_MUTEX_SHM_LOCK(SEM_MUTEX, sem_mutex);//互斥访问共享内存
    nms_shm_get(SHM_FILE, (void**)&sharememory, SHM_MAX_LEN);
    printf("sharememory: %s\n", sharememory);
    SLN_MUTEX_SHM_UNLOCK(sem_mutex);

    sem_post(sem_consumer);//信号量sem_consumer加一，唤醒是阻塞在该信号量上的服务进程
    printf("Post...\n");
    sem_wait(sem_productor);//等待服务进程回应
    /*
     timeout.tv_sec = time(NULL) + SEM_TIMEOUT_SEC;
     timeout.tv_nsec = 0;
     ret = sem_timedwait(sem_productor, &timeout);
     if (ret < 0) {
         printf("sem_timedwait: %s\n", strerror(errno));
     }
    */

    printf("Get response...\n");
    sem_close(sem_consumer);
    sem_close(sem_productor);

    return 0;
}
```

http://download.csdn.net/detail/gentleliu/8292189
