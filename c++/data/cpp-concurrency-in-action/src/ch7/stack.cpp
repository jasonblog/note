// Listing 7.4

// The reference-counted reclamation machinery
// This works reasonably well in low-load situations, where there are sutiable
// quiescent points at which no threads are in pop(). In high-load situaions,
// there may never be such a quiescent state. Under such a scenario, the
// to_be_deleted list would grow without bounds, and you'd be essentailly
// leaking memory again. If there aren't going to be any quiescent periods,
// you need to find an alternative mechanism for reclaming the nodes. The key
// is to identify when no more threads are accesing a particular node so that
// it can reclaimed. By far the easiest such mechanism to reason about is the
// use of hazard ponters.

#include <iostream>
#include <cstdio>
#include <atomic>
#include <memory>
#include <thread>

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

    std::atomic<node*>   head;
    std::atomic<unsigned> threads_in_pop;
    std::atomic<node*>   to_be_deleted;


    static void delete_nodes(node* nodes)
    {
        while (nodes) {
            node* next = nodes->next;
            delete nodes;
            nodes = next;
        }
    }

    void try_reclaim(node* old_head)
    {
        if (threads_in_pop == 1) {
            // claim list of to-be-deleted nodes
            node* nodes_to_delete = to_be_deleted.exchange(nullptr);

            // are you the only thread in pop()?
            if (!--threads_in_pop) {
                // on other thread can be accessing this list of pending nodes.
                // There may be new pending nodes, but you're not bothered
                // about them for now, as long as it's safe to reclaim your
                // list.
                delete_nodes(nodes_to_delete);
            } else if (nodes_to_delete) {
                // not safe to reclaim the nodes, so if there are any,
                // you must chain them back onto the list of nodes
                // pending deletion.
                // This can happen if there are multiple threads accessing the
                // data structure concurrently. Other threads might have
                // called pop() in between the first tet of thread_in_pop and
                // the "claiming" of the list, potentially adding new nodes to
                // the list that are still being accesed by one or more of
                // those other threads.
                chain_pending_nodes(nodes_to_delete);
            }

            delete old_head;
        } else {
            // not safe to delete any nodes, add the node to the pending list
            chain_pending_node(old_head);
            --threads_in_pop;
        }
    }

    void chain_pending_nodes(node* nodes)
    {
        node* last = nodes;

        // traverse the chain to find the end
        while (node* const next = last->next) {
            last = next;
        }

        chain_pending_nodes(nodes, last);
    }

    void chain_pending_nodes(node* first, node* last)
    {
        // replace the next pointer from the last node with
        // the current to_be_deleted pointer
        last->next = to_be_deleted;

        // store the first node in the chain as the new to_be_deleted pointer
        // have to use compare_exchange_weak in a loop here in order to ensure
        // that you don't leak any nodes that have been added by another thread
        while (!to_be_deleted.compare_exchange_weak(
                   last->next, first));

    }

    void chain_pending_node(node* n)
    {
        // adding a single node onto the list is a special case where the
        // first node onto the list is a special case where the first node
        // in the chain to be added is the same as the last one.
        chain_pending_nodes(n, n);
    }

public:
    stack()
        : head(nullptr)
        , threads_in_pop(0)
        , to_be_deleted(nullptr)
    {}

    void push(const T& data)
    {
        node* const new_node = new node(data);
        new_node->next = head.load();

        // loop to gurantee that last->next is correct
        while (!head.compare_exchange_weak(new_node->next, new_node));
    }

    std::shared_ptr<T> pop()
    {
        ++threads_in_pop; // increase counter before doing anything else
        node* old_head = head.load();

        while (old_head &&
               !head.compare_exchange_weak(old_head, old_head->next));

        std::shared_ptr<T> res;

        if (old_head) {
            res.swap(old_head->data); // extract data from node rather than
            // coping pointer
        }

        try_reclaim(old_head); // reclaim deleted nodes if you can
        return res;
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

