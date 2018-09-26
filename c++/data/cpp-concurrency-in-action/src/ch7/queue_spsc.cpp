#include <iostream>
#include <cstdio>
#include <atomic>
#include <memory>
#include <thread>

template <typename T>
class queue
{
private:
    struct node {
        std::shared_ptr<T> data;
        node* next;

        node()
            : next(nullptr)
        {}
    };

    std::atomic<node*> head;
    std::atomic<node*> tail;

    node* pop_head()
    {
        node* const old_head = head.load();

        if (old_head == tail.load()) {
            return nullptr;
        }

        head.store(old_head->next);
        return old_head;
    }
public:
    queue()
        : head(new node)
        , tail(head.load())
    {}

    queue(const queue& other) = delete;
    queue& operator=(const queue& other) = delete;

    ~queue()
    {
        while (node* const old_head = head.load()) {
            head.store(old_head->next);
            delete old_head;
        }
    }

    std::shared_ptr<T> pop()
    {
        node* old_head = pop_head();

        if (!old_head) {
            return std::shared_ptr<T>();
        }

        std::shared_ptr<T> const res(old_head->data);
        delete old_head;
        return res;
    }

    void push(T new_value)
    {
        std::shared_ptr<T> new_data(std::make_shared<T>(new_value));
        node* p = new node;
        node* const old_tail = tail.load();
        old_tail->data.swap(new_data);
        old_tail->next = p;
        tail.store(p);
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

    while (i < 10) {
        std::shared_ptr<int> p = q->pop();

        if (p) {
            printf("poping %d\n", *p);
            ++i;
        }
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

