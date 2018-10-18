# (三、條件變量）


## 條件變量(Condition Variable)

`條件變量`是一種同步原語（Synchronization Primitive）用於多線程之間的通信，它可以阻塞一個或同時阻塞多個線程直到：

- 收到來自其他線程的通知
- 超時
- 發生虛假喚醒(Spurious Wakeup)

C++11為條件變量提供了兩個類

- std::condition_variable：必須與std::unique_lock配合使用
- std::condition_variable_any：更加通用的條件變量，可以與任意類型的鎖配合使用，相比前者使用時會有額外的開銷

二者具有相同的成員函數

<table>
<thead>
<tr>
<th>成員函數</th>
<th>說明</th>
</tr>
</thead>
<tbody>
<tr>
<td>notify_one</td>
<td>通知一個等待線程</td>
</tr>
<tr>
<td>notify_all</td>
<td>通知全部等待線程</td>
</tr>
<tr>
<td>wait</td>
<td>阻塞當前線程直到被喚醒</td>
</tr>
<tr>
<td>wait_for</td>
<td>阻塞當前線程直到被喚醒或超過指定的等待時間（長度）</td>
</tr>
<tr>
<td>wait_until</td>
<td>阻塞當前線程直到被喚醒或到達指定的時間（點）</td>
</tr>
</tbody>
</table>

###二者在線程要等待條件變量前都必須要獲得相應的鎖

##條件變量為什麼叫條件變量？

- 條件變量存在虛假喚醒的情況，因此在線程被喚醒後需要檢查條件是否滿足
- 無論是notify_one或notify_all都是類似於發出脈衝信號，如果對wait的調用發生在notify之後是不會被喚醒的，所以接收者在使用wait等待之前也需要檢查條件（標識）是否滿足，另一個線程（通知者）在nofity前需要修改相應標識供接收者檢查

條件變量因此得名。

##為什麼條件變量需要和鎖一起使用？
觀察std::condition_variable::wait函數，發現它的兩個重載都必須將鎖作為參數


```cpp
void wait(std::unique_lock<std::mutex>& lock);
template< class Predicate >
void wait(std::unique_lock<std::mutex>& lock, Predicate pred);
```

首先考慮wait函數不需要鎖作為參數的情況，下面的代碼中flag初始化為false，線程A將flag置為true並使用notify_one發出通知，線程B使用while循環在wait前後都會檢查flag，直到flag被置為true才往下執行。

```cpp
// Thread A
{
    std::unique_lock lck(mt);
    flag = true;
}
cv.notify_one();

// Thread B
auto pred = []()
{
    std::unique_lock lck(mt);
    return flag;
};

while (!pred())
{
    cv.wait();
}
```

如果兩個線程的執行順序為：

- 線程B檢查flag發現其值為false
- 線程A將flag置為true
- 線程A使用notify_one發出通知
- 線程B使用wait進行等待

那麼線程B將不會被喚醒（即線程B沒有察覺到線程A發出的通知），這顯然不是程序員想要的結果，發生這種情況的根源在於線程B對條件的檢查和進入等待的中間是有空檔的。wait函數需要鎖作為參數正是為瞭解決這一問題的。

```cpp
// Thread B
auto pred = []()
{
    return flag;
};
std::unique_lock lck(mt);

while (!pred())
{
    cv.wait(lck);
}
```

當線程B調用wait的時候會釋放傳入的鎖並同時進入等待，當被喚醒時會重新獲得鎖，因此只要保證線程A在修改flag的時候是正確加鎖的那麼就不會發生前面的這種情況。
使用wait函數的另一個重載時下面的代碼與上面的6~8行是等價的。

```cpp
cv.wait(lck, pred);
```

不僅僅是C++，就博主所知道的語言但凡有條件變量的概念都必須與鎖配合使用。以C#、Java為例


- C#

```cpp
// Thread A
lock (obj)
{
    flag = true;
    System.Threading.Monitor.Pulse(obj);
}

// Thread B
lock (obj)
{
    while(!pred())
    {
        System.Threading.Monitor.Wait(obj);
    }
}
```


- Java

```java
// Thread A
synchronized(obj) {
    flag = true;
    obj.notify();
}

// Thread B
synchronized(obj) {
    while(!pred()) {
        obj.wait();
    }
}
```

C#與C++不同之處在於C#在Pulse或PulseAll的線程必須持有鎖，而C++的notify_one和notify_all則無所謂是否持有鎖。
