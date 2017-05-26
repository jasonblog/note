## Roman to Integer

Given a roman numeral, convert it to an integer.

Input is guaranteed to be within the range from 1 to 3999.

## Solution

羅馬數字轉換參考[12 Integer to Roman](../IntegertoRoman),我們首先建立一個靜態表:

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

由轉化規則可知：

* 當羅馬數字當前值比前面值大時，需要減去前面的值，比如IV,前面時1,後面時5,需要減去前面的值，結果為４,因此左減至多一位，因此不會出現減兩位數的情況。
* 噹噹前值比前面的小時，需要加上當前的值。比如VI, 當前為1，前面為5,則結果為6。

```cpp
int romanToInt(string s) {
	int pre = ROMAN_TABLE[s[0]];
	int sum = pre;
	for (int i = 1; i < s.size(); ++i) {
		int cur = ROMAN_TABLE[s[i]];
		sum += cur;
		if (cur > pre) {
			sum -= 2 * pre; // 原來是＋，需要轉化為－，因此減去兩倍
		}
		pre = cur;
	}
	return sum;
}
```

## 參考

1. [12 Integer to Roman](../IntegertoRoman)
