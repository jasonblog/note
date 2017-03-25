# （四）：posix信號量的互斥與同步


在前面講共享內存的IPC時曾說共享內存本身不具備同步機制，如果要實現同步需要使用信號量等手段來實現之，現在我們就來說說使用posix的信號量來實現posix多進程共享內存的同步。其實信號量也可以使用在同一進程的不同線程之間。

信號量是一個包含非負整數的變量，有兩個原子操作，wait和post，也可以說是P操作和V操作，P操作將信號量減一，V操作將信號量加一。如果信號量大於0，P操作直接減一，否則如果等於0，P操作將調用進程（線程）阻塞起來，直到另外進程（線程）執行V操作。

信號量可以理解為一種資源的數量，通過這種資源的分配來實現同步或者互斥。當信號量的值為1時，可以實現互斥的功能，此時信號量就是二值信號量，如果信號量的值大於一時，可以實現進程（線程）併發執行。信號量和互斥鎖條件變量之間的區別是：互斥鎖必須由給它上鎖的進程（線程）來解鎖，而信號燈P操作不必由執行過它V操作的進程（線程）來執行；互斥鎖類似於二值信號量，要麼加鎖，要麼解鎖；當向條件變量發信號時，如果此時沒有等待在該條件變量上的線程，信號將丟失，而信號量不會。信號量主要用於之間同步，但也可以用在線程之間。互斥鎖和條件變量主要用於線程同步，但也可以用於進程間的同步。

POSIX信號量是一個sem_t的變量類型，分有名信號量和無名信號量，無名信號量用在共享信號量內存的情況下，比如同一個進程的不同線程之間，或父子進程之間，有名信號量隨內核持續的，所以我們可以跨多個程序操作它們。


### 1.  函數sem_open創建一個有名信號量或打開一個已存在信號量。函數原型如下：

```c
#include <fcntl.h> /* For O_* constants */  
#include <sys/stat.h> /* For mode constants */  
#include <semaphore.h>  
  
sem_t *sem_open(const char *name, int oflag);  
sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value);  
Link with -pthread  
```

參數name為posix IPC 名稱， 參數oflag可以為0， O_CREAT 或O_CREAT | O_EXCL， 如果指定了O_CREAT，第三、四參數需要指定。mode為指定權限位，value參數指定信號量初始值，二值信號量的值通常為1，計數信號量的值通常大於1。
函數返回執行sem_t數據類型的指針，出錯返回SEM_FAILED。


###2. 函數sem_close關閉有名信號量。

```c
#include <semaphore.h>  
int sem_close(sem_t *sem);  
Link with -pthread.
```

### 3. 函數sem_unlink刪除有名信號量。

```c
#include <semaphore.h>  
int sem_unlink(const char *name);  
Link with -pthread.
```

###4.函數sem_wait和函數sem_trywait測試信號量的值，如果信號量值大於0，函數將信號量的值減一併立即返回，如果信號量的值等於0，sem_wait函數開始睡眠，直到信號量的值大於0，這是再減一併立即返回，而sem_trywait函數不睡眠直接返回，並返回EAGAIN錯誤。函數原型為：


```c
#include <semaphore.h>  
int sem_wait(sem_t *sem);  
int sem_trywait(sem_t *sem);  
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);  
Link with -pthread.  
```


### 5. 函數sem_post執行和sem_wait相反的操作。當一進程（線程）執行完操作之後，應該調用sem_post，將指定信號量加一，然後喚醒正在等待該信號量變為1的進程（線程）。函數原型：

```c
#include <semaphore.h>  
int sem_post(sem_t *sem);  
Link with -pthread.  
```

### 6. 函數sem_getvalue獲取指定信號量的當前值，如果當前信號量已上鎖，返回0，或者為負數，其絕對值為等待該信號量的解鎖的線程數。

```c
#include <semaphore.h>  
int sem_getvalue(sem_t *sem, int *sval);  
Link with -pthread.  
```

###   7. 無名信號量使用sem_init()初始化，函數原型為：


```c
#include <semaphore.h>  
int sem_init(sem_t *sem, int pshared, unsigned int value);  
Link with -pthread.  
```

sem為無名信號量地址，value為信號量初始值，pshared指定信號量是在進程的各個線程之間共享還是在進程之間共享，如果該值為0，表示在進程的線程之間共享，如果非0則在進出之間共享，無名信號量主要用在有共同祖先的進程或線程之間。

這一節使用posix共享內存來實現各個進程之間的通信，並使用posix信號量來達到互斥與同步的目的。首先我們來看看使用信號量實現對共享內存段的互斥訪問。

之前線程的訪問使用如下方式來達到對公共的內存互斥訪問：


```c
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;  
......  
pthread_mutex_lock(&mutex);//加鎖  
......  
/*share memory handle*/  
......  
pthread_mutex_unlock(&mutex);//解鎖  
......  
```

現在我們也使用類似方式來實現：

```c
sem_t *sem_mutex = NULL;  
......  
SLN_MUTEX_SHM_LOCK(SEM_MUTEX_FILE, sem_mutex);//加鎖  
......  
/*share memory handle*/  
 ......  
SLN_MUTEX_SHM_UNLOCK(sem_mutex);//解鎖  
...... 
```
其中SEM_MUTEX_FILE為sem_open函數需要的有名信號量名稱。

