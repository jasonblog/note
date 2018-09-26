#include <iostream>
#include <cstdio>
#include <atomic>
#include <memory>
#include <thread>
#include "hazard_pointer.h"

std::atomic<void*>& get_hazard_pointer_for_current_thread()
{
    // The first time each thread calls this function, a new instance of
    // hp_owner is created. The constructor for this new instance then
    // searchs through the table of owner/pointer pairs looking for an entry
    // without an owner. It uses compare_exchange_strong() to check for an
    // entry without an owner and claim it in one go.
    //
    // Once the hp_owner instance has been created for a given thread, further
    // accesses are much faster because the pointer is cached, so the table
    // doesn't have to be scanned again.

    printf("get harzard pointer for current thread, thread id: %d\n",
           std::this_thread::get_id());
    thread_local static hp_owner hazard;
    return hazard.get_pointer();
}

bool outstanding_hazard_pointers_for(void* p)
{
    for (unsigned i = 0; i < max_hazard_pointers; ++i) {
        if (hazard_pointers[i].pointer.load() == p) {
            return true;
        }
    }

    return false;
}

template <typename T>
void do_delete(void* p)
{
    delete static_cast<T*>(p);
}

struct data_to_reclaim {
    void* data;
    std::function<void(void*)> deleter;
    data_to_reclaim* next;

    template<typename T>
    data_to_reclaim(T* p)
        : data(p)
        , deleter(&do_delete<T>)
        , next(0)
    {}

    ~data_to_reclaim()
    {
        deleter(data);
    }
};

std::atomic<data_to_reclaim*> nodes_to_reclaim;
void add_to_reclaim_list(data_to_reclaim* node)
{
    node->next = nodes_to_reclaim.load();

    while (!nodes_to_reclaim.compare_exchange_weak(node->next, node));
}

template<typename T>
void reclaim_later(T* data)
{
    add_to_reclaim_list(new data_to_reclaim(data));
}

void delete_nodes_with_no_hazards()
{
    // first claims the entire list of nodes to be reclaimed;
    // ensures that this is the only thread trying to reclaim
    // this particular set of nodes; other threads are now free
    // to add futher nodes to the list or event try to reclaim
    // them without impacting the operation of this thread.
    data_to_reclaim* current = nodes_to_reclaim.exchange(nullptr);

    while (current) {
        data_to_reclaim* const next = current->next;

        // check each node in turn to see if there are any outstanding
        // hazard pointers.
        if (!outstanding_hazard_pointers_for(current->data)) {
            // if there aren't, delete the entry
            delete current;
        } else {
            // otherwise, just add the item back on the list for
            // reclaiming later
            add_to_reclaim_list(current);
        }

        current = next;
    }
}

template <typename T>
class stack
{
private:
    struct node {
        std::shared_ptr<T> data;
        node* next;

        node(const T& data_)
            : data(std::make_shared<T>(data_))
        {}
    };

    std::atomic<node*> head;

public:
    stack()
        : head(nullptr)
    {}

    void push(const T& data)
    {
        node* const new_node = new node(data);
        new_node->next = head.load();

        while (!head.compare_exchange_weak(new_node->next, new_node));
    }

    std::shared_ptr<T> pop()
    {
        std::atomic<void*>& hp = get_hazard_pointer_for_current_thread();
        node* old_head = head.load();

        do {
            node* temp;

            do { // loop until you've set the harzard pointer to head
                temp = old_head;
                hp.store(old_head);
                old_head = head.load();
            } while (old_head != temp);
        } while (old_head &&
                 !head.compare_exchange_strong(old_head, old_head->next));

        hp.store(nullptr); // clear hazard pointer once you're finished
        std::shared_ptr<T> res;

        if (old_head) {
            res.swap(old_head->data);

            if (outstanding_hazard_pointers_for(old_head)) {
                // check for hazard pointers referencing
                // a node before you delete it
                reclaim_later(old_head);
            } else {
                delete old_head;
            }

            delete_nodes_with_no_hazards();
        }

        return res;
    }
};

void push(stack<int>* s)
{
    printf("starting push\n");

    for (int i = 0; i < 10; ++i) {
        printf("pushing %d\n", i);
        s->push(i);
    }
}

void pop(stack<int>* s)
{
    printf("starting pop\n");
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
    printf("creating stack\n");
    stack<int> s;
    printf("stack created\n");
    std::thread t1(push, &s);
    std::thread t2(pop, &s);
    t1.join();
    t2.join();
    return 0;
}

