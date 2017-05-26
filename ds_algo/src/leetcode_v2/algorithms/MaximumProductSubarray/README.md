## Maximum Product Subarray

Find the contiguous subarray within an array (containing at least one number) which has the largest product.

For example, given the array `[2,3,-2,4]`,
the contiguous subarray `[2,3]` has the largest product = `6`. 

## Solution

設pos，neg分別保存當前的可能最大正數和最小負數，max為候選最大值。則

* `a[i] == 0, pos = neg = 0`
* `a[i] > 0, pos = MAX(pos * a[i], a[i]), neg = MIN(neg * a[i], a[i])`, 取`MAX，MIN`的原因是排除舊值等於0的情況
* `a[i] < 0, pos = MAX(neg * a[i], a[i]), neg = MIN(pos * a[i], a[i])`
* `max = MAX(max, pos)`

## Code
```cpp
int maxProduct(vector<int>& nums) {
    const size_t size = nums.size();
    if (size < 1)
	    return 0;
    if (size == 1)
	    return nums[0];
    int neg = 0, pos = 0, max = 0;
    for (auto i = 0; i < size; ++i) {
	    if (nums[i] == 0) {
		    pos = neg = 0;
	    } else if (nums[i] > 0) {
		    pos = MAX(pos * nums[i], nums[i]);
		    neg = MIN(neg * nums[i], neg);
	    } else {
		    int old_pos = pos;
		    int old_neg = neg;
		    pos = MAX(old_neg * nums[i], nums[i]);
		    neg = MIN(old_pos * nums[i], nums[i]);
	    }
	    max = MAX(max, pos);
    }
    return max;
}
```

## 擴展

[Maximum Subarray](../MaximumSubarray): 最大連續之和
