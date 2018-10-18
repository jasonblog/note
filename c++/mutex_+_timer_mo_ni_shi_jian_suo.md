# Mutex + Timer 模擬時間鎖


```cpp
#define _GNU_SOURCE
#include <iostream>
#include <assert.h>
#include <sys/time.h>
#include <signal.h>
#include <linux/types.h>
#include <sched.h>
#include <pthread.h>
#include <thread>
#include <unistd.h>
#include <sys/wait.h>
#include <mutex>
#include <time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <vector>

using namespace std;

#define GetProcessInfo(s) do { sprintf(s, "%d %d  %s:%d %s", getpid(), gettid(), __FILE__, __LINE__, __func__);} while (0)

pid_t gettid()
{
    pid_t tid;
    tid = syscall(SYS_gettid);
    return tid;
}

volatile static long counter = 0;

typedef void(*ProcessHandler)(int);

//互斥鎖
class MutexLock
{
public:
    MutexLock()
    {
        this_ = this;
        pthread_mutex_init(&m_stMutex, NULL);
    }
    ~MutexLock()
    {
        pthread_mutex_destroy(&m_stMutex);
    }

    void lock()
    {
        pthread_mutex_lock(&m_stMutex);
    }
    int unlock()
    {
        return pthread_mutex_unlock(&m_stMutex);
    }

    bool trylock()
    {
        return pthread_mutex_trylock(&m_stMutex) == 0;
    }

    void static timeout_cb(int sig);

    void static set_timer();

    static MutexLock* this_;

    std::mutex sendcmdmutex_;

private:
    pthread_mutex_t m_stMutex;
};

MutexLock* MutexLock::this_ = nullptr;

void MutexLock::timeout_cb(int sig)
{
    char s[100];
    GetProcessInfo(s);
    this_->sendcmdmutex_.unlock();
    std::cout << "\nunlock !!!" << s << std::endl;
}

class Timer
{
protected:
    struct sigaction  _act;
    struct sigaction  _oldact;
    struct itimerval  _tim_ticks;

    void wait(long timeout_ms)
    {
        struct timespec spec;
        spec.tv_sec  = timeout_ms / 1000;
        spec.tv_nsec = (timeout_ms % 1000) * 1000000;
        nanosleep(&spec, NULL);
    }

public:
    Timer(int sec, int usec)
    {
        //結構成員it_value指定首次定時的時間，
        // 結構成員it_interval指定下次定時的時間
        // 定時器工作時，先將it_value的時間值減到0，
        // 發送一個信號，再將it_value賦值為it_interval的值，重新開始定時，如此反覆。
        _tim_ticks.it_value.tv_sec = sec;
        _tim_ticks.it_value.tv_usec = usec;
        _tim_ticks.it_interval.tv_sec = 0;
        _tim_ticks.it_interval.tv_usec = 0;
    }

    void setHandler(ProcessHandler handler)
    {
        sigemptyset(&_act.sa_mask);
        _act.sa_flags = 0;
        _act.sa_handler = handler;
    }

    void cancel()
    {
        _tim_ticks.it_value.tv_sec = 0;
        _tim_ticks.it_value.tv_usec = 0;
        _tim_ticks.it_interval = _tim_ticks.it_value;
        setitimer(ITIMER_REAL, &_tim_ticks, NULL);
    }

    bool reset()
    {
#if 1
        int res = sigaction(SIGALRM, &_act, &_oldact);

        if (res) {
            perror("Fail to install handle: ");
            return false;
        }

        res = setitimer(ITIMER_REAL, &_tim_ticks, 0);

        if (res) {
            perror("Fail to set timer: ");
            // sigaction(SIGALRM, &_oldact, 0);
            return false;
        }

        return true;
#else
        int res = sigaction(SIGALRM, &_act, &_oldact);

        if (res) {
            perror("Fail to install handle: ");
            return false;
        }

        res = setitimer(ITIMER_REAL, &_tim_ticks, 0);

        if (res) {
            perror("Fail to set timer: ");
            sigaction(SIGALRM, &_oldact, 0);
            return false;
        }

        for (; ;) {
            wait(1000000);
        }

#endif
    }
};

void job(int id)
{
    char s[100];
    GetProcessInfo(s);
    using Ms = std::chrono::milliseconds;
    std::cout << "lock [+]" << s << std::endl;
    MutexLock::this_->sendcmdmutex_.lock();
    srand(time(NULL));
    int ms = rand() % 10000;
    std::cout << "lock [-]" << s << ", " << ms << " ms" << std::endl;
    Timer timer(5, 0);
    timer.setHandler(MutexLock::timeout_cb);
    timer.reset();
    std::this_thread::sleep_for(Ms(ms));
    MutexLock::this_->sendcmdmutex_.unlock();
    timer.cancel();
    std::cout << "unlock " << s << std::endl;
}

int main(void)
{
    MutexLock m;
    std::vector<std::thread> threads;

    for (int i = 0; i < 2; ++i) {
        threads.emplace_back(job, i);
    }

    for (auto& i : threads) {
        i.join();
    }

    return 0;
}
```
