# (一、標準庫的線程封裝類Thread和Future）


學習C++11的多線程已經有一段時間了，所以打算整理記錄下來，分成五篇文章：

```
一、標準庫的線程封裝類Thread和Future
二、互斥對象(Mutex)和鎖(Lock)
三、條件變量(Condition Variable)
四、原子操作(Atomic Operation)
五、內存序(Memory Order)
```

##多線程概述

線程(Thread)是程序中獨立執行的指令流，通常是系統調度的基本單位。含有兩個或兩個以上線程的程序就稱為多線程程序，在硬件允許的情況下多個線程可以彼此獨立的並行執行。在許多情況下都需要使用多線程，比如：


- 程序需要進行一些IO操作，IO操作通常需要一些時間才能完成，在這段時間線程就會被阻塞(Block)。使用多線程就可以啟動一個新的線程去等待IO操作完成，而當前線程就可不被阻塞的繼續執行別的工作；

- 在多核心處理器的機器上執行計算密集型的程序。使用多線程就可以將計算工作切分成多個獨立的計算工作交給多個線程在CPU的不同核心上並行計算。
C++11開始提供了對多線程的支持。標準庫提供了std::thread類來創建和管理線程，std::future類模板來獲取異步操作的結果。

###std::thread類

下面的代碼演示了使用std::thread類在主線程中創建了一個線程對象td，創建的線程執行函數thread_proc中的代碼輸出線程id。主線程調用thread對象td的join函數等待線程執行結束。


```cpp
#include <iostream>
#include <thread>

void thread_proc()
{
    std::cout << "thread id = " << std::this_thread::get_id() << std::endl;
}

int main()
{
    std::thread td(
        thread_proc); // 創建thread對象即創建了一個新的線程並執行
    td.join(); // 等待線程執行結束
}
```

###線程的執行體
std::thread的執行體並不要求必須是普通的函數，任何可調用(Callable)的對象都是可以的。舉例如下:

Lambda表達式

```cpp
#include <iostream>
#include <thread>

int main()
{
    std::thread td([](int a, int b) {
        std::cout << a << "+" << b << "=" << a + b << std::endl;
    }, 1, 2); // 使用Lambda表達式創建線程對象並傳遞參數1和2
    td.join(); // 等待線程執行結束
}
```

重載了operator()的類的對象

```cpp
#include <iostream>
#include <thread>

struct functor {
    void operator()(int a, int b)
    {
        std::cout << a << "+" << b << "=" << a + b << std::endl;
    }
};

int main()
{
    // 使用functor對象創建線程對象並傳遞參數1和2
    std::thread td(functor(), 1,2); 
    td.join(); // 等待線程執行結束
}
```


此外還可以使用std::bind表達式和Lambda表達式間接的讓非靜態成員函數做為執行體

`使用std::bind表達式綁定對象和其非靜態成員函數`



```cpp
#include <iostream>
#include <thread>
#include <functional>

class C
{
    int data_;
public:
    C(int data) : data_(data) {}
    void member_func()
    {
        std::cout << "this->data_ = " << this->data_ << std::endl;
    }
};

int main()
{
    C obj(10);
    std::thread td(std::bind(&C::member_func, &obj));
    td.join(); // 等待線程執行結束
}
```

使用Lambda表達式調用對象的非靜態成員函數

```cpp
#include <iostream>
#include <thread>

class C
{
    int data_;
public:
    C(int data) : data_(data) {}
    void member_func()
    {
        std::cout << "this->data_ = " << this->data_ << std::endl;
    }
};

int main()
{
    C obj(10);
    std::thread td([&obj]() {
        obj.member_func();
    });
    td.join(); // 等待線程執行結束
}
```
以上兩段代碼都輸出


```cpp
this->data_ = 10
```

###等待線程執行結束

無論是使用std::bind表達式還是Lambda表達式都需要注意保證obj對象不能在函數執行完成前被銷毀。
需要特別注意的是std::thread對象有個成員函數joinable用於判斷線程對象是否是可以join的。當線程對象被析構的時候如果對象joinable() == true會導致std::terminate()被調用。所以要讓thread對象被正確的析構就需要知道什麼情況下joinable()為false了

- 默認構造函數構造的thread對象

默認構造函數構造的對象不代表任何線程，所以joinable為false。

```cpp
#include <iostream>
#include <thread>
#include <iomanip>

int main()
{
    std::thread td;
    std::cout << "td.joinable() = " << std::boolalpha << td.joinable() << std::endl;
}
```

輸出

```cpp
td.joinable() = false
```

- 調用過join的thread對象

通過對join成員函數的調用可以使joinable為true的thread對象在join返回後變成false。

```cpp
#include <iostream>
#include <thread>
#include <iomanip>

int main()
{
    std::thread td([]() {});
    std::cout << "td.joinable() = " << std::boolalpha << td.joinable() << std::endl;
    td.join();
    std::cout << "td.joinable() = " << std::boolalpha << td.joinable() << std::endl;
}
```

