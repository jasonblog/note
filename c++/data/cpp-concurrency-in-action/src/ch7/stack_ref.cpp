#include <iostream>
#include <cstdio>
#include <atomic>
#include <memory>
#include <thread>

template <typename T>
class stack
{
private:
    struct node;

    // the external count is kept alongside the poiner to the node and is
    // increased every time the pointer is read. When the reader is finished
    // with the node, it decraes the internal count. A simple operation that
    // reads the pointer will thus leave the external count increaesd by one
    // and the internal count decreased by one when it's finished.
    //
    // When the external count/pointer pairing is no longer required (that is,
    // the node is no longer accessbile from a location accessible to multple
    // threads), the internal count is increased by the value of the external
    // count minus one and external counter is discarded. Once the internal
    // count is equal to zero, there are no outstanding references to the node
    // and it can be safely deleted.
    struct counted_node_ptr {
        int external_count;
        node* ptr;
    };

    struct node {
        std::shared_ptr<T> data;
        std::atomic_int internal_count;
        counted_node_ptr next;

        node(const T& data_)
            : data(new T(data_))
            , internal_count(0)
        {}
    };

    std::atomic<counted_node_ptr> head;
    // NOTE: on those platforms that support a double-word-compare-and-swap
    // operation, counted_node_ptr will be small enough for
    // std::atomic<counted_node_ptr> to be lock-free. If it isn't on your
    // platform, you might be better off using the std::shared_ptr<> version,
    // because std::atomic<> will use a mutex to guarantee atomicity when the
    // type is too large for the pltaform's atomic instrcutions (thus rending
    // your "lock-free" algorithm lock-based after all). Alternatively, if
    // you are're willing to limit the size of the counter, and you know that
    // your platform has spare bits in a pointer (for example, because the
    // address space is only 48 bits but a pointer is 64 bits), you can store
    // the count inside the spare bits of the pointer to fit it all back in a
    // single machine word.


    // once you've loaded the value of head, you must first increaes the count
    // of external references to the head node to indicate that you're
    // referencing it and to ensure that it's safe to deference it.
    // By incrementing the external reference count, you ensure that the
    // pointer remains valid for the duration of your access.
    void increase_head_count(counted_node_ptr& old_counter)
    {
        counted_node_ptr new_counter;

        do {
            new_counter = old_counter;
            ++new_counter.external_count;
        } while (!head.compare_exchange_strong(old_counter, new_counter));

        old_counter.external_count = new_counter.external_count;
    }

public:
    ~stack()
    {
        while (pop());
    }

    void push(const T& data)
    {
        counted_node_ptr new_node;
        new_node.ptr = new node(data);
        // internal_count is zeor, and the external_count is one;
        // because this is a new node, there's currently only one
        // external reference to the node (the head pointer itself).
        new_node.external_count = 1;
        new_node.ptr->next = head.load();

        while (!head.compare_exchange_weak(new_node.ptr->next, new_node));
    }

    std::shared_ptr<T> pop()
    {
        counted_node_ptr old_head = head.load();

        for (;;) {
            increase_head_count(old_head);
            node* ptr = old_head.ptr;

            // if the pointer is a null pointer, you're at the end of list:
            // no more entires
            if (!ptr) {
                return std::shared_ptr<T>();
            }

            // if the pointer isn't a null pointer, try to remove the node
            if (head.compare_exchange_strong(old_head, ptr->next)) {
                // you've taken the ownership of the node and can swap out
                // the data in prepration for returning it.
                std::shared_ptr<T> res;
                res.swap(ptr->data);

                // you've removed the node from the list, so you drop one
                // off the count for that, and you're no longer accessing
                // the node from this thread, so you drop another off the
                // count for that.
                const int count_increase = old_head.external_count - 2;

                // if the reference count is now zero, the previous value
                // (which is what fetch_add returns) was the negative of what
                // you just added, in which case you can delete the node.
                if (ptr->internal_count.fetch_add(count_increase) ==
                    -count_increase) {
                    delete ptr;
                }

                // whether or not you deleted the node, you've finished.
                return res;
            }
            // if the compare/exchange fails, another therad removed your node
            // before you did, or another thread added a new node to the stack.
            // Either way, you need start again with the fresh value of head
            // returned by the compare/exchange call. But first you must
            // decrease the reference count on the node you were trying to
            // remove. This thread won't access it anymore. If you're the last
            // thread to hold a reference (because another thread removed it
            // from the stack), the internal reference count will be 1, so
            // subtracing 1 will set the count to zero. In this case, you can
            // delete the node here before you loop.
            else if (ptr->internal_count.fetch_add(-1) == 1) {
                delete ptr;
            }
        }
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
    stack<int> s;
    std::thread t1(push, &s);
    std::thread t2(pop, &s);
    t1.join();
    t2.join();
    return 0;
}


