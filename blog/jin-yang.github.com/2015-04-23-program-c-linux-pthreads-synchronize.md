---
title: Linux 線程同步
layout: post
comments: true
language: chinese
category: [program,linux]
keywords: linux,program,pthread
description: 線程的三個主要同步原語：互斥鎖 (mutex)、信號量 (semaphore) 和條件變量 (cond)。其中 mutex 和 sem 都是對應 futex 進行了簡單的封裝，在不存在衝突的情況下就不用陷入到內核中進行仲裁；而且 pthread_join 也是藉助 futex 來實現的。簡單介紹下 Linux 中與線程相關的編程。
---

線程的三個主要同步原語：互斥鎖 (mutex)、信號量 (semaphore) 和條件變量 (cond)。其中 mutex 和 sem 都是對應 futex 進行了簡單的封裝，在不存在衝突的情況下就不用陷入到內核中進行仲裁；而且 pthread_join 也是藉助 futex 來實現的。

簡單介紹下 Linux 中與線程相關的編程。

<!-- more -->

<!-- 而 cond 則是依靠 mutex 和信號來實現其語意，也就是說信號存在丟失的情況。-->

## 簡介

Linux 線程在核內是以輕量級進程的形式存在的，擁有獨立的進程表項，而所有的創建、同步、刪除等操作都在核外的 `pthread` 庫中進行。


## 鎖

### 互斥鎖


{% highlight text %}
----- 初始化一個互斥鎖，可以通過PTHREAD_MUTEX_INITIALIZER宏進行初始化
int pthread_mutex_init(pthread_mutex_t* mutex, const thread_mutexattr_t* mutexattr);

----- 獲取互斥鎖，如果被鎖定，當前線程處於等待狀態；否則，本線程獲得互斥鎖並進入臨界區
int pthread_mutex_lock(pthread_mutex_t* mutex);

----- 與lock操作不同的是，在嘗試獲得互斥鎖失敗後，不會進入阻塞狀態，而是返回線程繼續執行
int pthread_mutex_trylock(pthread_mutex_t* mutex);

----- 釋放互斥鎖，被阻塞的線程會獲得互斥鎖然後執行
int pthread_mutex_unlock(pthread_mutex_t* mutex);

----- 用來釋放互斥鎖的資源
int pthread_mutex_destroy(pthread_mutex_t* mutex);
{% endhighlight %}

使用方式如下。

{% highlight text %}
pthread_mutex_t mutex;
pthread_mutex_init(&mutex,NULL);

void pthread1(void* arg)
{
   pthread_mutex_lock(&mutex);
   ... ...  //臨界區
   pthread_mutex_unlock(&mutex);
}

void pthread2(void* arg)
{
   pthread_mutex_lock(&mutex);
   ... ...  //臨界區
   pthread_mutex_unlock(&mutex);
}
{% endhighlight %}

### 讀寫鎖

讀寫鎖與互斥量類似，不過讀寫鎖允許更高的並行性，適用於讀的次數大於寫的次數的數據結構。一次只有一個線程可以佔有寫模式的讀寫鎖，但是多個線程可以同時佔有讀模式的讀寫鎖。

{% highlight text %}
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int pthread_rwlock_init(pthread_rwlock_t* rwlock, const pthread_rwlockattr_t* attr);

int pthread_rwlock_rdlock(pthread_rwlock_t* rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t* rwlock);

int pthread_rwlock_unlock(pthread_rwlock_t* rwlock);
int pthread_destroy(pthread_rwlock_t* rwlock);
{% endhighlight %}



### 自旋鎖

互斥鎖或者讀寫鎖是靠阻塞線程的方式是使其進入休眠，自旋鎖是讓線程不斷循判斷自旋鎖是否已經被解鎖，而不會使其休眠，適用於佔用自旋鎖時間比較短的場景。

## 條件變量

信號量只有鎖和不鎖兩種狀態，當條件變量和信號量一起使用時，允許線程以無競爭的方式等待特定的條件發生。也就是說，條件本身是由互斥量保護，線程在改變條件狀態之前必須先鎖住互斥量。

{% highlight text %}
----- 動態初始化條件變量，也可以通過PTHREAD_COND_INITIALIZER宏進行初始化
int pthread_cond_init(pthread_cond_t* cond,const pthread_condattr_t* attr);

----- 阻塞等待條件變量為真，注意線程會被條件變量一直阻塞。
int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex);

----- 與wait類似，但是可以設置超時時間，在經歷tspr時間後，即使條件變量不滿足，阻塞也被解除
int pthread_cond_timedwait(pthread_cond_t* cond, pthread_mutex_t* mutex,const struct timespec* tspr);

