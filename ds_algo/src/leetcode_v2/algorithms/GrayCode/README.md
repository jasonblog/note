## Gray Code

The gray code is a binary numeral system where two successive values differ in only one bit.

Given a non-negative integer n representing the total number of bits in the code, print the sequence of gray code. A gray code sequence must begin with 0.

For example, given n = 2, return [0,1,3,2]. Its gray code sequence is:

00 - 0
01 - 1
11 - 3
10 - 2
Note:
For a given n, a gray code sequence is not uniquely defined.

For example, [0,2,3,1] is also a valid gray code sequence according to the above definition.

For now, the judge is able to judge based on one instance of gray code sequence. Sorry about that.

## Solution

求格雷码。

刚开始不知道什么意思。。。我去，居然想到用搜索。。。结果到5时就产生组合爆炸了。。。

```cpp
vector<int> grayCode(int n) {
	vector<int> result;
	vector<int> cur;
	int N = 1;
	for (int i = 0; i < n; ++i)
		N <<= 1;
	dfs(result, cur, n, 0, N);
	return result;
}
void dfs(vector<int> &result, vector<int> cur, int n, int x, int N) { 
	cur.push_back(x);
	if (N == 1) {
		result = vector<int>(begin(cur), end(cur));
		return;
	}
	vector<int> next = getPossibleNext(n, x);
	for (auto i : next) {
		if (find(begin(cur), end(cur), i) == end(cur)) {
			dfs(result, cur, n, i, N - 1);
		}
	}
}
vector<int> getPossibleNext(int n, int x) {
	int base = 0x1;
	vector<int> result;
	for (int i = 0; i < n; ++i) {
		result.push_back(base ^ x);
		base <<= 1;
	}
	return result;
}
```

可见时间空间复杂度都是不能接受的。。。

后来发现原来是格雷码，记得以前格雷码是这样求的:
> 最高位保留作为结果的最高位
> 然后分别使用次高位和高位做异或运算。即前一位和后一位异或作为作为当前位。

上面的方法说得太复杂，其实就一个数i的格雷码，就是 `i ^ (i >> 1)`, 就是这么简单.

题目要求格雷码的一个序列，第一个为0，其实就是求0 ~ 2<sup>n</sup>-1的格雷码，于是结果为:

```cpp
vector<int> grayCode(int n) {
	vector<int> result;
	for (int i = 0; i < (1 << n); ++i) {
		result.push_back(i ^ (i >> 1));
	}
	return result;
}
```

我们也可以使用n-1推出n，即假设已经求出n-1位的结果集，现在求n位的结果:

* 把n-1位的结果集，最高位补0，此时n-1位变成了n位，比如0 1 变成00 01, 作为n的结果集。
* 把n-1位的结果集逆序，然后前面补1，作为n的结果集。
* 以上两点合起来就是最后的结果集。

比如n=2时，结果集为

```
00
01
11
10
```

现在求n=3的情况:

```
[0]00
[0]01
[0]11
[0]10

[1]10
[1]11
[1]01
[1]00

最后结果为:
000
001
011
010
110
111
101
100
```

代码为:

```cpp
vector<int> grayCode2(int n) {
	vector<int> result;
	result.push_back(0);
	for (int i = 0; i < n; ++i) {
		int high = 1 << i;
		for (int j = result.size() - 1; j >= 0; --j)
			result.push_back(result[j] + high);
	}
	return result;
}
```
