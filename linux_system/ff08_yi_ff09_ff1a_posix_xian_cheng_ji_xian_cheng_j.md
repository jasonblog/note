# （一）：posix線程及線程間互斥


有了進程的概念，為何還要使用線程呢？

首先，回憶一下上一個系列我們講到的IPC，各個進程之間具有獨立的內存空間，要進行數據的傳遞只能通過通信的方式進行，這種方式不僅費時，而且很不方便。而同一個進程下的線程是共享全局內存的，所以一個線程的數據可以在另一個線程中直接使用，及快捷又方便。

其次，在Linux系統下，啟動一個新的進程必須分配給它獨立的地址空間，建立眾多的數據表來維護它的代碼段、堆棧段和數據段，這是一種"昂貴"的多任務工作方式。而運行於一個進程中的多個線程，它們彼此之間使用相同的地址空間，共享大部分數據，啟動一個線程所花費的空間遠遠小於啟動一個進程所花費的空間，而且，線程間彼此切換所需的時間也遠遠小於進程間切換所需要的時間。

但是，伴隨著這些優點，線程卻帶來了同步與互斥的問題。下面先講講線程基本函數：

###1. 線程的創建pthread_create

```c
#include <pthread.h>
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
              void *(*start_routine) (void *), void *arg);
```

 一個線程由一個線程ID（參數thread）標識，新的線程創建成功，其值通過指針thread返回。

參數attr為線程屬性（比如：優先級、初始棧大小等），通常我們使用默認設置，設為NULL。

參數start_routine為一個函數指針，指向線程執行的函數，最後參數arg為函數start_routine唯一參數，如果需要傳遞多個參數，需要打包為結構，然後將其地址傳給該函數。

pthread_create成功時返回0，失敗為非0值，這和其他linux系統調用的習慣不一樣。

###2. pthread_join函數

```c
#include <pthread.h>  
int pthread_join(pthread_t thread, void **retval);  
```


通過調用該函數等待一個給定線程終止，類似於線程的waitpid函數。

該函數等待參數thread指定的線程終止，該函數會阻塞，直到線程thread終止，將線程返回的(void *)指針賦值為retval指向的位置，然後回收已經終止線程佔用的所有存儲器資源。


###3. pthread_self函數

```c
#include <pthread.h>  
pthread_t pthread_self(void);
```

該函數用於獲取線程自身線程ID。類似於進程的getpid函數。


###4. pthread_detach函數

```c
#include <pthread.h>  
int pthread_detach(pthread_t thread);  
```

 該函數可分離可結合線程，線程可以通過以pthread_self()為參數的pthread_detach調用來分離他們自己。

一個分離線程是不能被其他線程回收或殺死的，他的存儲器資源在他終止時由系統自動釋放。一個可結合線程能夠被其他線程收回其資源和殺死，在被其他線程收回之前，他的存儲器資源是沒有被釋放的。在任何一個時間點上，線程是可結合的或者是可分離的。默認情況下，線程是被創建成可結合的。


為了避免存儲器洩露，每個可結合線程都應該要麼被其他線程現實的收回，要麼通過調用pthread_detach函數被分離。


在現實的程序中，我們一般都使用分離的線程。

### 5. pthread_exit函數

```c
#include <pthread.h>  
void pthread_exit(void *retval);
```

 該函數作用就是終止線程。如果該線程未曾分離，他的線程ID和退出狀態將一直保留到調用進程內某個其他線程對他調用pthread_join。

另外，當線程函數（pthread_create第三個參數）返回時，該線程將終止；當創建該線程的進程main函數返回時，該線程也將終止。

下面給一個簡單的示例
 
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

執行2次輸出為：
 
```sh
# ./target_bin  
=======func_th->9==thread1: 3077856064====  
=======func_th->9==thread2: 3069463360====  
# ./target_bin  
=======func_th->9==thread2: 3069315904====  
=======func_th->9==thread1: 3077708608====  
```

 類似於進程，線程的調度隨機的。


在前面開始我們說到同一個進程內的線程是共享全局內存的，那麼當多個線程同時去修改一個全局變量的時候就會出問題，如果一個線程在修改某個變量時中途被掛起，操作系統去調度另外一個線程執行，那就可能導致錯誤。我們無法保證操作系統對這些操作都是原子的。

在我們在現在的例子中這樣去復現這種問題：一個線程對一個全局變量（100）進行讀-加1-讀操作，另個變量對該全局進行減1操作，我們通過sleep來實現加線程先執行，減線程在加線程的加和讀之間進行，最後來查看加操作是否是我們期望的結果（101）。例子如下：

（該示例只是為了強化運行時出錯，並對這種錯誤有一個宏觀的瞭解而寫）

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

    sleep(4);//此時add線程掛起，sub線程執行鍵操作

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

    sleep(1);  //保證add線程先被調度
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
執行結果如下：


```sh
# ./target_bin  
==do func_add_th==thread1: 3078355776====  
before add 1, gval=100  
==do func_sub_th==thread2: 3069963072====  
after add 1, gval=100  
```

