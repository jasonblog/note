# C/C++每線程（thread-local）變量的使用


在一個進程中定義的全局或靜態變量都是所有線程可見的，即每個線程共同操作一塊存儲區域。而有時我們可能有這樣的需求：對於一個全局變量，每個線程對其的修改只在本線程內有效，各線程之間互不幹擾。即每個線程雖然共享這個全局變量的名字，但這個變量的值就像只有在本線程內才會被修改和讀取一樣。
線程局部存儲和線程特有數據都可以實現上述需求。

## 1. 線程局部存儲


線程局部存儲提供了持久的每線程存儲，每個線程都擁有一份對變量的拷貝。線程局部存儲中的變量將一直存在，直到線程終止，屆時會自動釋放這一存儲。一個典型的例子就是errno的定義（uClibc-0.9.32），每個線程都有自己的一份errno的拷貝，防止了一個線程獲取errno時被其他線程幹擾。

要定義一個線程局部變量很簡單，只需簡單的在全局或靜態變量的聲明中包含`__thread`說明符即可。例如：

```c
static __thread int buf[MAX_ERROR_LEN];  
```

這樣定義的變量，在一個線程中只能看到本線程對其的修改。
關於線程局部變量的聲明和使用，需要注意以下幾點：

1. 如果變量聲明中使用了關鍵字static或extern，那麼關鍵字__thread必須緊隨其後。
2. 與一般的全局或靜態變量聲明一樣，線程局部變量在聲明時可以設置一個初始值。
3. 可以使用C語言取址操作符（&）來獲取線程局部變量的地址。

在一個線程中修改另一個線程的局部變量：
__thread變量並不是在線程之間完全隱藏的，每個線程保存自己的一份拷貝，因此每個線程的這個變量的地址不同。但這個地址是整個進程可見的，因此一個線程獲得另外一個線程的局部變量的地址，就可以修改另一個線程的這個局部變量。


C++中對__thread變量的使用有額外的`限制`：

1. 在C++中，如果要在定義一個thread-local變量的時候做初始化，初始化的值必須是一個常量表達式。 
2. __thread只能修飾POD類型，即不帶自定義的構造、拷貝、賦值、析構的類型，不能有non-static的protected或private成員，沒有基類和虛函數，因此對定義class做了很多限制。但可以改為修飾class指針類型便無需考慮此限制。

## 2. 線程特有數據

`上面是C/C++語言實現每線程變量的方式`，而POSIX thread使用getthreadspecific和setthreadspecific 組件來實現這一特性，因此編譯要加-pthread，但是使用這種方式使用起來`很繁瑣，並且效率很低`。不過我也簡單講一下用法。
使用線程特有數據需要下面幾步：

1. 創建一個鍵（key），，用以將不同的線程特有數據區分開來。調用函數pthread_key_create()可創建一個key，且只需要在首個調用該函數的線程中創建一次。
2. 在不同線程中，使用pthread_setspecific()函數將這個key和本線程（調用者線程）中的某個變量的值關聯起來，這樣就可以做到不同線程使用相同的key保存不同的value。
3. 在各線程可通過pthread_getspecific()函數來取得本線程中key對應的值。

三個接口函數的說明：


```cpp
#include <pthread.h>  
int pthread_key_create(pthread_key_t * key, void (*destructor)(void *));  
```


用於創建一個key，成功返回0。
函數destructor指向一個自定義函數，定義如下。在線程終止時，會自動執行該函數進行一些析構動作，例如釋放與key綁定的存儲空間的資源。如果無需解構，可將destructor置為NULL。
```cpp
void dest(void *value)
{
 /* Release storage pointed to by 'value' */
}
```

參數value是與key關聯的指向線程特有數據塊的指針。

注意，如果一個線程有多個線程特有數據塊，那麼對各個解構函數的調用順序是不確定的，因此每個解構函數的設計要相互獨立。

```cpp
int pthread_setspecific(pthread_key_t key, const void * value);  
```

用於設置key與本線程內某個指針或某個值的關聯。成功返回0。

```cpp
void *pthread_getspecific(pthread_key_t key);  
```

用於獲取key關聯的值，由該函數的返回值的指針指向。如果key在該線程中尚未被關聯，該函數返回NULL。