----- 喚醒因為條件變量阻塞的線程，至少會喚醒一個等待隊列上的線程
int pthread_cond_signal(pthread_cond_t* cond);

----- 喚醒等待該條件的所有線程
int pthread_cond_broadcast(pthread_cond_t* cond);

----- 銷燬條件變量
int pthread_cond_destroy(pthread_cond_t* cond);
{% endhighlight %}

注意，如果有多個線程在等待信號的發生，那麼在通過 `pthread_cond_signal()` 發送信號時，會根據調度策略選擇等待隊列中的一個線程。

### 示例

比較典型的應用是生產者和消費者模型。如下的示例中，兩個線程共享 `x` 和 `y` 變量，條件變量用於表示 `x > y`  成立。

{% highlight text %}
int x, y;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

//----- 等待x>y成立後執行
pthread_mutex_lock(&mut);
while (x <= y) {
        pthread_cond_wait(&cond, &mut);
}
/* operate on x and y */
pthread_mutex_unlock(&mut);

//----- 修改x,y，可能會導致x>y，如果是那麼就觸發條件變量
pthread_mutex_lock(&mut);
/* modify x and y */
if (x > y)
        pthread_cond_broadcast(&cond);
pthread_mutex_unlock(&mut);
{% endhighlight %}

這個處理邏輯看起來還比較簡單，但是為什麼在被喚醒後還要進行條件判斷，也就是為什麼要使用 `while` 循環來判斷條件。實際上更多的是為了跨平臺的兼容，由於對於調度策略沒有明確的規定，那麼不同的實現就可能會導致 "驚群效應"。

例如，多個線程在等待信號量，同時被喚醒，那麼只有一個線程會進入臨界區進行處理，那麼此時就必須要再次條件判斷。

### 原理

首先重點解釋下 `pthread_cond_wait()` 函數，該函數功能可以直接通過 `man 3p pthread_cond_wait` 查看；簡單來說，其實現的功能是，釋放 `mutex` 並阻塞到 `cond` ，更重要的是，這兩步是 **原子操作** 。

在解釋之前，為了防止與 NTPL 中的實現混淆，暫時去掉 `pthread` 頭，僅為方便說明。

#### 1. 基本條件變量

{% highlight text %}
----- 如上，在等待事件時，通常會通過while()進行條件檢測
while(pass == 0)
    cond_wait(...);
 
----- 當條件被改變時，通過signal()函數通知到其它線程
pass = 1;
cond_signal(...)
{% endhighlight %}

#### 2. 互斥量保護

注意，如上的兩個線程中都涉及到了共享變量 `pass` ，那麼為了防止競態條件的發生，需要通過加鎖進行保護，也就是如下的內容：

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
mutex_unlock(mtx); // 必須要在cond_signal之後，否則同樣存在競態條件
{% endhighlight %}

#### 3. 條件變量語義

實際在上述的示例中，隱含了 `cond_wait()` 必須要自動釋放鎖，也就是說如果只是等待條件變量，實際的內容如下：

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
久而久之，程序員發現unlock, just_wait, lock這三個操作始終得在一起。於是就提供了一個pthread_cond_wait()函數來同時完成這三個函數。另外一個證據是，signal()函數是不需要傳遞mutex參數的，所以關於mutex參數是用於同步wait()和signal()函數的說法更加站不住腳。所以我的結論是：傳遞的mutex並不是為了防止wait()函數內部的Race Condition！而是因為調用wait()之前你總是獲得了某個mutex（例如用於解決此處pass變量的Race Condition的mutex），並且這個mutex在你調用wait()之前必須得釋放掉，調用wait()之後必須得重新獲取。所以，pthread_cond_wait()函數不是一個細粒度的函數，卻是一個實用的函數。

### 競態條件討論

其中第一行和第二行的兩個操作必須是 "原子化" 的操作，第三行可以分離出去。之所以要求第一、二行必須是原子操作，是因為要保證：如果線程 A 先進入 wait 函數 (可能還在釋放mtx)，那麼必須保證其它線程在其之後調用 broadcast 時能夠將線程 A 喚醒。

對於上述的操作：

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

如果執行序列是：a1, a2, a3, b1, b2, b3, b4, a4，那麼線程 A 將不會被喚醒，也就是說 wait() 是在 signal() 之前調用的，所以不滿足上文提到的保證。


先將線程附加到等待隊列
釋放mutex
進入等待

感興趣的同學的可以看下源碼（pthread_cond_wait.c），附加到等待隊列這個操作是加鎖的，所以可以保證之前發起的signal不會錯誤得喚醒本線程，而之後發起的signal必然喚醒本線程。因此，下面的代碼是絕對不會出錯的：// 線程A，條件測試
pthread_mutex_lock(mtx);        // a1
while(pass == 0) {              // a2 
    pthread_cond_wait(cv, mtx); // a3
}
pthread_mutex_unlock(mtx);      // a4

