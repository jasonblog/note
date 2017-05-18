## Container With Most Water
Given n non-negative integers a1, a2, ..., an, where each represents a point at coordinate (i, ai). n vertical lines are drawn such that the two endpoints of line i is at (i, ai) and (i, 0). Find two lines, which together with x-axis forms a container, such that the container contains the most water.

Note: You may not slant the container. 

## Solution

首先需要知道这些问题:

* 面积由长和宽, 宽度由x轴间隔决定，高度由短的直线决定
* 当宽度减少时，面积要变大，高度必须变大
* 可以使用控制变量法，先宽度从最大开始,即`i = 0, j = n - 1`, i向有移动
j向左移动。每次求面积，获取最大值
* 当i,j移动时，宽度必然减少，因此高度必须增大，因此高度减少的可以排除掉
* 面积由短的决定，因此先移动短的。

有了上面思路，算法就出来了，设left,right分别指示左直线和右直线，初始化left =0, right = n - 1,当left <= right, 左移left，否则右移right。

## Code
```c
int maxArea(int a[], int n)
{
	int max = -1;
	int left = 0, right = n - 1;
	for (int i = 1; i < n; ++i) {
		int area = (a[left] <= a[right] ? a[left] : a[right]) * (right - left);
		max = max < area ? area : max;
		if (a[left] <= a[right]) {
			int l = a[left];
			left++;
			while (left < right && a[left] <= l) {
				left++;
			}
		} else {
			int r = a[right];
			right--;
			while (left < right && a[right] <= r) {
				right--;
			}
		}
	}
	return max;
}
```
