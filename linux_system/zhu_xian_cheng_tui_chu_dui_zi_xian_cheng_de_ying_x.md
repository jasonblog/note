# 主線程退出對子線程的影響


對於程序來說，如果主進程在子進程還未結束時就已經退出，那麼Linux內核會將子進程的父進程ID改為1（也就是init進程），當子進程結束後會由init進程來回收該子進程。

那如果是把進程換成線程的話，會怎麼樣呢？假設主線程在子線程結束前就已經退出，子線程會發生什麼？

在一些論壇上看到許多人說子線程也會跟著退出，其實這是錯誤的，原因在於他們混淆了線程退出和進程退出概念。實際的答案是主線程退出後子線程的狀態依賴於它所在的進程，如果進程沒有退出的話子線程依然正常運轉。如果進程退出了，那麼它所有的線程都會退出，所以子線程也就退出了。


## 主線程先退出


先來看一個主線程先退出的例子：

```cpp
#include <pthread.h>
#include <unistd.h>

#include <stdio.h>

void* func(void* arg)
{
    pthread_t main_tid = *static_cast<pthread_t*>(arg);
    pthread_cancel(main_tid);
    while (true)
    {
        printf("child loops\n");
		sleep(1);
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    pthread_t main_tid = pthread_self();
    pthread_t tid = 0;
    pthread_create(&tid, NULL, func, &main_tid);
    while (true)
    {
        printf("main loops\n");
    }
    sleep(1);
    printf("main exit\n");
    return 0;
}
```

把主線程的線程號傳給子線程，在子線程中通過`pthread_cancel`終止主線程使其退出。運行程序，可以發現在打印了一定數量的「main loops」之後程序就掛起了，但卻沒有退出。

主線程因為被子線程終止了，所有沒有看到「main exit」的打印。子線程終止了主線程後進入了死循環while中，所以程序看起來像掛起了。如果我們讓子進程while循環中的打印語句生效再運行就可以發現程序會一直打印「child loops」字樣。

主線程被子線程終止了，但他們所依賴的進程並沒有退出，所以子線程依然正常運轉。


## 主線程隨進程一起退出

之前看到一些人說如果主線程先退出了，子線程也會跟著退出，其實他們混淆了線程退出和進程退出的概念。下面這個例子代表了他們的觀點：


```cpp
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

void* func(void* arg)
{
    while (true)
    {
        printf("child loops\n");
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    pthread_t main_tid = pthread_self();
    pthread_t tid = 0;
    pthread_create(&tid, NULL, func, &main_tid);
    sleep(1);
    printf("main exit\n");
    return 0;
}
```

運行上面的代碼，會發現程序在打印一定數量的「child loops」和一句「main exit」之後退出，並且在退出之前的最後一句打印是「main exit」。

按照他們的邏輯，你看，因為主線程在打印完「main exit」後退出了，然後子線程也跟著退出了，所以隨後就沒有子線程的打印了。

但其實這裡是混淆了進程退出和線程退出的概念了。實際的情況是主線程中的main函數執行完ruturn後彈棧，然後調用glibc庫函數exit，exit進行相關清理工作後調用_exit系統調用退出該進程。所以，這種情況實際上是因為進程運行完畢退出導致所有的線程也都跟著退出了，並非是因為主線程的退出導致子線程也退出。


## Linux線程模型

實際上，posix線程和一般的進程不同，在概念上沒有主線程和子線程之分（雖然在實際實現上還是有一些區分），如果仔細觀察apue或者unp等書會發現基本看不到「主線程」或者「子線程」等詞語，在csapp中甚至都是用「對等線程」一詞來描述線程間的關係。

在Linux 2.6以後的posix線程都是由用戶態的pthread庫來實現的。在使用pthread庫以後，在用戶視角看來，每一個tast_struct就對應一個線程（tast_struct原本是內核對應一個進程的結構），而一組線程以及他們所共同引用的一組資源就是進程。從Linux 2.6開始，內核有了線程組的概念，tast_struct結構中增加了一個tgid（thread group id）字段。getpid（獲取進程號）通過系統調用返回的也是tast_struct中的tgid，所以tgid其實就是進程號。而tast_struct中的線程號pid字段則由系統調用syscall(SYS_gettid)來獲取。

當線程收到一個kill致命信號時，內核會將處理動作施加到整個線程組上。為了應付「發送給進程的信號」和「發送給線程的信號」，tast_struct裡面維護了兩套signal_pending，一套是線程組共用的，一套是線程獨有的。通過kill發送的致命信號被放在線程組共享的signal_pending中，可以任意由一個線程來處理。而通過pthread_kill發送的信號被放在線程獨有的signal_pending中，只能由本線程來處理。

關於線程與信號，apue有這麼幾句：

```
每個線程都有自己的信號屏蔽字，但是信號的處理是進程中所有線程共享的。
這意味著儘管單個線程可以阻止某些信號，但當線程修改了與某個信號相關的處理行為以後，
所有的線程都必須共享這個處理行為的改變。這樣如果一個線程選擇忽略某個信號，
而其他的線程可以恢覆信號的默認處理行為，或者是為信號設置一個新的處理程序，
從而可以撤銷上述線程的信號選擇。 

如果信號的默認處理動作是終止該進程，那麼把信號傳遞給某個線程仍然會殺掉整個進程。
```



例如一個程序a.out創建了一個子線程，假設主線程的線程號為9601，子線程的線程號為9602（它們的tgid都是9601），因為默認沒有設置信號處理程序，所以如果運行命令kill 9602的話，是可以把9601和9602這個兩個線程一起殺死的。如果不知道Linux線程背後的故事，可能就會覺得遇到靈異事件了。

另外系統調用syscall(SYS_gettid)獲取的線程號與pthread_self獲取的線程號是不同的，pthread_self獲取的線程號僅僅在線程所依賴的進程內部唯一，在pthread_self的man page中有這樣一段話：


```sh
Thread IDs are guaranteed to be unique only within a process. 
A thread ID may be reused after a terminated thread has been joined, 
or a detached thread has terminated.
```

所以在內核中唯一標識線程ID的線程號只能通過系統調用syscall(SYS_gettid)獲取。

##參考文獻

- W.Richard Stevens. UNIX環境高級編程(第3版), 人民郵電出版社, 2014
- Linux man page. pthread_self(3)