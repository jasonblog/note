# Pros and Cons

## 未物件封裝版本
```cpp
#include <pthread.h>
#include <queue>
#include <stdio.h>
#include <unistd.h>

// 注意pthread_*函數返回的異常值，為了簡單（偷懶），我沒有去處理它們

pthread_mutex_t mutex;
pthread_cond_t condvar;

std::queue<int> msgQueue;
struct Produce_range {
    int start;
    int end;
};

void *producer(void *args)
{
    int start = static_cast<Produce_range *>(args)->start;
    int end = static_cast<Produce_range *>(args)->end;
    for (int x = start; x < end; x++) {
        usleep(200 * 1000);
        pthread_mutex_lock(&mutex);
        msgQueue.push(x);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&condvar);
        printf("Produce message %d\n", x);
    }
    pthread_exit((void *)0);
    return NULL;
}

void *consumer(void *args)
{
    int demand = *static_cast<int *>(args);
    while (true) {
        pthread_mutex_lock(&mutex);
        if (msgQueue.size() <= 0) {
            pthread_cond_wait(&condvar, &mutex);
        }
        if (msgQueue.size() > 0) {
            printf("Consume message %d\n", msgQueue.front());
            msgQueue.pop();
            --demand;
        }
        pthread_mutex_unlock(&mutex);
        if (!demand) break;
    }
    pthread_exit((void *)0);
    return NULL;
}


int main()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condvar, NULL);

    pthread_t producer1, producer2, producer3, consumer1, consumer2;

    Produce_range range1 = {0, 10};
    pthread_create(&producer1, &attr, producer, static_cast<void *>(&range1));
    Produce_range range2 = {range1.end, range1.end + 10};
    pthread_create(&producer2, &attr, producer, static_cast<void *>(&range2));
    Produce_range range3 = {range2.end, range2.end + 10};
    pthread_create(&producer3, &attr, producer, static_cast<void *>(&range3));

    int consume_demand1 = 20;
    int consume_demand2 = 10;
    pthread_create(&consumer1, &attr, consumer, 
            static_cast<void *>(&consume_demand1));
    pthread_create(&consumer2, &attr, consumer, 
            static_cast<void *>(&consume_demand2));

    pthread_join(producer1, NULL);
    pthread_join(producer2, NULL);
    pthread_join(producer3, NULL);
    pthread_join(consumer1, NULL);
    pthread_join(consumer2, NULL);
}
```

## 物件導向版

- thread.h

```cpp
#ifndef __thread_h__
#define __thread_h__

#include <pthread.h>

class Thread
{
public:
    Thread();
    virtual ~Thread();

    int start();
    int join();
    int detach();
    pthread_t self();

    virtual void* run() = 0;

private:
    pthread_t  m_tid;
    int        m_running;
    int        m_detached;
};

#endif
```
- thread.cpp

```cpp
#include "thread.h"

static void* runThread(void* arg)
{
    ((Thread*)arg)->run();
    return NULL;
}

Thread::Thread() : m_tid(0), m_running(0), m_detached(0) {}

Thread::~Thread()
{
    if (m_running == 1 && m_detached == 0) {
        pthread_detach(m_tid);
    }

    if (m_running == 1) {
        pthread_cancel(m_tid);
    }
}

int Thread::start()
{
    int result = pthread_create(&m_tid, NULL, runThread, this);

    if (result == 0) {
        m_running = 1;
    }

    return result;
}

int Thread::join()
{
    int result = -1;

    if (m_running == 1) {
        result = pthread_join(m_tid, NULL);

        if (result == 0) {
            m_detached = 0;
        }
    }

    return result;
}

int Thread::detach()
{
    int result = -1;

    if (m_running == 1 && m_detached == 0) {
        result = pthread_detach(m_tid);

        if (result == 0) {
            m_detached = 1;
        }
    }

    return result;
}

pthread_t Thread::self()
{
    return m_tid;
}

```

- main.cpp


```cpp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <queue>
#include "thread.h"

pthread_mutex_t mutex;
pthread_cond_t condvar;
std::queue<int> msgQueue;

struct Produce_range {
    int start;
    int end;
};

class producer: public Thread
{
public:
    producer(Produce_range range)
    {
        mRange = range;
    }

    void* run()
    {
        int start = mRange.start;
        int end = mRange.end;

        for (int x = start; x < end; x++) {
            usleep(200 * 1000);
            pthread_mutex_lock(&mutex);
            msgQueue.push(x);
            pthread_mutex_unlock(&mutex);
            pthread_cond_signal(&condvar);
            printf("Produce message %d\n", x);
        }

        pthread_exit((void*)0);
        return NULL;
    }

private:
    Produce_range mRange;
};

class consumer : public Thread
{
public:
    consumer(int consume_demand)
    {
        mConsume_demand = consume_demand;
    }

    void* run()
    {
        int demand = mConsume_demand;

        while (true) {
            pthread_mutex_lock(&mutex);

            if (msgQueue.size() <= 0) {
                pthread_cond_wait(&condvar, &mutex);
            }

            if (msgQueue.size() > 0) {
                printf("Consume message %d\n", msgQueue.front());
                msgQueue.pop();
                --demand;
            }

            pthread_mutex_unlock(&mutex);

            if (!demand) {
                break;
            }
        }

        pthread_exit((void*)0);
        return NULL;
    }

private:
    int mConsume_demand;
};

int main(int argc, char** argv)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condvar, NULL);

    Produce_range range1 = {0, 10};
    Produce_range range2 = {range1.end, range1.end + 10};
    Produce_range range3 = {range2.end, range2.end + 10};

    producer producer1(range1);
    producer producer2(range2);
    producer producer3(range3);

    consumer consumer1(10), consumer2(20);

    producer1.start();
    producer2.start();
    producer3.start();
    consumer1.start();
    consumer2.start();

    producer1.join();
    producer2.join();
    producer3.join();
    consumer1.join();
    consumer2.join();

    printf("main done\n");

    exit(0);
}
```


- Makefile

```sh
CC			= g++
CFLAGS		= -c -Wall 
LDFLAGS		= -lpthread
SOURCES		= main.cpp thread.cpp
INCLUDES	= -I.
OBJECTS		= $(SOURCES:.cpp=.o)
TARGET		= thread

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)
```