```cpp
int pthread_key_delete(pthread_key_t key);  
```

用於註銷一個key，以供下一次調用pthread_key_create()使用。
Linux支持最多1024個key，一般是128個，所以通常key是夠用的，如果一個函數需要多個線程特有數據的值，可以將它們封裝為一個結構體，然後僅與一個key關聯。

寫一個例子：



```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

pthread_key_t key;

static void key_destrutor(void* value)
{
    printf("dest called\n");
    /* 這個例子中，關聯key的值並沒有malloc等操作，因此不用做釋放動作。 */
    return (void)0;
}

int get_pspec_value_int()
{
    int* pvalue;
    pvalue = (int*)pthread_getspecific(key);
    return *pvalue;
}

void* thread_handler(void* args)
{
    int index = *((int*)args);
    int pspec;

    pspec = 0;
    /* 設置key與value的關聯 */
    pthread_setspecific(key, (void*)&pspec);

    while (1) {
        sleep(4);
        /* 獲得key所關聯的value */
        pspec = get_pspec_value_int();
        printf("thread index %d = %d\n", index, pspec);
        /* 修改value的值，本例中用於測試不同線程的value不會互相干擾。 */
        pspec += index;
        pthread_setspecific(key, (void*)&pspec);
    }

    return (void*)0;
}

int main()
{
    pthread_t pid1;
    pthread_t pid2;
    int ret;
    int index1 = 1, index2 = 2;

    struct thr1_st m_thr_v, *p_mthr_v;

    /* 創建一個key */
    pthread_key_create(&key, key_destrutor);

    if (0 != (ret = pthread_create(&pid1, NULL, thread_handler, (void*)&index1))) {
        perror("create thread failed:");
        return 1;
    }

    if (0 != (ret = pthread_create(&pid2, NULL, thread_handler, (void*)&index2))) {
        perror("create thread failed:");
        return 1;
    }

    /* 設置key與value的關聯 */
    memset(&m_thr_v, 0, sizeof(struct thr1_st));
    pthread_setspecific(key, (void*)&m_thr_v);

    while (1) {
        sleep(3);

        /* 獲得key所關聯的value */
        p_mthr_v = (struct thr1_st*)pthread_getspecific(key);
        printf("main len = %d\n", p_mthr_v->len);
        /* 修改value的值，本例中用於測試不同線程的value不會互相干擾。 */
        p_mthr_v->len += 5;
        pthread_setspecific(key, (void*)p_mthr_v);
    }

    /* 註銷一個key */
    pthread_key_delete(key);
    pthread_join(pid1, 0);
    pthread_join(pid2, 0);

    return 0;
}
```

上面的例子說明瞭如何定義線程特有數據。其中由於本例中的數據只是一個value而已，所以並沒有必須註冊解構函數，而如果是進行了malloc的指針，則需要在解構函數中釋放，否則會出現內存洩露。執行這個程序就會看到每個線程對關聯到key的值的修改是互不幹擾的，也即實現了線程特有數據存儲。

另外值得注意的是，pthread_key_create()只需在第一個使用這個key的線程中調用一次即可，在這個例子中，很明顯要在main函數中調用。而如果我們要實現一個庫函數，這個庫函數中需要創建並使用key，那麼就會造成多次調用pthread_key_create()。
`pthread_once()`函數可以解決這樣的問題，其聲明如下：

```cpp
#include <pthread.h>  
int pthread_once(pthread_once_t *once_control, void (*init)(void)); 
```

該函數可以做到無論有多少個線程對該函數調用了多少次，都只會在第一次被調用時執行自定義的init函數。
參數once_control是一個指針，指向初始化為PTHREAD_ONCE_INIT的靜態變量，例如：

```cpp
pthread_once_t once_var = PTHREAD_ONCE_INIT;  
```

該變量通過自身狀態的變化來控制只有在第一次被調用的時候才執行init回調函數。

## 參考資料；

[1] 孫劍等 譯 Michael Kerrisk 著. Linux/UNIX系統編程手冊（上） [M]. 人民郵電出版社，2014.<br>
[2] Thread-Local Storage http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2659.htm [OL]. Lawrence Crowl, 2008-06-11.<br>
[3] C++ ISO drafts http://www.csci.csusb.edu/dick/c++std/cd2/expr.html

