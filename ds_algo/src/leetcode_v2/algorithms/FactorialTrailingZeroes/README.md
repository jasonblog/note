## Factorial Trailing Zeroes

Given an integer n, return the number of trailing zeroes in n!.

Note: Your solution should be in logarithmic time complexity.

Credits:

Special thanks to @ts for adding this problem and creating all test cases.

## Solution

自然數相乘(0 除外), 只有當因子2 和 5相乘時才會有結尾0產生，比如`20 * 5 = 2 * 5 * 2 * 5`, 有兩個`2 * 5`, 因此末尾有兩個0.

階乘計算時，自然數連續相乘，只需要統計有多少`2 * 5`,即可求出多少個末尾0, 而2的個數大於5的個數，因此最後只決定於有多少個`5`.

比如`10! = 1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9 * 10 = 2^8 * 3^4 * 5^2`,由於5的因子數為2,因此`10!`末尾有2個0

我們知道5,10,15,20產生一個5因子，即每間隔5產生一個5因子，而25產生2個5因子，即每個25又會多一個5因子，因此`25!`共可產生6個5因子。

因此要計算階乘N的5的因子個數，先統計每隔5產生因子，其值等於`N / 5`, 接下來計算每隔25產生的因子,個數應該為`N / 25`, 而由於前面已經除了5,因此
只需要`N / 5`即可. 依次類推，直到沒有5因子產生.:w

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
