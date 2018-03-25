# 從 pthread 轉換到 std::thread


以前一直都是用pthread的API寫C++的多線程程序。雖然很早之前就聽說，從C++11開始，標準庫裡已經包含了對線程的支持，不過一直沒有拿來用，最近剛好有空，藉著pthread的經驗學習下std::thread的用法。


## Thread


std::thread的構造函數方便得出人意料，這得感謝std::bind這個神奇的函數。在std::thread的構造函數裡，你可以直接傳遞一個函數和這個函數的參數列表給這個線程。你甚至可以傳遞一個類成員函數。如果你這麼做了，參數列表的第二個參數（第一個參數是被傳遞的成員函數）會被作為該類成員函數所作用的實例。

是不是有點繞……舉個例子來說吧：


```cpp
// 假設buy是一個可調用的函數對象，它即可能是函數指針，也可能是函數對象
std::thread Annie(buy);
// Annie會去執行buy()
std::thread Bob(buy, book, food);
// Bob會去執行buy(book, food)

// 假設buy是Consumer的一個可調用的成員函數
Consumer Clara;
std::thread action(buy, Clara, phone);
// Clara會去執行Consumer.buy(phone)
```

隨便提一下，當你創建了一個（非空的）線程對象時，對應線程就會執行，不需要顯式的調用start或者run。

如果之前你沒有用過pthread，也許不會理解何為「方便得出人意料」。

在pthread裡面，你需要這樣指定線程執行的函數:


```cpp
pthread_create(&thread, &attr, f, static_cast<void *>(&args));
// 其中f是函數，args是所有參數打包成的結構體。因為pthread_create的第四個參數類型是void*，所以需要強制轉型
```

考慮下之前那個Bob買書和飯菜的例子，如果要在pthread裡面實現，首先需要定義一個結構體，然後把book和food賦值給這個結構體的成員。

接著把結構體轉換成void*類型，傳遞進去。

這還沒完呢，因為剛剛的幾步只是實現了「傳進去」，還得「取出來」。
之後在函數buy中，再把void*的參數重新轉型成某個（可能是一次性的）結構體，最後取出book和food這兩個值。

Ok！終於搞定了。隨便一提，pthread_create只接受`void *f(void *)`這樣的函數簽名。如果你想調用現成的函數，你得包裝一下。

這就是為什麼std::thread的構造函數「方便得出人意料」。

創建線程後，調用Thread.join就會阻塞到線程執行結束為止（相當於pthread_join）。你也可以選擇detach該線程，這時候線程會獨立執行，不會隨調用者終止而結束。


## Mutex

有時候需要限制多個線程對同一資源的訪問，這時候一般會使用Mutex。Mutex就是一把鎖，只有某些線程可以同時佔用它（通過lock操作）。當線程不用的時候，就得通過unlock操作來釋放它。

對於Mutex，std::thread和pthread差不多，無非是`pthread_mutex_lock(&mutex)`變成了`mutex.lock()`等等。

不過在std::thread中，mutex往往和lock系列模板一起使用。這是因為lock系列模板包裝了mutex類，提供了RAII風格的加鎖解鎖。


```cpp
{
    std::lock_guard<std::mutex> guard(mutex); // 加鎖
    ...
    // 自動解鎖
}
```

## Condition variable

有時候線程之間需要某種同步——當某些條件不滿足時，停止執行直到該條件被滿足。這時候需要引入condition variable，狀態變量。

在經典的生產者消費者模式下，生產者和消費者就是通過condition variable來實現同步的。

當有限的生產力無法滿足日益增長的消費需求時，消費者進程就會去睡一覺，直到它想要的東西生產出來才醒來。

```cpp
std::condition_variable condvar;

consumer:
    std::unique_lock<std::mutex> ulock(mutex);
    condvar.wait(ulock, []{ return msgQueue.size() > 0;});

producer:
    condvar.notify_all();
```


