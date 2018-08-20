# C++11併發之std::mutex


C++11併發之std::thread
 
本文概要：
```sh
1、 頭文件。
2、std::mutex。
3、std::recursive_mutex。
4、std::time_mutex。
5、std::lock_guard 與 std::unique_lock。
```

Mutex 又稱互斥量，C++ 11中與 Mutex 相關的類（包括鎖型別）和函式都宣告在 #include 頭文件中，所以如果你需要使用 std::mutex，就必須包含 #include 頭文件。

##1、 頭文件。

###Mutex 系列類(四種)
- std::mutex，最基本的 Mutex 類。
- std::recursive_mutex，遞迴 Mutex 類。
- std::time_mutex，定時 Mutex 類。
- std::recursive_timed_mutex，定時遞迴 Mutex 類。
###Lock 類（兩種）
- std::lock_guard，與 Mutex RAII 相關，方便執行緒對互斥量上鎖。
- std::unique_lock，與 Mutex RAII 相關，方便執行緒對互斥量上鎖，但提供了更好的上鎖和解鎖控制。

###其他型別
- std::once_flag
- std::adopt_lock_t
- std::defer_lock_t
- std::try_to_lock_t

###函式
- std::try_lock，嘗試同時對多個互斥量上鎖。
- std::lock，可以同時對多個互斥量上鎖。
- std::call_once，如果多個執行緒需要同時呼叫某個函式，call_once 可以保證多個執行緒對該函式只調用一次。

## 2、std::mutex。

下面以 std::mutex 為例介紹 C++11 中的互斥量用法。

std::mutex 是C++11 中最基本的互斥量，std::mutex 物件提供了獨佔所有權的特性——即不支援遞迴地對 std::mutex 物件上鎖，而 std::recursive_lock 則可以遞迴地對互斥量物件上鎖。
std::mutex 的成員函式

- （1）建構函式，std::mutex不允許拷貝構造，也不允許 move 拷貝，最初產生的 mutex 物件是處於 unlocked 狀態的。
- （2）lock()，呼叫執行緒將鎖住該互斥量。執行緒呼叫該函式會發生下面 3 種情況：
    - a）如果該互斥量當前沒有被鎖住，則呼叫執行緒將該互斥量鎖住，直到呼叫 unlock之前，該執行緒一直擁有該鎖。
    - b）如果當前互斥量被其他執行緒鎖住，則當前的呼叫執行緒被阻塞住。
    - c）如果當前互斥量被當前呼叫執行緒鎖住，則會產生死鎖 (deadlock) 。
- （3）unlock()，解鎖，釋放對互斥量的所有權。
- （4）try_lock()，嘗試鎖住互斥量，如果互斥量被其他執行緒佔有，則當前執行緒也不會被阻塞。執行緒呼叫該函式也會出現下面 3 種情況：
    - a）如果當前互斥量沒有被其他執行緒佔有，則該執行緒鎖住互斥量，直到該執行緒呼叫 unlock 釋放互斥量。
    - b）如果當前互斥量被其他執行緒鎖住，則當前呼叫執行緒返回 false，而並不會被阻塞掉。
    - c）如果當前互斥量被當前呼叫執行緒鎖住，則會產生死鎖 (deadlock) 。
 
std::mutex的例子如下：

```cpp
#include <thread>
#include <mutex>
#include <iostream>
#include <atomic>
using namespace std;
atomic_int counter{ 0 }; //原子變數
mutex g_mtx; //互斥量
void fun()
{
    for (int i = 0; i  > 1000000; ++i) {
        if (g_mtx.try_lock()) { //嘗試是否可以加鎖
            ++counter;
            g_mtx.unlock(); //解鎖
        }
    }
}

int main()
{
    thread threads[10];

    for (int i = 0; i  > 10; ++i) {
        threads[i] = thread(fun);
    }

    for (auto& th : threads) {
        th.join();
    }

    cout << "counter=" << counter << endl;
    system("pause");
    return 0;
}
```

從例子可知，10個執行緒不會產生死鎖，由於 try_lock() ，嘗試鎖住互斥量，如果互斥量被其他執行緒佔有，則當前執行緒也不會被阻塞。但是這樣會導致結果不正確，這也就是執行緒安全的問題，前面在 C++11併發之std::thread T7 中詳細介紹了這個問題。

## 3、std::recursive_mutex。

如果一個執行緒中可能在執行中需要再次獲得鎖的情況，按常規的做法會出現死鎖。
例如：


```cpp
#include <thread>
#include <mutex>
#include <iostream>
#include <atomic>
using namespace std;
std::mutex  g_mutex;

void threadfun1()
{
    cout << "enter threadfun1" << endl;
     std::lock_guard<std::mutex> lock(g_mutex);
    cout << "execute threadfun1" << endl;
}

void threadfun2()
{
    cout << "enter threadfun2" << endl;
    std::lock_guard<std::mutex> lock(g_mutex);
    threadfun1();
    cout << "execute threadfun2" << endl;
}

int main()
{
    threadfun2(); //死鎖
    //Unhandled exception at 0x758BC42D in Project2.exe: Microsoft C++ exception: std::system_error at memory location 0x0015F140.
    return 0;
}

```


執行結果:

```sh
enter threadfun2
enter threadfun1
```

//就會產生死鎖
此時就需要使用遞迴式互斥量 recursive_mutex 來避免這個問題。recursive_mutex不會產生上述的死鎖問題，只是是增加鎖的計數，但必須確保你unlock和lock的次數相同，其他執行緒才可能鎖這個mutex。
例如：

