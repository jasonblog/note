## Largest Number

Given a list of non negative integers, arrange them such that they form the largest number.

For example, given `[3, 30, 34, 5, 9]`, the largest formed number is `9534330`.

Note: The result may be very large, so you need to return a string instead of an integer.

## Solution

首先想到這其實就是一道排序題，當不是簡單的數值排序，而是實現字典排序。

當數字長度一樣時，顯然直接字典排序即可。關鍵在於當長度不一樣時，如何排？

`12 121`, 顯然`12121 > 12112`, 因此`12`排在`121`前面，`12 123`，顯然`12312 > 12123`,因此`123`排在`12`前面.

**因此簡單認為短的排在前面還是後面都是錯誤的**

當短的字符結束還沒有得到結果，顯然短的字符串一定是長字符串的前綴，當短的字符結束時，短的重新回到起始位置，繼續比較。

但當其中一個字符串是另一個字符串的重複字串時會出現死循環，比如`12 1212`,因此需要特殊處理.

另外需要注意全是`0`的情況，此時應該輸出`0`，而不是`0000`.

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

上面的方法，關鍵都在處理長度不一樣時的情況，先判斷是否重複字串，然後循環比較字符。代碼有點複雜。

其實我們只需要判斷兩個字符串組合時誰先誰大。設`s1,s2`, 其實我們只需要比較` s1 + s2` 和` s2 + s1`即可。

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

上面代碼使用了函數式編程方法，並使用了`c++11`的lambda表達式，語法為
```
[ capture ] ( params ) mutable exception attribute -> ret { body }
其中capture為空、= 、 &，分別表示不捕獲外部變量、傳值、傳引用。
```
