## Number of 1 Bits
Write a function that takes an unsigned integer and returns the number of ’1' bits it has (also known as the Hamming weight).

For example, the 32-bit integer ’11' has binary representation 00000000000000000000000000001011, so the function should return 3.

Credits:
Special thanks to @ts for adding this problem and creating all test cases.

# Solution

一个数x与x-1的与运算，就是把最低位的1置0。即 `x & (x - 1)`会把最低位的1置0。

# Coding
```c
int hammingWeight(uint32_t n)
{
		int sum = 0;
		while (n) {
			++sum;
			n &= (n - 1);
		}
		return sum;
}
```
