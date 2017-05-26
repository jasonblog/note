## Product of Array Except Self 

Given an array of n integers where n > 1, `nums`, return an array output such that `output[i]` is equal to the product of all the elements of `nums` except `nums[i]`.

Solve it without division and in O(n).

For example, given `[1,2,3,4]`, return `[24,12,8,6]`.

Follow up:
Could you solve it with constant space complexity? (Note: The output array does not count as extra space for the purpose of space complexity analysis.)

## Solution

簡單方法是使用兩個數組a,b，其中一個存儲`nums[0..i - 1]`, 另一個數組存`nums[i + 1..n-1]`, 於是結果為`result[i] = a[i] * b[n - i - 1]`，即
a存儲i前面的積，b存儲i後面元素的積

```cpp
vector<int> productExceptSelf(vector<int>& nums) {
    if (nums.empty()) // 空
	    return vector<int>();
    if (nums.size() == 1) { //只有一個元素，返回{0}
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

以上方法O(n)時間，除了結果空間，還需要額外空間O(n)

題目要求O(1)空間。

我們發現a數組得到i前面的積後，只需要累乘後面的積即可。

* `a[n- 1]`不需要乘
* `a[n - 2] = a[n - 2] * nums[n - 1]`
* `a[n - 3] = a[n - 3] * nums[n - 1] * nums[n - 2]`
* ...
* `a[0] = a[0] * nums[n - 1] * nums[n - 2] * ... * nums[1]`

使用一個變量product，表示從後往前的累乘，則`a[i] = a[i] * product`

```cpp
vector<int> productExceptSelf(vector<int> &nums) {
    if (nums.empty()) // 空
	    return vector<int>();
    if (nums.size() == 1) { //只有一個元素，返回{0}
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
此時空間為O(1)
