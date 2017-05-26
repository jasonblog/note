## Container With Most Water
Given n non-negative integers a1, a2, ..., an, where each represents a point at coordinate (i, ai). n vertical lines are drawn such that the two endpoints of line i is at (i, ai) and (i, 0). Find two lines, which together with x-axis forms a container, such that the container contains the most water.

Note: You may not slant the container. 

## Solution

首先需要知道這些問題:

* 面積由長和寬, 寬度由x軸間隔決定，高度由短的直線決定
* 當寬度減少時，面積要變大，高度必須變大
* 可以使用控制變量法，先寬度從最大開始,即`i = 0, j = n - 1`, i向有移動
j向左移動。每次求面積，獲取最大值
* 當i,j移動時，寬度必然減少，因此高度必須增大，因此高度減少的可以排除掉
* 面積由短的決定，因此先移動短的。

有了上面思路，算法就出來了，設left,right分別指示左直線和右直線，初始化left =0, right = n - 1,當left <= right, 左移left，否則右移right。

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
