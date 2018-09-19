#include <iostream>
#include <cstdio>
#include <queue>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

template <typename T>
class queue
{
private:
    mutable std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable data_cond;

public:
    queue() {}

    queue(const queue& other)
    {
        std::lock_guard<std::mutex> lk(other.mut);
        data_queue = other.data_queue;
    }

    void push(T new_value)
    {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(new_value);
        data_cond.notify_one();
    }

    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] {return !data_queue.empty();});
        value = data_queue.front();
        data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] {return !data_queue.empty();});
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mut);

        if (data_queue.empty()) {
            return false;
        }

        value = data_queue.front();
        data_queue.pop();
        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);

        if (data_queue.empty()) {
            return std::shared_ptr<T>();
        }

        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};

void push(queue<int>* q)
{
    for (int i = 0; i < 10; ++i) {
        //std::cout << "pushing " << i << std::endl;
        printf("pushing %d\n", i);
        q->push(i);
    }
}

void pop(queue<int>* q)
{
    for (int i = 0; i < 10; ++i) {
        printf("poping %d\n", *q->wait_and_pop());
    }
}

int main()
{
    queue<int> q;
    std::thread t1(push, &q);
    std::thread t2(pop, &q);
    t1.join();
    t2.join();
    return 0;
}

