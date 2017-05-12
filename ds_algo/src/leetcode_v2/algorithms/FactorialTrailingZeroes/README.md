## Factorial Trailing Zeroes

Given an integer n, return the number of trailing zeroes in n!.

Note: Your solution should be in logarithmic time complexity.

Credits:

Special thanks to @ts for adding this problem and creating all test cases.

## Solution

自然数相乘(0 除外), 只有当因子2 和 5相乘时才会有结尾0产生，比如`20 * 5 = 2 * 5 * 2 * 5`, 有两个`2 * 5`, 因此末尾有两个0.

阶乘计算时，自然数连续相乘，只需要统计有多少`2 * 5`,即可求出多少个末尾0, 而2的个数大于5的个数，因此最后只决定于有多少个`5`.

比如`10! = 1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9 * 10 = 2^8 * 3^4 * 5^2`,由于5的因子数为2,因此`10!`末尾有2个0

我们知道5,10,15,20产生一个5因子，即每间隔5产生一个5因子，而25产生2个5因子，即每个25又会多一个5因子，因此`25!`共可产生6个5因子。

因此要计算阶乘N的5的因子个数，先统计每隔5产生因子，其值等于`N / 5`, 接下来计算每隔25产生的因子,个数应该为`N / 25`, 而由于前面已经除了5,因此
只需要`N / 5`即可. 依次类推，直到没有5因子产生.:w

## Code
```c
int trailingZeroes(int n)
{
	if (n < 0)
		return 0;
	int sum = 0;
	while (n) {
		sum += n / 5;
		n /= 5;
	}
	return sum;
}
```