<p><code>condition_variable</code>需要和<code>unique_lock</code>搭配使用。在一個線程調用wait之前，它必須持有<code>unique_lock</code>鎖。當<code>wait</code>被調用時，該鎖會被釋放，線程會陷入沉睡，等待著~~王子~~生產者發過來的喚醒信號。當生產者調用同一個<code>condition_variable</code>的<code>notify_all</code>方法時，所有沉睡在該變量前的消費者會被喚醒，並嘗試重新獲取之前釋放的<code>unique_lock</code>，繼續執行下去。（注意這裡發生了鎖爭用，只有一個消費者能夠獲得鎖，其他消費者得等待該消費者釋放鎖）。如果只想叫醒一個線程，可以用<code>notify_one</code>。pthread中也提供了對應的方法，分別是<code>pthread_cond_wait</code>,<code>pthread_cond_broadcast</code>,<code>pthread_cond_signal</code>。</p>

<p><code>wait</code>可以接受兩個參數。此時第二個參數用於判斷當前是否要沉睡。</p>

```cpp
[]{ return msgQueue.size() > 0;});
```

相當於

```cpp
while (msgQueue.size() <= 0) {
    condvar.wait()
}
```

嗯，還有一個問題。這裡把沉睡的線程比作睡美人，萬一王子變成了青蛙，來不及喚醒她，那睡美人不就得睡到天荒地老海枯石爛？


<p>為瞭解決這個問題，通過<code>wait_until</code>和<code>wait_for</code>，你可以設定線程的等待時間。設置<code>notify_all_at_thread_exit</code>也許能幫得上忙。在pthread，對應的調用是<code>pthread_cond_timedwait</code>。</p>


## More

C++11的線程庫還提供了其他多線程編程的概念，比如future和atomic。


## future

future包裝了未來某個計算結果的期諾。當你對所獲得的future調用get時，程序會一直阻塞直到future的值被計算出來。（如果future的值已經計算出來了，get調用會立刻獲得返回值）而這一切都是在後臺執行的。

舉個例子：（future相關的內容需要`#include <future>`）

```cpp
#include <chrono>
#include <iostream>
#include <future>

using namespace std;

int main()
{
    future<int> f1 = async(launch::async, [](){
        std::chrono::milliseconds dura(2000);
        std::this_thread::sleep_for(dura);
        return 0; 
    });
    future<int> f2 = async(launch::async, [](){
        std::chrono::milliseconds dura(2000);
        std::this_thread::sleep_for(dura);
        return 1; 
    });
    cout << "Results are: "
        << f1.get() << " " << f2.get() << "\n";
    return 0;
}
```

```sh
 g++ -std=c++11 -pthread ./future.cpp
$ time ./a.out 
Results are: 0 1
./a.out  0.00s user 0.00s system 0% cpu 2.012 total # 是兩秒左右而不是四秒哦

```

除了async， packaged_task和promise也都返回一個future。也許接下來我可能會寫一篇文章，講講這三者之間的差別。

## atomic

atomic位於頭文件atomic下，實現了類似於java.util.concurrent.atomic的功能。它提供了一組輕量級的、作用在單個變量上的原子操作，是volatile的替代品。有些時候你也可以用它來替換掉Lock（假如整個race condition中只有單個變量）

下面這個例子解釋了什麼叫做原子操作：

```cpp
#include <atomic>
#include <iostream>
#include <thread>

using namespace std;

const int NUM = 100;

int target = 0;
atomic<int> atomicTarget(0);

template<typename T>
void atomicPlusOne(int trys)
{
    while (trys > 0) {
        atomicTarget.fetch_add(1);
        --trys;
    }
}

void plusOne(int trys)
{
    while (trys > 0) {
        ++target;
        --trys;
    }
}

int main()
{
    thread threads[NUM];
    thread atomicThreads[NUM];
    for (int i = 0; i < NUM; i++) {
        atomicThreads[i] = thread(atomicPlusOne<int>, 10000);
    }
    for (int i = 0; i < NUM; i++) {
        threads[i] = thread(plusOne, 10000);
    }

    for (int i = 0; i < NUM; i++) {
        atomicThreads[i].join();
    }
    for (int i = 0; i < NUM; i++) {
        threads[i].join();
    }

    cout << "Atomic target's value : " << atomicTarget << "\n";
    cout << "Non-atomic target's value : " << target << "\n";
    // atomicTarget的值總是固定的，而target的值每次運行時各不相同
    //
    // g++ -std=c++11 -pthread ./atom.cpp
    // Atomic target's value : 1000000
    // Non-atomic target's value : 842480
    return 0;
}
```

## Pros and Cons