通過輸出我們可以看到sub操作在加1和讀之間操作，最終讀取出來的值仍然是100，不是我們期望的101。

這就是兩個線程不是互斥帶來的結果，所以我們希望在某某一線程一段代碼執行期間，只有一個線程在運行，當運行完成之後，下一個線程運行該部分代碼，所以我們需要將該部分代碼加鎖。這就是線程編程，也是併發編程需要考慮的問題。


解決多線程共享的問題就是使用互斥鎖（mutex，即mutual exliusion）來保護共享數據。在執行某一段代碼是首先要持有該互斥鎖，執行完成之後再釋放該鎖。互斥鎖是類型為pthread_mutex_t的變量。使用如下方法來加鎖和解鎖操作。


派生到我的代碼片

```c
#include <pthread.h>  
int pthread_mutex_lock(pthread_mutex_t *mutex);  
int pthread_mutex_trylock(pthread_mutex_t *mutex);  
int pthread_mutex_unlock(pthread_mutex_t *mutex);  
```

首先我們需要初始化鎖，初始化方法有兩種，一種是靜態初始化，給鎖變量賦值PTHREAD_MUTEX_INITIALIZER，一種動態初始化，使用函數pthread_mutex_init。

我們使用靜態方法初始化：

```c
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;
```

 當試圖使用pthread_mutex_lock（）獲得一個已經被另外線程加鎖的鎖時，本線程將阻塞，直到互斥鎖被解鎖為止。函數pthread_mutex_trylock為獲取鎖的非阻塞版本，當獲取失敗時會立即返回。

我們修改add和sub線程函數分別如下：

```c
void* func_add_th(void* arg)
{
    int*     val = (int*)arg;

    pthread_mutex_lock(&mutex);//此處加鎖
    printf("==do %s==thread%d: %u====\n", __func__,
           *val, (unsigned int)pthread_self());

    printf("before add 1, gval=%d\n", gval);
    gval += 1;

    sleep(4);

    printf("after add 1, gval=%d\n", gval);
    pthread_mutex_unlock(&mutex);//此處釋放鎖
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

執行結果為：

```sh
# ./target_bin  
==do func_add_th==thread1: 3077614400====  
before add 1, gval=100  
after add 1, gval=101  
==do func_sub_th==thread2: 3069221696====  
```

 通過結果輸出可以看到，sub操作是在add操作執行完成之後才執行的，而add線程輸出結果也是我們預期的，所以我們的加鎖是成功的。但是如果add線程要執行很久的話，sub線程就要阻塞很久，我們可以將sub線程加鎖函數改為非阻塞版本，當加鎖失敗時，立即返回。

修改後的sub線程函數：

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
運行輸出為：

```sh
# ./target_bin  
==do func_add_th==thread1: 3077638976====  
before add 1, gval=100  
failed to lock!  
after add 1, gval=101  
```

當多個線程同時需要多個相同鎖時，可能會出現死鎖的情況。比如兩個線程同時需要互斥鎖1和互斥鎖2，線程a先獲得鎖1，線程b獲得鎖2，這是線程a、b分別還需要鎖2和鎖1，但此時兩個鎖都被加鎖了，都阻塞在那裡等待對方釋放鎖，這樣死鎖就出現了。我們來實現一下死鎖的情況，將之前兩個例子的線程函數修改如下：

```c
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     mutex_sec = PTHREAD_MUTEX_INITIALIZER;

void* func_add_th(void* arg)
{
    int*     val = (int*)arg;

    pthread_mutex_lock(&mutex);//1. add線程先加第一個鎖mutex
    printf("==do %s==thread%d: %u====\n", __func__,
           *val, (unsigned int)pthread_self());
    sleep(2);//等待2秒，讓sub線程加第二個鎖mutex_sec
    pthread_mutex_lock(&mutex_sec);//4. add線程加鎖mutex_sec失敗

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

    pthread_mutex_lock(&mutex_sec);//2. Sub線程比add線程先加鎖mutex_sec
    pthread_mutex_lock(&mutex);//3. Sub線程加鎖mutex失敗

    printf("==do %s==thread%d: %u====\n", __func__,
           *val, (unsigned int)pthread_self());

    gval -= 1;
    pthread_mutex_unlock(&mutex);
    pthread_mutex_unlock(&mutex_sec);

    return NULL;
}
```

上面兩個線程按照函數註釋中1-2-3-4順序執行，運行時程序就卡在那裡出現了死鎖。

可以使用非阻塞版本的加鎖函數來加鎖，不過也要注意在第二個鎖加鎖不成功情況下，需要釋放第一個鎖再返回，不然其他線程仍然得不到第一個鎖。有時在線程需要多個互斥鎖時，讓線程按照指定的同樣順序進行加鎖也可以避免死鎖。程序死鎖出現時很難定位，所以程序猿在編程（尤其是在設計）時需要注意避免這個問題。


本節示例代碼下載：

http://download.csdn.net/detail/gentleliu/8270863
 