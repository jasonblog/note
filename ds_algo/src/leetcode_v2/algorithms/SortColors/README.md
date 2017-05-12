## Sort Colors 

Given an array with n objects colored red, white or blue, sort them so that objects of the same color are adjacent, with the colors in the order red, white and blue.

Here, we will use the integers 0, 1, and 2 to represent the color red, white, and blue respectively.

Note:
You are not suppose to use the library's sort function for this problem.


Follow up:
A rather straight forward solution is a two-pass algorithm using counting sort.
First, iterate the array counting number of 0's, 1's, and 2's, then overwrite array with total number of 0's, then 1's and followed by 2's.

Could you come up with an one-pass algorithm using only constant space?

## Solution

由于数字在一定范围以内，首先排除常规的排序算法。

显然可以使用计数排序，分别统计0,1,2的次数，然后逐一拷贝即可,时间复杂度O(n)，空间复杂度O(1)


但以上方法，显然需要2趟，能不能1趟就搞定呢？


显然如果所有的0和2都排好了，那1自然也排好了。我们使用两个指针zero, two分别指向0,2的位置，当扫描数组a[i]等于0时，交换a[i]和a[zero]位置，
同时更新zero指针。同理当a[i] == 2时，交换a[two]和a[i],并更新two。

## Code
```c
void sortColors(int a[], int n)
{
	if (a == NULL || n == 1)
		return;
	int zero = 0, two = n - 1;
	for (int i = 0; i <= two; ++i) {
		if (a[i] == 0) {
			swap(&a[i], &a[zero++]);
		}
		if (a[i] == 2) {
			swap(&a[i--], &a[two--]);
		}
	}
}
```