// 線程B，條件發生修改，對應的signal代碼
pthread_mutex_lock(mtx);   // b1
pass = 1;                  // b2
pthread_mutex_unlock(mtx); // b3
pthread_cond_signal(cv);   // b4
如果線程A先運行，那麼執行序列必然是：a1, a2, a3, b1, b2, b3, b4, a4。如果線程B先運行，那麼執行序列可能是：b1, b2, b3, b4, a1, a2, a4也可能是：b1, b2, b3, a1, a2, a3, b4, a4

http://blog.sina.com.cn/s/blog_967817f20101bsf0.html
另外需要了解Memory Barrier和Automatic Operation，無鎖編程
pthread_cleanup_push 
?????pthread_cancel 和取消點
-->


## 信號量


{% highlight text %}
----- 初始化信號量，pshared為0只能在當前進程的線程間共享，否則可以進程間共享，value給出了信號量的初始值
int sem_init(sem_t* sem, int pshared,unsigned int value);
----- 阻塞線程，直到信號量的值大於0，解除阻塞後同時會將sem的值減一
sem_wait(sem_t* sem);
----- 是wait的非阻塞版本
sem_trywait(sem_t* sem);
----- 增加信號量的值，會利用線程的調度策略選擇一個已經被阻塞的線程
sem_post(sem_t* sem);
----- 釋放信號量所佔用的資源
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

## 取消點

一個線程可以通過 `pthread_cancel()` 向另外一個線程發送結束請求，接收此請求的線程可以通過線程的兩個屬性來決定是否取消，以及是同步 (延時) 取消還是異步 (立即) 取消；而且即使該函數調用成功返回，也不代表線程就結束了。

可以通過如下的兩個函數設置線程的屬性：

{% highlight text %}
int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);

PTHREAD_CANCEL_ENABLE
  可取消，不過有兩種取消方式，一種是同步取消，另外一種是異步取消。
PTHREAD_CANCEL_DISABLE
  不可取消，那麼接收取消請求後，它依然繼續執行。
PTHREAD_CANCEL_DEFERRED
  可取消，會在下一個取消點退出；簡單來說，取消點就是程序在運行的時候檢測是否收到取消請求，常見的有如下函數：
    pthread_join() pthread_cond_wait() pthread_cond_timedwait() pthread_testcancel() sem_wait() sigwait()
PTHREAD_CANCEL_ASYNCHRONOUS
  異步取消，也就是說線程在收到取消請求之後，立即取消退出。
{% endhighlight %}

默認情況下，一個線程是可取消的並且是同步取消的。對於可以取消的線程，需要考慮退出時該線程後續的資源清理，例如，線程執行到一個鎖內時，已經獲得鎖了，這時異常退出了，那麼此時就是一個死鎖的問題。

此時需要使用如下兩個函數：

{% highlight text %}
#include <pthread.h>
void pthread_cleanup_push(void (*routine)(void *), void *arg);
void pthread_cleanup_pop(int execute);
{% endhighlight %}

如果函數沒有異常退出，那麼可以通過 `phread_cleanup_pop()` 彈出這些棧裡的函數，此時的參數要為 0，如果非 0 的話，彈出的同時也會執行這些函數的。例如，對於死鎖問題可以做如下處理：

{% highlight text %}
pthread_cleanup_push(pthread_mutex_unlock, (void *) &mutex);
pthread_mutex_lock(&mutex);
/* do some work */
pthread_mutex_unlock(&mutex);
pthread_cleanup_pop(0);
{% endhighlight %}


<!--
## futex

Futex 可以簡單理解為保存鎖的狀態以及一個等待該鎖的任務等待隊列。

### 源碼解析

內核中的實現在 `kernel/futex.c` 文件中，

初始化時會創建一個 hash 表，
http://blog.csdn.net/npy_lp/article/details/7331245

當非競態時，只需要在用戶空間處理即可，只有在出現競態時才會進入到內核空間中；而且針對非競態進行了優化。

其中地址是整數對齊的，而且只能通過原子的彙編指令操作。

futex_init()
 |-alloc_large_system_hash()  分配名為futex的hash表
 |-futex_detect_cmpxchg()

-->

## 參考

<!--
http://www.cnblogs.com/zhaoyl/p/3620204.html
http://blog.csdn.net/zmxiangde_88/article/details/7997874

http://www.cnblogs.com/blueclue/archive/2010/06/11/1754899.html
https://yangwenbo.com/articles/linux-implementation-of-posix-thread-cancellation-points.html
-->


{% highlight text %}
{% endhighlight %}
