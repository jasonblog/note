# （二）：posix線程同步


上一節說到線程的互斥鎖，互斥鎖適合防止訪問某個共享變量，這一節我們來看看兩個線程如何實現同步。互斥鎖也可以實現線程同步，當該值滿足某種條件時當前線程繼續執行，否則繼續輪詢，不過這樣相當浪費cpu時間。我們需要的是讓某個線程進入睡眠，當滿足該線程執行條件時，另外線程主動通知它，這就是這一節我們要說的條件變量，它常和互斥鎖一起使用。

如同信號量，線程可以對一個條件變量執行等待操作。如果如果線程 A 正在等待一個條件變量，它會被阻塞直到另外一個線程B，向同一個條件變量發送信號以改變其狀態。不同於信號量，條件變量沒有計數值，也不佔據內存空間，線程 A 必須在 B 發送信號之前開始等待。如果 B 在 A 執行等待操作之前發送了信號，這個信號就丟失了，同時 A會一直阻塞直到其它線程再次發送信號到這個條件變量。

條件變量將允許你實現這樣的目的：在一種情況下令線程繼續運行，而相反情況下令線程阻塞。只要每個可能涉及到改變狀態的線程正確使用條件變量，Linux 將保證當條件改變的時候由於一個條件變量的狀態被阻塞的線程均能夠被激活。

GNU/Linux 剛好提供了這個機制，每個條件變量都必須與一個互斥體共同使用，以防止這種競爭狀態的發生。這種設計下，線程函數應遵循以下步驟： 

thread_function首先鎖定互斥體並且讀取標誌變量的值。 
如果標誌變量已經被設定，該線程將互斥體解鎖然後執行工作函數
如果標誌沒有被設置，該線程自動鎖定互斥體並開始等待條件變量的信號

這裡最關鍵的特點就在第三條。這裡，GNU/Linux系統允許你用一個原子操作完成解除互斥體鎖定和等待條件變量信號的過程而不會被其它線程在中途插入執行。這就避免了在thread_function中檢測標誌和等待條件變量的過程中其它線程修改標誌變量並對條件變量發送信號的可能性。

互斥鎖是類型為pthread_mutex_t的變量，和互斥鎖類似，初始化也兩種方法：
一種是靜態初始化，給鎖變量賦值PTHREAD_COND_INITIALIZER，

```c
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;  
```

一種動態初始化，使用函數pthread_cond_init，

```c
int pthread_cond_init(pthread_cond_t *restrict cond,  const pthread_condattr_t *restrict attr);
```

其中第一個參數是一個指向pthread_cond_t變量的指針。第二個參數是一個指向條件變量屬性對象的指針；這個參數在 GNU/Linux 系統中是被忽略的。

函數pthread_cond_wait（）使線程阻塞在一個條件變量上。函數原型如下：

```c
#include <pthread.h>  
int pthread_cond_wait(pthread_cond_t *restrict cond,  pthread_mutex_t *restrict mutex);  
```


第一個參數是指向一個 pthread_cond_t 類型變量的指針，第二個參數是指向一個pthread_mutex_t類型變量的指針。當調用 pthread_cond_wait 的時候，互斥體對象必須已經被調用線程鎖定。這個函數以一個原子操作解鎖互斥體並鎖定條件變量等待信號。當信號到達且調用線程被解鎖之後，pthread_cond_wait自動申請鎖定互斥體對象。

函數傳入的參數mutex用於保護條件，因為我們在調用pthread_cond_wait時，如果條件不成立我們就進入阻塞，但是進入阻塞這個期間，如果條件變量改變了的話，那我們就漏掉了這個條件。因為這個線程還沒有放到等待隊列上，所以調用pthread_cond_wait前要先鎖互斥量，即調用pthread_mutex_lock(),pthread_cond_wait在把線程放進阻塞隊列後，自動對mutex進行解鎖，使得其它線程可以獲得加鎖的權利。這樣其它線程才能對臨界資源進行訪問並在適當的時候喚醒這個阻塞的進程。當pthread_cond_wait返回的時候又自動給mutex加鎖。

函數pthread_cond_signal（）用來釋放被阻塞在條件變量cond上的一個線程。多個線程阻塞在此條件變量上時，哪一個線程被喚醒是由線程的調度策略所決定的。要注意的是，必須用保護條件變量的互斥鎖來保護這個函數，否則條件滿足信號又可能在測試條件和調用pthread_cond_wait函數之間被髮出，從而造成無限制的等待。函數原型如下：


```c
#include <pthread.h>  
int pthread_cond_signal(pthread_cond_t *cond);  
```

下面給一個示例：

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
                          &mutex); //當條件不滿足時，此時線程1會自動對mutex進行解鎖，使得其它線程可以獲得加鎖的權利，當收到甦醒時會在執行加鎖操作
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
                &cond); //當條件滿足時，線程2發送信號，線程1收到信號會解除阻塞狀態。
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

    sleep(1);     //讓線程1先執行，進入等待狀態

    if (0 != pthread_create(&tid2, NULL, func2_th, NULL)) {
        printf("pthread_create failed!\n");
        return -1;
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;
}
```
另一個用來阻塞線程的函數是pthread_cond_timedwait（）。函數原型如下：

```c
#include <pthread.h>  
int pthread_cond_timedwait(pthread_cond_t *restrict cond,  pthread_mutex_t *restrict mutex, const struct timespec *restrict abstime);   
```

pthread_cond_timedwait 和pthread_cond_wait 一樣，自動解鎖互斥量及等待條件變量， 但它還限定了等待時間。如果在 abstime 指定的時間內 cond 未觸發，互斥量 mutex 被重 新加鎖，並返回錯誤 ETIMEDOUT 。


。函數pthread_cond_broadcast（pthread_cond_t *cond）用來喚醒所有被阻塞在條件變量cond上的線程。這些線程被喚醒後將再次競爭相應的互斥鎖，所以必須小心使用這個函數。函數原型如下：


```c
#include <pthread.h>  
int pthread_cond_broadcast(pthread_cond_t *cond);  
```

http://download.csdn.net/detail/gentleliu/8280393

