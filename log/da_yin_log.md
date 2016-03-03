# 打印log

```c
fprintf (stderr, "YAO This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __FUNCTION__);

```


- 減少log數次

```c
unsigned int count = 0;
if (count % 10 == 0) {
   fprintf (stderr, "YAO This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __FUNCTION__);
    count++;
}
    
```

- kernel 

```c
dump_stack()


dump_stack不準確的原因分析

kernel panic後打印的堆棧信息是調用dump_stack函數獲得的。而dump_stack的原理是遍歷堆棧，把所有可能是內核函數的內容找出來，並打印對應的函數。因為函數調用時會把下一條指令的地址放到堆棧中。所以只要找到這些return address，就可以找到這些return address所在函數，進而打印函數的調用關係。 
     但是dump_stack可能不準確，可能的原因有三： 
     1.所有這些可以找到的函數地址，存在/proc/kallsyms中。它並不包括內核中所有的函數，而只包括內核中stext~etext和sinittext~einittext範圍的函數，及模塊中的函數。詳細可參考scripts/kallsyms.c 
     2.一些函數在編譯時進行了優化，把call指令優化為jmp指令，這樣在調用時就不會把return address放到堆棧，導致dump_stack時在堆棧中找不到對應的信息。 
     3.堆棧中可能有一些數值，它們不是return address，但是在內核函數地址的範圍裡，這些數值會被誤認為return address從而打印出錯誤的調用關係。 
     
```


```py
#! /usr/bin/python
# -*- coding: utf-8 -*-
import os
import sys

def InsertToFunc(lines, funcLines, FileName):
    #print lines
    #print funcLines

    for i in funcLines:
        for j in range(i-1, len(lines)):
            if str(lines[j]).find(';') != -1:
                break

            if str(lines[j]).find('{') != -1:
                if str(lines[j]).find('}') != -1:
                    break

                if str(FileName).find('.java') != -1:
                    lines.insert(j+1, '\tSlog.d(Thread.currentThread().getStackTrace()[2].getClassName(), "YAO ["+ Thread.currentThread().getStackTrace()[2].getMethodName() +" | "+Thread.currentThread().getStackTrace()[2].getFileName()+":"+Thread.currentThread().getStackTrace()[2].getLineNumber()+"]");\n');
                elif str(FileName).find('.cpp') != -1 or str(FileName).find('.c') != -1:
                    #lines.insert(j+1, '\t::printf ("This is line %d of file %s (function %s)\\n", __LINE__, __FILE__, __func__);\n')
                    lines.insert(j+1, '\tALOGW("YAO This is line %d of file %s (function %s)\\n", __LINE__, __FILE__, __func__);\n')

                break
    return lines

def main():
    FileName = sys.argv[1]
    print FileName
 
    if str(FileName).find('.java') != -1:
        os.system("ctags-exuberant -x " + FileName + " | ack -o -w 'method\s+.*' | ack -o '\d+\s+.*' | ack -o '^\d+\s+' | sort -k 1 -nr > /tmp/test.txt")
    elif str(FileName).find('.cpp') != -1 or str(FileName).find('.c') != -1:
        os.system("ctags-exuberant -x " + FileName + " | ack -o -w 'function\s+.*' | ack -o '\d+\s+.*' | ack -o '^\d+\s+' | sort -k 1 -nr > /tmp/test.txt")
    else:
        print 'noknow type'
        sys.exit(0)

    f = open('/tmp/test.txt', 'r+')
    #funcLines = map(int, funcLines)
    funcLines = [int(i)for i in f.read().splitlines()]
    f.close()

    f = open(str(FileName), 'r+')
    lines = [i for i in f.read().splitlines()]
    f.close()

    InsertListfinish = InsertToFunc(lines, funcLines, FileName)

    newfile = open(str(FileName), "w+")
    newfile.truncate()

    for l in InsertListfinish:
        newfile.write(l + '\n')

    newfile.close()

if __name__=='__main__':
    if len(sys.argv) > 1:
        main()
    else:
        pass
        print "please input python test.py filename"


```


- linux getpid gettid


```c
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h> 

#define gettid() syscall(__NR_gettid)  

typedef void* (*start_routine_t)(void*);

void* someThread(void* tmp)
{
    printf("PID of this process: %d\n", getpid());
    // printf("The ID of this thread is: %u\n", (unsigned int)pthread_self());

    // /usr/include/asm-i386/unistd.h 
    printf("The ID of this of this thread is: %ld\n", gettid());
    sleep(2); // Keep it alive so we're sure the second thread gets a unique ID.
    return NULL;
}


int main(int argc, char** argv)
{
    printf("PID of this process: %d\n", getpid());
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, someThread, NULL);
    sleep(1); // Hack to avoid race for stdout.
    pthread_create(&thread2, NULL, someThread, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return (0);
}
```

```c
gcc -Wall -g -pthread test.c -o test
./test
```

- linux c++ constructor deconstructor log

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
        mEnterTime(clock()) {
            printf("%s\n", file);
        ::printf("TAG=%s TID:%d ...%s:%d:\tEnter %s\n",
                 logtag, static_cast<int>(syscall(__NR_gettid)), strrchr(file, '/'), line, func);
    }

    inline void timeDiff(int diffLine) {
#if 0
        LOG(LOG_DEBUG, logtag,
            "TID:%d ...%s:%d:\tTime diff from line %d is %lld millis\n",
            static_cast<int32_t>(syscall(__NR_gettid)), strrchr(file, '/'), line, diffLine,
            android::uptimeMillis() - mEnterTime);
#endif
    }

    inline ~__CallLog__() {
        ::printf("TAG=%s TID:%d ...%s:%d:\tLeave %s (takes %ld millis)\n",
                 logtag, static_cast<int>(syscall(__NR_gettid)), strrchr(file, '/'), line, func, clock() - mEnterTime);
    }
};

#define GLOGENTRY(args...) __CallLog__ __call_log__(LOG_TAG, __FILE__, __LINE__, __FUNCTION__);
#define GLOGTENTRY(logtag, args...) __CallLog__ __call_log__(logtag, __FILE__, __LINE__, __FUNCTION__);


#define LOG_TAG "YAO"
int main(int argc, char* argv[])
{
    GLOGENTRY();

    return 0;
}
```


- windows 印出 pid tid , 需要裝 pthread lib


```c
#include <process.h>
printf("PID =%d, TID=%d\n", _getpid(), pthread_self());
```

### Windows下如何用C語言打印當前進程的進程號？

```c
#include <windows.h> // for GetCurrentProcessId  

std::cout << "!!!! imuPath=" << m_imuPath
		<< " pid=" << GetCurrentProcessId()
		<< " tid=" << GetCurrentThreadId() << std::endl;
```

