#include <stack>
#include <iostream>
using namespace std;
class Queue
{
private:
    stack<int> first;
    stack<int> second;
public:
    bool empty(void)
    {
        return first.empty() && second.empty();
    }
    void push(int x)
    {
        first.push(x);
    }
    int peek(void)
    {
        if (second.empty()) {
            channel(first, second);
        }

        return second.top();
    }
    void pop(void)
    {
        if (second.empty()) {
            channel(first, second);
        }

        second.pop();
    }
private:
    void channel(stack<int>& src, stack<int>& dest)
    {
        while (!src.empty()) {
            dest.push(src.top());
            src.pop();
        }
    }
};
int main(int argc, char** argv)
{
    Queue queue;
    queue.push(1);
    queue.push(2);
    queue.pop();
    cout << queue.peek() << endl;
    return 0;
}
