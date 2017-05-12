#include <stack>
#include <algorithm>
#include <iostream>
#include <cstdio>
using namespace std;
class MinStack
{
public:
    void push(int x)
    {
        values.push(x);

        if (minValues.empty() || x <= minValues.top()) {
            minValues.push(x);
        }
    }
    void pop()
    {
        int x = values.top();
        values.pop();

        if (x == minValues.top()) {
            minValues.pop();
        }

    }
    int top() const
    {
        return values.top();
    }
    int getMin() const
    {
        return minValues.top();
    }
    int size() const
    {
        return values.size();
    }
private:
    stack<int> values;
    stack<int> minValues;
};
void test(const MinStack& stack)
{
    printf("top = %d, min = %d\n", stack.top(), stack.getMin());
}
int main(int argc, char** argv)
{
    MinStack stack;
    stack.push(0);
    stack.push(1);
    stack.push(0);
    test(stack);
    stack.pop();
    test(stack);
    return 0;
}