最後總結下std::thread對比於pthread的優缺點：
優點：
1. 簡單，易用
2. 跨平臺，pthread只能用在POSIX系統上（其他系統有其獨立的thread實現）
3. 提供了更多高級功能，比如future
4. 更加C++（跟匿名函數，std::bind，RAII等C++特性更好的集成）

缺點：
1. 沒有RWlock。有一個類似的shared_mutex，不過它屬於C++14,你的編譯器很有可能不支持。
2. 操作線程和Mutex等的API較少。畢竟為了跨平臺，只能選取各原生實現的子集。如果你需要設置某些屬性，需要通過API調用返回原生平臺上的對應對象，再對返回的對象進行操作。

附上我自己寫的，分別用std::thread和pthread實現的多生產者多消費者程序。注意行數上的差距。

pthread版本


```cpp
#include <pthread.h>
#include <queue>
#include <stdio.h>
#include <unistd.h>

// 注意pthread_*函數返回的異常值，為了簡單（偷懶），我沒有去處理它們

pthread_mutex_t mutex;
pthread_cond_t condvar;

std::queue<int> msgQueue;
struct Produce_range {
    int start;
    int end;
};

void *producer(void *args)
{
    int start = static_cast<Produce_range *>(args)->start;
    int end = static_cast<Produce_range *>(args)->end;
    for (int x = start; x < end; x++) {
        usleep(200 * 1000);
        pthread_mutex_lock(&mutex);
        msgQueue.push(x);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&condvar);
        printf("Produce message %d\n", x);
    }
    pthread_exit((void *)0);
    return NULL;
}

void *consumer(void *args)
{
    int demand = *static_cast<int *>(args);
    while (true) {
        pthread_mutex_lock(&mutex);
        if (msgQueue.size() <= 0) {
            pthread_cond_wait(&condvar, &mutex);
        }
        if (msgQueue.size() > 0) {
            printf("Consume message %d\n", msgQueue.front());
            msgQueue.pop();
            --demand;
        }
        pthread_mutex_unlock(&mutex);
        if (!demand) break;
    }
    pthread_exit((void *)0);
    return NULL;
}


int main()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condvar, NULL);

    pthread_t producer1, producer2, producer3, consumer1, consumer2;

    Produce_range range1 = {0, 10};
    pthread_create(&producer1, &attr, producer, static_cast<void *>(&range1));
    Produce_range range2 = {range1.end, range1.end + 10};
    pthread_create(&producer2, &attr, producer, static_cast<void *>(&range2));
    Produce_range range3 = {range2.end, range2.end + 10};
    pthread_create(&producer3, &attr, producer, static_cast<void *>(&range3));

    int consume_demand1 = 20;
    int consume_demand2 = 10;
    pthread_create(&consumer1, &attr, consumer, 
            static_cast<void *>(&consume_demand1));
    pthread_create(&consumer2, &attr, consumer, 
            static_cast<void *>(&consume_demand2));

    pthread_join(producer1, NULL);
    pthread_join(producer2, NULL);
    pthread_join(producer3, NULL);
    pthread_join(consumer1, NULL);
    pthread_join(consumer2, NULL);
} 
```

std::thread版本

```cpp
#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>

// 注意某些調用可能會拋出std::system_error， 為了簡單（偷懶），我沒有去捕獲
std::mutex mutex;
std::condition_variable condvar;

std::queue<int> msgQueue;

void producer(int start, int end)
{
    for (int x = start; x < end; x++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        {        
            std::lock_guard<std::mutex> guard(mutex);
            msgQueue.push(x);
        }
        printf("Produce message %d\n", x);
        condvar.notify_all();
    }
}

void consumer(int demand)
{
    while (true) {
        std::unique_lock<std::mutex> ulock(mutex);
        condvar.wait(ulock, []{ return msgQueue.size() > 0;});
        // wait的第二個參數使得顯式的double check不再必要
        printf("Consume message %d\n", msgQueue.front());
        msgQueue.pop();
        --demand;
        if (!demand) break;
    }
}


int main()
{
    std::thread producer1(producer, 0, 10);
    std::thread producer2(producer, 10, 20);
    std::thread producer3(producer, 20, 30);
    std::thread consumer1(consumer, 20);
    std::thread consumer2(consumer, 10);

    producer1.join();
    producer2.join();
    producer3.join();
    consumer1.join();
    consumer2.join();
} 
```