輸出

```cpp
td.joinable() = true
td.joinable() = false
```

- 調用過detach的thread對象

通過對detach成員函數的調用允許線程不再受thread對象管理，所以thread對象的joinable自然變成false了。


```cpp
#include <iostream>
#include <thread>
#include <iomanip>

int main()
{
    std::thread td([]() {});
    std::cout << "td.joinable() = " << std::boolalpha << td.joinable() << std::endl;
    td.detach();
    std::cout << "td.joinable() = " << std::boolalpha << td.joinable() << std::endl;
}
```

輸出

```cpp
td.joinable() = true
td.joinable() = false
```

- 其他各種原因不再擁有線程所有權的情況

如以`移動語義(move semantic)`構造或複製給另一個thread對象，與joinable為false的對象通過std::swap交換等。


###線程管理函數
除了std::thread的成員函數外在std::this_thread命名空間中也定義了一系列函數用於管理當前線程。

<table>
<thead>
<tr>
<th>函數名</th>
<th>說明</th>
</tr>
</thead>
<tbody>
<tr>
<td>yield</td>
<td>建議線程調度者執行其他線程，相當於主動讓出剩下的執行時間，具體調度算法取決於實現。</td>
</tr>
<tr>
<td>get_id</td>
<td>獲取當前線程的線程id。</td>
</tr>
<tr>
<td>sleep_for</td>
<td>指定的一段時間內停止當前線程的執行。</td>
</tr>
<tr>
<td>sleep_until</td>
<td>停止當前線程的執行直到指定的時間點。</td>
</tr>
</tbody>
</table>

### std::future類模板

std::future類模板是標準庫提供的一種用於獲取異步操作的結果的機制。前面的演示代碼中線程執行函數體都沒有返回值，事實上std::thread的線程執行函數是可以有返回值的，但是其返回值會被忽略。此外使用std::future還可以延遲異步操作中異常(Exception)的拋出。下面的代碼演示了通過std::async啟動一個異步操作，並通過std::future::get取得返回值和捕獲異步操作拋出的異常。

```cpp
#include <iostream>
#include <future>
#include <thread>
#include <cstdint>
#include <stdexcept>
#include <limits>

std::uint32_t add(std::uint32_t a, std::uint32_t b)
{
    std::cout << "thread id = " << std::this_thread::get_id() << ", a = " << a <<
              ", b = " << b << std::endl;

    if (std::numeric_limits<std::uint32_t>::max() - a < b) {
        throw std::overflow_error("overflow_error");
    }

    return a + b;
}

int main()
{
    // 使用std::async啟動異步操作並返回std::future對象
    std::future<std::uint32_t> f1 = std::async(std::launch::async, add, 1ul, 2ul);
    // 通過std::future::get等待異步操作完成並取得返回值
    std::uint32_t sum1 = f1.get();
    std::cout << "thread id = " << std::this_thread::get_id() << ", sum1 = " << sum1
              << std::endl;
    // 4000000000ul + 4000000000ul會拋出異常，異常會被延遲到std::future::get或std::future::wait時拋出
    std::future<std::uint32_t> f2 = std::async(std::launch::async, add,
                                    4000000000ul, 4000000000ul);

    try {
        std::uint32_t sum2 = f2.get();
        std::cout << "thread id = " << std::this_thread::get_id() << ", sum2 = " << sum2
                  << std::endl;
    } catch (const std::overflow_error& e) {
        std::cout << "thread id = " << std::this_thread::get_id() << ", e.what() = " <<
                  e.what() << std::endl;
    }
}
```

輸出

```sh
thread id = 2, a = 1, b = 2
thread id = 1, sum1 = 3
thread id = 3, a = 4000000000, b = 4000000000
thread id = 1, e.what() = overflow_error
```


`使用std::future獲取std::thread對象創建線程異步操作的結果`

使用std::future獲取std::thread對象創建線程異步操作的結果有兩種方法。

`使用std::packaged_task類模板`

使用std::packaged_task類模板來使std::future能夠獲取std::thread對象創建線程的異步操作結果的步驟如下。

- 使用std::packaged_task包裝線程執行函數獲得一個std::packaged_task對象，該對象會處理被包裝函數的返回值和異常；
- 通過這個std::packaged_task對象獲取其關聯的std::future對象，用於獲取異步操作的結果；
- 將std::packaged_task對象做為std::thread對象的線程執行函數，啟動線程；
- 通過std::future對象獲取其返回值。

演示代碼如下

