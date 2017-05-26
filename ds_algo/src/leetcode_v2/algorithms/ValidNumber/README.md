## Valid Number

Validate if a given string is numeric.

Some examples:
```
"0" => true
" 0.1 " => true
"abc" => false
"1 a" => false
"2e10" => true
```
** Note: It is intended for the problem statement to be ambiguous. You should gather all requirements up front before implementing one. **

## Solution

細節題，狀態機

以下是更完備的測試樣例:
```
0 => true
 0.1  => true
abc => false
1 a => false
2e10 => true
4e+ => false
.-4 => false
.0 => true
46.e-3 => true
46.e3 => true
.e1 => false
```

分兩種情況討論。

第一種情況，字符序列中先出現了`e`,即還沒有出現小數點。

* `e`的前後都必須是整數, 比如`1e1 -1e-1 123e002`, 而 `e2 e-2 123e -123e`都是非法的。

第二種情況, 字符串序列中先出現了小數點，即還沒有出現e。

* 若小數點前面是整數，但小數點後面沒有內容，是合法的數字。比如`0. -5.`都是合法的。
* 若小數點前面是空，後面不能直接接`e`,比如`.e1 .e-2`都是非法的。
* 除了前面兩種情況，小數點必須是合法的正數科學計算法(比如`e123 123e-1 1e1`)。

另外，還需要實現`strip`函數，去掉字符串前後導空字符.

## Code
```c
char *strip(char *src, char *dest)
{
	if (src == NULL)
		return NULL;
	if (strlen(src) == 0) {
		dest[0] = 0;
		return 0;
	}
	int i = 0, j = strlen(src) - 1;
	while (isblank(src[i])) i++;
	while (isblank(src[j])) j--;
	int k = 0;
	while (i <= j) {
		dest[k++] = src[i++];
	}
	dest[k] = 0;
	return dest;
}
bool isInteger(char *s)
{
	if (s == NULL || strlen(s) == 0)
		return false;
	int i = 0;
	int len = strlen(s);
	if (s[i] == '+' || s[i] == '-') {
		// + - 
		if (len == 1)
			return false;
		i++;
	}
	for (; i < len; ++i) {
		if (!isdigit(s[i])) {
			return false;
		}
	}
	return true;
}
bool isE(char c)
{
	if (c == 'e' || c == 'E')
		return true;
	return false;
}
bool isSci(char *s)
{
	if (s == NULL || strlen(s) == 0)
		return false;
	int len = strlen(s);
	if (len == 1) {
		return isdigit(s[0]);
	}
	for (int i = 0; i < len; ++i) {
		if (isE(s[i])) {
			return isInteger(s + i + 1);
		}
		if (!isdigit(s[i]))
			return false;
	}
	return true;
}
bool isNumber(char *s)
{
	if (s == NULL)
		return false;
	char *striped = malloc(sizeof(char) * strlen(s));
	strip(s, striped);
	int len = strlen(striped);
	if (len == 0)
		return false;
	int i = 0;
	if (striped[i] == '+' || striped[i] == '-') {
		// 只有+ - 號非法
		if (len == 1)
			return false;
		++i;
	}
	for(; i < len; ++i) {
		if (isE(striped[i])) {
			// 出現e，但還沒有出現小數點, e前面必須是整數，e後面也是整數
			if (i >= 1 && isdigit(striped[i - 1])) {
					// 1e123
				return isInteger(striped + i + 1);
			} else {
				// e2 -e2 Invalid
				return false;
			}
		}
		// 出現小數點
		if (striped[i] == '.') {
			// 小數點前面是整數，後面沒有內容也是合法的，比如(5.), (0.)都是合法的小數
			if (i >= 1 && isdigit(striped[i - 1]) && i == len - 1) {
				// -4. Valid
				return true;
			}
			// (.e1) (.e)是非法的
			if (i == 0 && isE(striped[i + 1])) {
				// .e1 Invalid
				return false;
			}
			// 小數點後面是科學計數法是正確的。
			return isSci(striped + i + 1);
		}
		if (!isdigit(striped[i])) {
			return false;
		}
	}
	return true;
}
```