其中兩個加鎖解鎖的實現為：

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

其實就是初始化一個二值信號量，其初始值為1，並執行wait操作，使信號量的值變為0，此時其它進程想要操作共享內存時也需要執行wait操作，但此時信號量的值為0，所以開始等待信號量的值變為1。噹噹前進程操作完共享內存後，開始解鎖，執行post操作將信號量的值加一，此時其它進程的wait可以返回了。

下面為一個互斥訪問共享內存的示例，posix共享內存實現請查看前面IPC的系列文章。
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

    SLN_MUTEX_SHM_LOCK(SEM_MUTEX_FILE, sem_mutex); //加鎖

    nms_shm_get(SHM_FILE, (void**)&str,
                SHM_MAX_LEN);  //下面三行互斥訪問共享內存
    sleep(6);
    snprintf(str, SHM_MAX_LEN, "posix semphore server!");

    SLN_MUTEX_SHM_UNLOCK(sem_mutex); //解鎖

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

先啟動服務進程首先加鎖，創建共享內存並操作它，加鎖中sleep 6秒，以便測試客戶進程是否在服務進程未釋放鎖時處於等待狀態。客戶進程在服務進程啟動之後馬上啟動，此時處於等待狀態，當服務進程6秒之後解鎖，客戶進程獲得共享內存信息。再過6秒之後，服務進程刪除共享內存，客戶進程再此獲取共享內存失敗。


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

posix有名信號量創建的信號量文件和共享內存文件在/dev/shm/目錄下：

```sh
# ls /dev/shm/   
 sem.sem_mutex share_memory_file  
 ```
 
 在兩個進程共享數據時，當一個進程向共享內存寫入了數據後需要通知另外的進程，這就需要兩個進程之間實現同步，這裡我們給上面的程序在互斥的基礎上加上同步操作。同步也是使用posix信號量來實現。
server process：


```c
int main(int argc, const char* argv[])
{
    sem_t* sem_mutex = NULL;
    sem_t* sem_consumer = NULL, *sem_productor = NULL;
    int semval;
    char* sharememory = NULL;

    sem_consumer = sem_open(SEM_CONSUMER_FILE, O_CREAT, 0666,
                            0); //初始化信號量sem_consumer ，並設置初始值為0

    if (SEM_FAILED == sem_consumer) {
        printf("sem_open <%s>: %s\n", SEM_CONSUMER_FILE, strerror(errno));
        return -1;
    }

    sem_productor = sem_open(SEM_PRODUCTOR_FILE, O_CREAT, 0666,
                             0);//初始化信號量sem_productor ，並設置初始值為0

    if (SEM_FAILED == sem_productor) {
        printf("sem_open <%s>: %s\n", SEM_PRODUCTOR_FILE, strerror(errno));
        return -1;
    }

    for (;;) {//服務進程一直循環處理客戶進程請求
        sem_getvalue(sem_consumer, &semval);
        printf("%d waiting...\n", semval);

        if (sem_wait(sem_consumer) <
            0) {//如果sem_consumer為0，則阻塞在此，等待客戶進程post操作使sem_consumer大於0，此處和客戶進程同步
            printf("sem_wait: %s\n", strerror(errno));
            return -1;
        }

        printf("Get request...\n");

        SLN_MUTEX_SHM_LOCK(SEM_MUTEX,
                           sem_mutex);//此處開始互斥訪問共享內存
        nms_shm_get(SHM_FILE, (void**)&sharememory, SHM_MAX_LEN);
        sleep(6);
        snprintf(sharememory, SHM_MAX_LEN, "Hello, this is server's message!");
        SLN_MUTEX_SHM_UNLOCK(sem_mutex);

        sem_post(sem_productor);//使信號量sem_productor加一，使阻塞的客戶進程繼續執行
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
                            O_RDWR);//獲取信號量sem_consumer的值

    if (SEM_FAILED == sem_consumer) {
        printf("sem_open <%s>: %s\n", SEM_CONSUMER_FILE, strerror(errno));
        return -1;
    }

    sem_productor = sem_open(SEM_PRODUCTOR_FILE,
                             O_RDWR);//獲取信號量sem_productor 的值

    if (SEM_FAILED == sem_productor) {
        printf("sem_open <%s>: %s\n", SEM_PRODUCTOR_FILE, strerror(errno));
        return -1;
    }

    //clear_exist_sem(sem_productor);

    SLN_MUTEX_SHM_LOCK(SEM_MUTEX, sem_mutex);//互斥訪問共享內存
    nms_shm_get(SHM_FILE, (void**)&sharememory, SHM_MAX_LEN);
    printf("sharememory: %s\n", sharememory);
    SLN_MUTEX_SHM_UNLOCK(sem_mutex);

    sem_post(sem_consumer);//信號量sem_consumer加一，喚醒是阻塞在該信號量上的服務進程
    printf("Post...\n");
    sem_wait(sem_productor);//等待服務進程迴應
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
