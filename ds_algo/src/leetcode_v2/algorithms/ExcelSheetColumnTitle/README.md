## Excel Sheet Column Title

Given a positive integer, return its corresponding column title as appear in an Excel sheet.

## Solution

进制转化，其实质就是把数字转成26进制

由于`1->A`, `2->B`, `26->Z`,即所以需要把n-1在操作

## Code
```cpp
string convertToTitle(int n) {
	string result;
	while (n) {
		result.push_back(--n % 26 + 'A');
		n /= 26;
	}
	return string(result.rbegin(), result.rend());
}
```
## 扩展

逆过程[Excel Sheet Column Number](../ExcelSheetColumnNumber)
