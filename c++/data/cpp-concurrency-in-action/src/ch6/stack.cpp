#include <exception>
#include <iostream>
#include <stack>
#include <memory>
#include <thread>
#include <mutex>

template <typename T>
class stack
{
private:
    std::stack<T> data;
    mutable std::mutex m;

public:
    stack() {}

    stack(const stack& other)
    {
        std::lock_guard<std::mutex> lock(other.m);
        data = other.data;
    }

    stack& operator=(const stack&) = delete;

    void push(T new_value)
    {
        std::lock_guard<std::mutex> lock(m);
        data.push(new_value);
    }

    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lock(m);

        if (data.empty()) {
            throw std::underflow_error("empty stack");
        }

        std::shared_ptr<T> const res(new T(data.top()));
        data.pop();
        return res;
    }

    void pop(T& value)
    {
        std::lock_guard<std::mutex> lock(m);

        if (data.empty()) {
            throw std::underflow_error("empty stack");
        }

        value = data.top();
        data.pop();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};

int main()
{
    stack<int> s;
    s.push(1);
    std::cout << *s.pop() << std::endl;
}

