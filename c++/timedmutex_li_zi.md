# timed_mutex 例子

```cpp
#define _GNU_SOURCE
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <sstream>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>

std::timed_mutex mutex;

#define GetProcessInfo(s) do { sprintf(s, "%d %d  %s:%d %s", getpid(), gettid(), __FILE__, __LINE__, __func__);} while (0)

pid_t gettid()
{
    pid_t tid;
    tid = syscall(SYS_gettid);
    return tid;
}

void job(int id)
{
    char s[100];
    using Ms = std::chrono::milliseconds;

    {
        // std::unique_lock<std::timed_mutex> lck(mutex, std::defer_lock);
        GetProcessInfo(s);
        std::cout << "\n" << s << " [++]" << std::endl;
        std::unique_lock<std::timed_mutex> lck(mutex, std::defer_lock);
        // 5秒內沒有 unlock 就放掉
        lck.try_lock_for(Ms(5000));
        GetProcessInfo(s);
        std::cout << s << " [+]" << std::endl;
        std::this_thread::sleep_for(Ms(10000));
        std::cout << s << " [-]" << std::endl;
    }
    //lck.unlock();
}

int main()
{
    std::vector<std::thread> threads;

    for (int i = 0; i < 2; ++i) {
        threads.emplace_back(job, i);
    }

    for (auto& i : threads) {
        i.join();
    }
}
```