## Product of Array Except Self 

Given an array of n integers where n > 1, `nums`, return an array output such that `output[i]` is equal to the product of all the elements of `nums` except `nums[i]`.

Solve it without division and in O(n).

For example, given `[1,2,3,4]`, return `[24,12,8,6]`.

Follow up:
Could you solve it with constant space complexity? (Note: The output array does not count as extra space for the purpose of space complexity analysis.)

## Solution

简单方法是使用两个数组a,b，其中一个存储`nums[0..i - 1]`, 另一个数组存`nums[i + 1..n-1]`, 于是结果为`result[i] = a[i] * b[n - i - 1]`，即
a存储i前面的积，b存储i后面元素的积

```cpp
vector<int> productExceptSelf(vector<int>& nums) {
    if (nums.empty()) // 空
	    return vector<int>();
    if (nums.size() == 1) { //只有一个元素，返回{0}
	    return vector<int>({0});
    }
    auto n = nums.size();
    vector<int> a(n, 1), b(n, 1);
    for (auto i = 1; i < n; ++i) {
	    a[i] = a[i - 1] * nums[i - 1];
	    b[i] = b[i - 1] * nums[n - i];
    }
    for (auto i = 0; i < n; ++i) {
	    a[i] *= b[n - i - 1];
    }
    return a;
}
```

以上方法O(n)时间，除了结果空间，还需要额外空间O(n)

题目要求O(1)空间。

我们发现a数组得到i前面的积后，只需要累乘后面的积即可。

* `a[n- 1]`不需要乘
* `a[n - 2] = a[n - 2] * nums[n - 1]`
* `a[n - 3] = a[n - 3] * nums[n - 1] * nums[n - 2]`
* ...
* `a[0] = a[0] * nums[n - 1] * nums[n - 2] * ... * nums[1]`

使用一个变量product，表示从后往前的累乘，则`a[i] = a[i] * product`

```cpp
vector<int> productExceptSelf(vector<int> &nums) {
    if (nums.empty()) // 空
	    return vector<int>();
    if (nums.size() == 1) { //只有一个元素，返回{0}
	    return vector<int>({0});
    }
    int n = nums.size();
    vector<int> result(n, 1);
    for (int i = 1; i < n; ++i)
	    result[i] = result[i - 1] * nums[i - 1];
    int product = 1;
    for (int i = n - 2; i >= 0; --i) {
	    product *= nums[i + 1];
	    result[i] *= product;
    }
    return result;

}
```
此时空间为O(1)
