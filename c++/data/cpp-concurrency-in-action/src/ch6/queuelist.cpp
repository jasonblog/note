#include <iostream>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>

template <typename T>
class queue
{
private:
    struct node {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };

    std::mutex head_mutex;
    std::unique_ptr<node> head;
    std::mutex tail_mutex;
    node* tail;

    node* get_tail()
    {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        return tail;
    }

    std::unique_ptr<node> pop_head()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);

        if (head.get() == get_tail()) {
            return nullptr;
        }

        std::unique_ptr<node> old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }

public:
    queue(): head(new node), tail(head.get())
    {}

    queue(const queue& other) = delete;
    queue& operator=(const queue& other) = delete;

    std::shared_ptr<T> try_pop()
    {
        std::unique_ptr<node> old_head = pop_head();
        return old_head ? old_head->data : std::shared_ptr<T>();
    }

    void push(T new_value)
    {
        std::shared_ptr<T> new_data(
            std::make_shared<T>(std::move(new_value)));
        std::unique_ptr<node> p(new node);
        node* const new_tail = p.get();
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        tail->data = new_data;
        tail->next = std::move(p);
        tail = new_tail;
    }
};

void push(queue<int>* q)
{
    for (int i = 0; i < 10; ++i) {
        printf("pushing %d\n", i);
        q->push(i);
    }
}

void pop(queue<int>* q)
{
    int i = 0;

    while (true) {
        if (std::shared_ptr<int> p = q->try_pop()) {
            printf("poping %d\n", *p);
            ++i;
        }

        if (i == 10) {
            break;
        }
    }
}

int main()
{
    queue<int> q;
    std::thread th1(push, &q);
    std::thread th2(pop, &q);
    th1.join();
    th2.join();
    return 0;
}

