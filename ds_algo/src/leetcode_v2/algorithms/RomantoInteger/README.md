## Roman to Integer

Given a roman numeral, convert it to an integer.

Input is guaranteed to be within the range from 1 to 3999.

## Solution

罗马数字转换参考[12 Integer to Roman](../IntegertoRoman),我们首先建立一个静态表:

```cpp
unordered_map<char, int> ROMAN_TABLE = {
	{'I', 1},
	{'V', 5},
	{'X', 10},
	{'L', 50},
	{'C', 100},
	{'D', 500},
	{'M', 1000}
};
```

由转化规则可知：

* 当罗马数字当前值比前面值大时，需要减去前面的值，比如IV,前面时1,后面时5,需要减去前面的值，结果为４,因此左减至多一位，因此不会出现减两位数的情况。
* 当当前值比前面的小时，需要加上当前的值。比如VI, 当前为1，前面为5,则结果为6。

```cpp
int romanToInt(string s) {
	int pre = ROMAN_TABLE[s[0]];
	int sum = pre;
	for (int i = 1; i < s.size(); ++i) {
		int cur = ROMAN_TABLE[s[i]];
		sum += cur;
		if (cur > pre) {
			sum -= 2 * pre; // 原来是＋，需要转化为－，因此减去两倍
		}
		pre = cur;
	}
	return sum;
}
```

## 参考

1. [12 Integer to Roman](../IntegertoRoman)
