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

使用兩個棧模擬隊列

首先定義channel(first, second)操作，表示把第一個棧逐一彈出，並壓入第二個棧:

```cpp
void channel(stack<int> &src, stack<int> &dest) {
	while (!src.empty()) {
		dest.push(src.top());
		src.pop();
	}
}

```

* push(), push時往第一個棧壓入
* empty(), 當且僅當兩個棧都為空時，隊列空
* peek(), 若第二個棧為空，則channel(first, second), 返回second.top()
* pop(), 若第二個棧為空，則channel(first, second), second.pop()

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


