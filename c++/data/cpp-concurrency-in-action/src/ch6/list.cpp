#include <iostream>
#include <thread>
#include <memory>
#include <cstdio>

template <typename T>
class list
{
    struct node {
        std::mutex m;
        std::shared_ptr<T> data;
        std::unique_ptr<node>  next;

        node()
            : next()
        {}

        node(T const& value)
            : data(std::make_shared<T>(value))
        {}
    };

    node head;

public:
    list()
    {}

    ~list()
    {
        remove_if([](T const&) {
            return true;
        });
    }

    list(list const& other) = delete;
    list& operator=(list const& other) = delete;

    void push_front(T const& value)
    {
        std::unique_ptr<node> new_node(new node(value));
        std::lock_guard<std::mutex> lk(head.m);
        new_node->next = std::move(head.next);
        head.next = std::move(new_node);
    }

    template <typename Function>
    void for_each(Function f)
    {
        node* current = &head;
        std::unique_lock<std::mutex> lk(head.m);

        while (node* const next = current->next.get()) {
            std::unique_lock<std::mutex> next_lk(next->m);
            lk.unlock();
            f(*next->data);
            current = next;
            lk = std::move(next_lk);
        }
    }

    template <typename Predicate>
    std::shared_ptr<T> find_first_of(Predicate p)
    {
        node* current = &head;
        std::unique_lock<std::mutex> lk(head.m);

        while (node* const next = current->next.get()) {
            std::unique_lock<std::mutex> next_lk(next->m);
            lk.unlock();

            if (p(*next->data)) {
                return next->data;
            }

            current = next;
            lk = std::move(next_lk);
        }

        return std::shared_ptr<T>();
    }

    template <typename Predicate>
    void remove_if(Predicate p)
    {
        node* current = &head;
        std::unique_lock<std::mutex> lk(head.m);

        while (node* const next = current->next.get()) {
            std::unique_lock<std::mutex> next_lk(next->m);

            if (p(*next->data)) {
                std::unique_ptr<node> old_next = std::move(current->next);
                current->next = std::move(next->next);
                next_lk.unlock();
            } else {
                lk.unlock();
                current = next;
                lk = std::move(next_lk);
            }
        }
    }
};

void push(list<int>* l)
{
    for (int i = 0; i < 10; ++i) {
        printf("pushing %d\n", i);
        l->push_front(i);
    }
}

void visit(int i)
{
    printf("visit %d\n", i);
}

int main()
{
    list<int> l;
    std::thread t(push, &l);
    sleep(1);
    l.for_each(visit);
    t.join();
    return 0;
}

