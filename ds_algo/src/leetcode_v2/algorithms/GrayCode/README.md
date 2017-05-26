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

求格雷碼。

剛開始不知道什麼意思。。。我去，居然想到用搜索。。。結果到5時就產生組合爆炸了。。。

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

可見時間空間複雜度都是不能接受的。。。

後來發現原來是格雷碼，記得以前格雷碼是這樣求的:
> 最高位保留作為結果的最高位
> 然後分別使用次高位和高位做異或運算。即前一位和後一位異或作為作為當前位。

上面的方法說得太複雜，其實就一個數i的格雷碼，就是 `i ^ (i >> 1)`, 就是這麼簡單.

題目要求格雷碼的一個序列，第一個為0，其實就是求0 ~ 2<sup>n</sup>-1的格雷碼，於是結果為:

```cpp
vector<int> grayCode(int n) {
	vector<int> result;
	for (int i = 0; i < (1 << n); ++i) {
		result.push_back(i ^ (i >> 1));
	}
	return result;
}
```

我們也可以使用n-1推出n，即假設已經求出n-1位的結果集，現在求n位的結果:

* 把n-1位的結果集，最高位補0，此時n-1位變成了n位，比如0 1 變成00 01, 作為n的結果集。
* 把n-1位的結果集逆序，然後前面補1，作為n的結果集。
* 以上兩點合起來就是最後的結果集。

比如n=2時，結果集為

```
00
01
11
10
```

現在求n=3的情況:

```
[0]00
[0]01
[0]11
[0]10

[1]10
[1]11
[1]01
[1]00

最後結果為:
000
001
011
010
110
111
101
100
```

代碼為:

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
