#include <iostream>
#include <cstdio>
#include <atomic>
#include <memory>
#include <thread>

// NOTE: if atomic operations on std::shared_ptr<> are lock-free,
// then compare/exchange would be enough to implement the lock-free
// mpmc queue

template <typename T>
class queue
{
private:
    struct node;
    struct counted_node_ptr {
        int external_count;
        node* ptr;
    };

    std::atomic<counted_node_ptr> head;
    std::atomic<counted_node_ptr> tail;

    struct node_counter {
        unsigned internal_count: 30;
        unsigned external_counters: 2;
        // you need only 2 bits because there are at most two such
        // counters (next and tail)
    };

    struct node {
        std::atomic<T*> data;
        std::atomic<node_counter> count;
        std::atomic<counted_node_ptr> next;

        node()
        {
            node_counter new_count;
            new_count.internal_count = 0;
            new_count.external_counters = 2;
            // because every node starts out referenced from tail and
            // from the next pointer of the previous node once you've
            // actually aadded it to the queue
            count.store(new_count);

            counted_node_ptr new_next;
            new_next.ptr = nullptr;
            new_next.external_count = 0;
            next.store(new_next);
        }

        void release_ref()
        {
            node_counter old_counter =
                count.load(std::memory_order_relaxed);
            node_counter new_counter;

            do {
                new_counter = old_counter;
                --new_counter.internal_count;
            } while (!count.compare_exchange_strong(
                         old_counter, new_counter,
                         std::memory_order_acquire, std::memory_order_relaxed));

            if (!new_counter.internal_count &&
                !new_counter.external_counters) {
                delete this;
            }
        }

    };

    void set_new_tail(counted_node_ptr& old_tail,
                      counted_node_ptr const& new_tail)
    {
        node* const current_tail_ptr = old_tail.ptr;

        while (!tail.compare_exchange_weak(old_tail, new_tail) &&
               old_tail.ptr == current_tail_ptr);

        if (old_tail.ptr == current_tail_ptr) {
            free_external_counter(old_tail);
        } else {
            current_tail_ptr->release_ref();
        }
    }

    static void increase_external_count(
        std::atomic<counted_node_ptr>& counter,
        counted_node_ptr& old_counter)
    {
        counted_node_ptr new_counter;

        do {
            new_counter = old_counter;
            ++new_counter.external_count;
        } while (!counter.compare_exchange_strong(
                     old_counter, new_counter,
                     std::memory_order_acquire, std::memory_order_relaxed));

        old_counter.external_count = new_counter.external_count;
    }

    static void free_external_counter(counted_node_ptr& old_node_ptr)
    {
        node* const ptr = old_node_ptr.ptr;
        int const count_increase = old_node_ptr.external_count - 2;
        node_counter old_counter =
            ptr->count.load(std::memory_order_relaxed);
        node_counter new_counter;

        do {
            new_counter = old_counter;
            --new_counter.external_counters;
            new_counter.internal_count += count_increase;
        } while (!ptr->count.compare_exchange_strong(
                     old_counter, new_counter,
                     std::memory_order_acquire, std::memory_order_relaxed));

        if (!new_counter.internal_count &&
            !new_counter.external_counters) {
            delete ptr;
        }
    }

public:
    queue()
        : head()
        , tail()
    {}

    queue(const queue& other) = delete;
    queue& operator=(const queue& other) = delete;

    ~queue()
    {
        /*
        while (node* const old_head = head.load()) {
            head.store(old_head->next);
            delete old_head;
        }
        */
    }

    void push(T new_value)
    {
        std::unique_ptr<T> new_data(new T(new_value));
        counted_node_ptr new_next;
        new_next.ptr = new node;
        new_next.external_count = 1;
        counted_node_ptr old_tail = tail.load();

        for (;;) {
            increase_external_count(tail, old_tail);
            T* old_data = nullptr;

            if (old_tail.ptr->data.compare_exchange_strong(
                    old_data, new_data.get())) {
                counted_node_ptr old_next = {0};

                if (!old_tail.ptr->next.compare_exchange_strong(
                        old_next, new_next)) {
                    delete new_next.ptr;
                    new_next = old_next;
                }

                set_new_tail(old_tail, new_next);
                new_data.release();
                break;
            } else {
                counted_node_ptr old_next = {0};

                if (old_tail.ptr->next.compare_exchange_strong(
                        old_next, new_next)) {
                    old_next = new_next;
                    new_next.ptr = new node;
                }

                set_new_tail(old_tail, old_next);
            }
        }
    }

    std::unique_ptr<T> pop()
    {
        counted_node_ptr old_head = head.load(std::memory_order_relaxed);

        for (;;) {
            increase_external_count(head, old_head);

            node* const ptr = old_head.ptr;

            if (ptr == tail.load().ptr) {
                ptr->release_ref();
                return std::unique_ptr<T>();
            }

            counted_node_ptr next = ptr->next.load();

            if (head.compare_exchange_strong(old_head, next)) {
                T* const res = ptr->data.exchange(nullptr);
                free_external_counter(old_head);
                return std::unique_ptr<T>(res);
            }

            ptr->release_ref();
        }
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


