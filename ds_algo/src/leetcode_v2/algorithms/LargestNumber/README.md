## Largest Number

Given a list of non negative integers, arrange them such that they form the largest number.

For example, given `[3, 30, 34, 5, 9]`, the largest formed number is `9534330`.

Note: The result may be very large, so you need to return a string instead of an integer.

## Solution

首先想到这其实就是一道排序题，当不是简单的数值排序，而是实现字典排序。

当数字长度一样时，显然直接字典排序即可。关键在于当长度不一样时，如何排？

`12 121`, 显然`12121 > 12112`, 因此`12`排在`121`前面，`12 123`，显然`12312 > 12123`,因此`123`排在`12`前面.

**因此简单认为短的排在前面还是后面都是错误的**

当短的字符结束还没有得到结果，显然短的字符串一定是长字符串的前缀，当短的字符结束时，短的重新回到起始位置，继续比较。

但当其中一个字符串是另一个字符串的重复字串时会出现死循环，比如`12 1212`,因此需要特殊处理.

另外需要注意全是`0`的情况，此时应该输出`0`，而不是`0000`.

## Code
```c
bool isDup(char *s1, char *s2)
{
	if (s1 == NULL)
		return s2 == NULL;
	int len1 = strlen(s1), len2 = strlen(s2);
	if (len1 == 0 || len2 == 0)
		return false;
	char *p1, *p2;
	if (len1 >= len2) {
		if (len1 % len2 != 0)
			return false;
		p1 = s1;
		p2 = s2;
	} else {
		if (len2 % len1 != 0)
			return false;
		p1 = s2;
		p2 = s1;
	}
	while (*p1) {
		if (*p1 != *p2)
			return false;
		*p1++, *p2++;
		if (*p2 == 0) {
			if (len1 >= len2)
				p2 = s2;
			else
				p2 = s1;
		}
	}
	return true;
}
int cmp(const void *a, const void *b)
{
	char s1[20], s2[20];
	sprintf(s1, "%d", *(int *)a);
	sprintf(s2, "%d", *(int *)b);
	if (isDup(s1, s2))
		return 0;
	char *p1 = s1, *p2 = s2;
	while (*p1 && p2) {
		if (*p1 != *p2)
			return *p2 - *p1;
		p1++, p2++;
		if (*p1 == 0)
			p1 = s1;
		if (*p2 == 0)
			p2 = s2;
	}
	return 0;
}
char *largestNumber(int *num, int n)
{
	char *result = malloc(sizeof(char) * N);
	memset(result, 0, sizeof(char) * N);
	qsort(num, n, sizeof(num[0]), cmp);
	if (num[0] == 0) {
		strcpy(result, "0\0");
		return result;
	}
	int sum = 0;
	for (int i = 0; i < n; ++i) {
		sum += sprintf(result + sum, "%d", num[i]);
	}
	return result;
}
```

## Solution 2

上面的方法，关键都在处理长度不一样时的情况，先判断是否重复字串，然后循环比较字符。代码有点复杂。

其实我们只需要判断两个字符串组合时谁先谁大。设`s1,s2`, 其实我们只需要比较` s1 + s2` 和` s2 + s1`即可。

## Code
```cpp
class Solution {
	public:
		static bool cmp(const string &s1, const string &s2) {
			return s1 + s2 > s2 + s1;
		}
		string largestNumber(vector<int> &num) {
			vector<string> v;
			for_each(num.begin(), num.end(), [&](int i){v.push_back(to_string(i));});
			sort(v.begin(), v.end(), cmp);
			string result;
			for_each(v.begin(), v.end(), [&](string s){result += s;});
			if (result[0] == '0')
				return "0";
			return result;
		}

};
```

上面代码使用了函数式编程方法，并使用了`c++11`的lambda表达式，语法为
```
[ capture ] ( params ) mutable exception attribute -> ret { body }
其中capture为空、= 、 &，分别表示不捕获外部变量、传值、传引用。
```
