## Reverse Bits

Reverse bits of a given 32 bits unsigned integer.

For example, given input `43261596` (represented in binary as `00000010100101000001111010011100`), return `964176192` (represented in binary as `00111001011110000010100101000000`).

Follow up:
If this function is called many times, how would you optimize it?

Related problem: Reverse Integer

Credits:
Special thanks to @ts for adding this problem and creating all test cases.

## Solution

位运算

## Code
```c
uint32_t reverseBits(uint32_t n) {
	uint32_t m = 0;
	for (int i = 0; i < 31; ++i) {
		m |= (n & 0x1);
		m <<= 1;
		n >>= 1;
	}
	return m | (n & 0x1);
}
```
