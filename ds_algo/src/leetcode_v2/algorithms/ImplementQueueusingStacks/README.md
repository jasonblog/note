## Implement Queue using Stacks 

Implement the following operations of a queue using stacks.

* push(x) -- Push element x to the back of queue.
* pop() -- Removes the element from in front of queue.
* peek() -- Get the front element.
* empty() -- Return whether the queue is empty.
Notes:

* You must use only standard operations of a stack -- which means only `push to top`, `peek/pop from top`, `size`, and `is empty` operations are valid.
* Depending on your language, stack may not be supported natively. You may simulate a stack by using a list or deque (double-ended queue), as long as you use only standard operations of a stack.
* You may assume that all operations are valid (for example, no pop or peek operations will be called on an empty queue).

## Solution

使用两个栈模拟队列

首先定义channel(first, second)操作，表示把第一个栈逐一弹出，并压入第二个栈:

```cpp
void channel(stack<int> &src, stack<int> &dest) {
	while (!src.empty()) {
		dest.push(src.top());
		src.pop();
	}
}

```

* push(), push时往第一个栈压入
* empty(), 当且仅当两个栈都为空时，队列空
* peek(), 若第二个栈为空，则channel(first, second), 返回second.top()
* pop(), 若第二个栈为空，则channel(first, second), second.pop()

```cpp
bool empty(void) {
		return first.empty() && second.empty();
}
void push(int x) {
	first.push(x);
}
int peek(void) {
	if (second.empty()) {
		channel(first, second);
	}
	return second.top();
}
void pop(void) {
	if (second.empty()) {
		channel(first, second);
	}
	second.pop();
}
```