```cpp
#include <iostream>
#include <future>
#include <thread>
#include <cstdint>
#include <stdexcept>
#include <limits>
#include <utility>

std::uint32_t add(std::uint32_t a, std::uint32_t b)
{
    std::cout << "thread id = " << std::this_thread::get_id() << ", a = " << a <<
              ", b = " << b << std::endl;

    if (std::numeric_limits<std::uint32_t>::max() - a < b) {
        throw std::overflow_error("overflow_error");
    }

    return a + b;
}

int main()
{
    // 使用std::packaged_task包裝add函數
    std::packaged_task<std::uint32_t(std::uint32_t, std::uint32_t)> task1(add);
    // 取得std::future用於獲取異步操作的結果
    std::future<std::uint32_t> f1 = task1.get_future();
    // 將task1對象作為線程的函數執行體
    std::thread(std::move(task1), 1ul, 2ul).detach();
    // 通過std::future對象獲取異步操作的結果
    std::uint32_t sum1 = f1.get();
    std::cout << "thread id = " << std::this_thread::get_id() << ", sum1 = " << sum1
              << std::endl;
    std::packaged_task<std::uint32_t(std::uint32_t, std::uint32_t)> task2(add);
    std::future<std::uint32_t> f2 = task2.get_future();
    // 4000000000ul + 4000000000ul會拋出異常
    std::thread(std::move(task2), 4000000000ul, 4000000000ul).detach();

    try {
        std::uint32_t sum2 = f2.get();
        std::cout << "thread id = " << std::this_thread::get_id() << ", sum2 = " << sum2
                  << std::endl;
    } catch (const std::overflow_error& e) {
        std::cout << "thread id = " << std::this_thread::get_id() << ", e.what() = " <<
                  e.what() << std::endl;
    }
}

```


輸出

```sh
thread id = 2, a = 1, b = 2
thread id = 1, sum1 = 3
thread id = 3, a = 4000000000, b = 4000000000
thread id = 1, e.what() = overflow_error
```

###使用std::promise類模板

使用std::promise類模板來使std::future能夠獲取std::thread對象創建線程的異步操作結果的步驟如下。

- 創建一個std::promise對象；
- 獲取該std::promise對象的關聯的std::future對象；
- 使用std::thread創建線程並將std::promise對象傳進去；
- 線程執行函數內部通過std::promise的set_value、set_value_at_thread_exit、set_exception或set_exception_at_thread_exit設置值或異常供std::future對象獲取；
- 使用std::future對象等待並獲取異步操作的結果。

演示代碼如下


```cpp
#include <iostream>
#include <future>
#include <thread>
#include <cstdint>
#include <stdexcept>
#include <limits>
#include <utility>
#include <exception>

std::uint32_t add(std::uint32_t a, std::uint32_t b)
{
    std::cout << "thread id = " << std::this_thread::get_id() << ", a = " << a <<
              ", b = " << b << std::endl;

    if (std::numeric_limits<std::uint32_t>::max() - a < b) {
        throw std::overflow_error("overflow_error");
    }

    return a + b;
}

// add函數的包裝函數
void add_wrapper(std::promise<std::uint32_t> promise, std::uint32_t a,
                 std::uint32_t b)
{
    try {
        // 設置值為供std::future對象獲取
        promise.set_value(add(a, b));
    } catch (...) {
        // 設置異常在std::future獲取值時拋出
        promise.set_exception(std::current_exception());
    }
}

int main()
{
    // 創建std::promise對象
    std::promise<std::uint32_t> promise1;
    // 獲取關聯的std::future對象
    std::future<std::uint32_t> f1 = promise1.get_future();
    // 啟動線程執行add函數的包裝函數
    std::thread(add_wrapper, std::move(promise1), 1ul, 2ul).detach();
    // 等待並獲取異步操作的結果
    std::uint32_t sum1 = f1.get();
    std::cout << "thread id = " << std::this_thread::get_id() << ", sum1 = " << sum1
              << std::endl;
    std::promise<std::uint32_t> promise2;
    std::future<std::uint32_t> f2 = promise2.get_future();
    // 4000000000ul + 4000000000ul會拋出異常
    std::thread(add_wrapper, std::move(promise2), 4000000000ul,
                4000000000ul).detach();

    try {
        std::uint32_t sum2 = f2.get();
        std::cout << "thread id = " << std::this_thread::get_id() << ", sum2 = " << sum2
                  << std::endl;
    } catch (const std::overflow_error& e) {
        std::cout << "thread id = " << std::this_thread::get_id() << ", e.what() = " <<
                  e.what() << std::endl;
    }
}
```
輸出

```sh
thread id = 2, a = 1, b = 2
thread id = 1, sum1 = 3
thread id = 3, a = 4000000000, b = 4000000000
thread id = 1, e.what() = overflow_error
```

從實現的角度來看，std::async內部可以使用std::packaged_task來實現，而std::packaged_task內部可以使用std::promise來實現。


## 最後

本文是《C++11之多線程》系列的第一篇，僅僅簡單的介紹了如何使用標準庫創建線程和獲取異步操作的結果。在下一篇中主要會講C++11中的多線程同步。