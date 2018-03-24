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
