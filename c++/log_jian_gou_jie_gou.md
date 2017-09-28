# LOG 建構解構


```cpp
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>

//#define gettid() syscall(__NR_gettid)

class __CallLog__
{
private:
    const char* logtag;
    const char* file;
    int line;
    const char* func;
    long mEnterTime;
public:
    inline __CallLog__(const char* __logtag, const char* __file, int __line,
                       const char* __func):
        logtag(__logtag), file(__file), line(__line), func(__func),
        mEnterTime(clock())
    {
        printf("%s\n", file);
        ::printf("TAG=%s TID:%d ...%s:%d:\tEnter %s\n",
                 logtag, static_cast<int>(syscall(__NR_gettid)), strrchr(file, '/'), line, func);
    }

    inline void timeDiff(int diffLine)
    {
#if 0
        LOG(LOG_DEBUG, logtag,
            "TID:%d ...%s:%d:\tTime diff from line %d is %lld millis\n",
            static_cast<int32_t>(syscall(__NR_gettid)), strrchr(file, '/'), line, diffLine,
            android::uptimeMillis() - mEnterTime);
#endif
    }

    inline ~__CallLog__()
    {
        ::printf("TAG=%s TID:%d ...%s:%d:\tLeave %s (takes %ld millis)\n",
                 logtag, static_cast<int>(syscall(__NR_gettid)), strrchr(file, '/'), line, func,
                 clock() - mEnterTime);
    }
};

#define GLOGENTRY(args...) __CallLog__ __call_log__(LOG_TAG, __FILE__, __LINE__, __FUNCTION__);
#define GLOGTENTRY(logtag, args...) __CallLog__ __call_log__(logtag, __FILE__, __LINE__, __FUNCTION__);


#define LOG_TAG "YAO"

void test1()
{
    GLOGENTRY();

}

void test()
{
    GLOGENTRY();
    test1();
}

int main(int argc, char* argv[])
{
    GLOGENTRY();
    test();

    return 0;
}
```