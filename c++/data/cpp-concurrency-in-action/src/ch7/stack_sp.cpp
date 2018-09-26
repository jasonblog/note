#include <thread>
#include <memory>
#include <atomic>
#include <iostream>

template <typename T>
class stack
{
private:
    struct node {
        std::shared_ptr<T> data;
        std::shared_ptr<node> next;

        node(T const& data_)
            : data(std::make_shared<T>(data_))
        {}
    };

    std::shared_ptr<node> head;
public:
    void push(T const& data)
    {
        std::shared_ptr<node> const new_node = std::make_shared<node>(data);
        new_node->next = std::atomic_load(head);

        while (!std::atomic_compare_exchange_weak(&head,
                &new_node->next, new_node));
    }
    std::shared_ptr<T> pop()
    {
        std::shared_ptr<node> old_head = std::atomic_load(head);

        while (old_head && !std::atomic_compare_exchange_weak(&head,
                &old_head, old_head->next));

        return old_head ? old_head->data : std::shared_ptr<T>();
    }
};

void push(stack<int>* s)
{
    for (int i = 0; i < 10; ++i) {
        printf("pushing %d\n", i);
        s->push(i);
    }
}

void pop(stack<int>* s)
{
    int count = 0;
    std::shared_ptr<int> e;

    while (count < 10) {
        if (e = s->pop()) {
            printf("popping %d\n", *e);
            ++count;
        }
    }
}

int main()
{
    std::shared_ptr<int> sp = std::make_shared<int>(1);
    //printf("std::atomic_is_lock_free(std::shared_ptr): %d\n",
    //        std::atomic_is_lock_free(&sp));
    stack<int> s;
    std::thread t1(push, &s);
    std::thread t2(pop, &s);
    t1.join();
    t2.join();
    return 0;
}

