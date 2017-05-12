## Remove Element

Given an array and a value, remove all instances of that value in place and return the new length.

The order of elements can be changed. It doesn't matter what you leave beyond the new length.

## Solution
遍历数组，并逐一原地拷贝不等于key的元素
```cpp
int removeElement(int a[], int n, int key)
{
	int k = 0;
	for (int i = 0; i < n; ++i) {
		if (a[i] != key)
			a[k++] = a[i];
	}
	return k;
}
```
