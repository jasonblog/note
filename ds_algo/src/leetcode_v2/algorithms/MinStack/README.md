## Min Stack
Design a stack that supports push, pop, top, and retrieving the minimum element in constant time.

    * push(x) -- Push element x onto stack.
    * pop() -- Removes the element on top of the stack.
    * top() -- Get the top element.
    * getMin() -- Retrieve the minimum element in the stack.

## Solution

使用两个栈实现，其中一个栈values保存值，另一个栈minValues保存最小值。

* 当入栈时，如果有更小的值(x <= minValues.top()), 则把该值也压入最小值栈
* 当出栈时，如果弹出的值是最小值(x == minValues.top())， 则把最小值栈也弹出一个值

```cpp
class MinStack {
	public:
		void push(int x) {
			values.push(x);
			if (minValues.empty() || x <= minValues.top())
				minValues.push(x);
		}
		void pop() {
			int x = values.top();
			values.pop();
			if (x == minValues.top())
				minValues.pop();

		}
		int top() const {
			return values.top();
		}
		int getMin() const {
			return minValues.top();
		}
		int size() const {
			return values.size();
		}
	private:
		stack<int> values;
		stack<int> minValues;
};
```
