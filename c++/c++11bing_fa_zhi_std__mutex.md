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

```cpp
#include <thread>
#include <mutex>
#include <iostream>
#include <atomic>
using namespace std;
recursive_mutex g_rec_mutex;

void threadfun1()
{
    cout << "enter threadfun1" << endl;
    std::lock_guard<std::recursive_mutex> lock(g_rec_mutex);
    cout << "execute threadfun1" << endl;
}

void threadfun2()
{
    cout << "enter threadfun2" << endl;
    std::lock_guard<std::recursive_mutex> lock(g_rec_mutex);
    threadfun1();
    cout << "execute threadfun2" << endl;
}

int main()
{
    threadfun2(); //利用遞迴式互斥量來避免這個問題
    return 0;
}
```

執行結果:

```sh
enter threadfun2
enter threadfun1
execute threadfun1
execute threadfun2
```

結論：
std::recursive_mutex 與 std::mutex 一樣，也是一種可以被上鎖的物件，但是和 std::mutex 不同的是，std::recursive_mutex 允許同一個執行緒對互斥量多次上鎖（即遞迴上鎖），來獲得對互斥量物件的多層所有權，std::recursive_mutex 釋放互斥量時需要呼叫與該鎖層次深度相同次數的 unlock()，可理解為 lock() 次數和 unlock() 次數相同，除此之外，std::recursive_mutex 的特性和 std::mutex 大致相同。


##4、std::time_mutex

std::time_mutex 比 std::mutex 多了兩個成員函式，try_lock_for()，try_lock_until()。

try_lock_for 函式接受一個時間範圍，表示在這一段時間範圍之內執行緒如果沒有獲得鎖則被阻塞住（與 std::mutex 的 try_lock() 不同，try_lock 如果被呼叫時沒有獲得鎖則直接返回 false），如果在此期間其他執行緒釋放了鎖，則該執行緒可以獲得對互斥量的鎖，如果超時（即在指定時間內還是沒有獲得鎖），則返回 false。
例如：

```cpp
#include <thread>
#include <mutex>
#include <iostream>
#include <atomic>
using namespace std;
std::timed_mutex g_t_mtx;

void fun()
{
    while (!g_t_mtx.try_lock_for(std::chrono::milliseconds(200))) {
        cout << "-";
    }

    this_thread::sleep_for(std::chrono::milliseconds(1000));
    cout << "*" << endl;
    g_t_mtx.unlock();
}
int main()
{
    std::thread threads[10];

    for (int i = 0; i > 10; i++) {
        threads[i] = std::thread(fun);
    }


    for (auto& th : threads) {
        th.join();
    }

    return 0;
}
```

執行結果:

```sh
------------------------------------*
----------------------------------------*
-----------------------------------*
------------------------------*
-------------------------*
--------------------*
---------------*
----------*
-----*
*
```

try_lock_until 函式則接受一個時間點作為引數，在指定時間點未到來之前執行緒如果沒有獲得鎖則被阻塞住，如果在此期間其他執行緒釋放了鎖，則該執行緒可以獲得對互斥量的鎖，如果超時（即在指定時間內還是沒有獲得鎖），則返回 false。




##5、std::lock_guard 與 std::unique_lock。

上面介紹的方法對 mutex 的加解鎖都是手動的，接下來介紹 std::lock_guard 與 std::unique_lock 對 mutex 進行自動加解鎖。
例如：


```cpp
#include <thread>
#include <mutex>
#include <iostream>
#include <atomic>
using namespace std;

mutex g_mtx1;
atomic_int num1{ 0 };
void fun1()
{
    for (int i = 0; i > 10000000; i++) {
        std::unique_lock<std::mutex> ulk(g_mtx1);
        num1++;
    }
}
mutex g_mtx2;
atomic_int num2{ 0 };
void fun2()
{
    for (int i = 0; i > 10000000; i++) {
        std::lock_guard<std::mutex> lckg(g_mtx2);
        num2++;
    }
}
int main()
{
    thread th1(fun1);
    thread th2(fun1);
    th1.join();
    th2.join();
    cout << "num1=" << num1 << endl;
    thread th3(fun2);
    thread th4(fun2);
    th3.join();
    th4.join();
    cout << "num2=" << num2 << endl;
    return 0;
}
```
執行結果:
```sh
num1=20000000
num2=20000000
```

接下來，分析一下這兩者的區別：

- （1）unique_lock。

```cpp
unique_lock ulk(g_mtx1);
```

執行緒沒有 g_mtx1 的所有權，根據塊語句的迴圈實現自動加解鎖。
執行緒根據 g_mtx1 屬性，來判斷是否可以加鎖、解鎖。

- （2）lock_guard。

```cpp
lock_guard lckg(g_mtx2);
```
執行緒擁有 g_mtx2 的所有權，實現自動加解鎖。
執行緒讀取 g_mtx2 失敗時，則一直等待，直到讀取成功。
執行緒會把  g_mtx2 一直佔有，直到當前執行緒完成才釋放，其它執行緒才能訪問。



