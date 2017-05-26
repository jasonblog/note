## Min Stack
Design a stack that supports push, pop, top, and retrieving the minimum element in constant time.

    * push(x) -- Push element x onto stack.
    * pop() -- Removes the element on top of the stack.
    * top() -- Get the top element.
    * getMin() -- Retrieve the minimum element in the stack.

## Solution

使用兩個棧實現，其中一個棧values保存值，另一個棧minValues保存最小值。

* 當入棧時，如果有更小的值(x <= minValues.top()), 則把該值也壓入最小值棧
* 當出棧時，如果彈出的值是最小值(x == minValues.top())， 則把最小值棧也彈出一個值

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
