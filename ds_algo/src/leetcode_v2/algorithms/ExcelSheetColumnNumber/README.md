## Excel Sheet Column Number
Related to question Excel Sheet Column Title

Given a column title as appear in an Excel sheet, return its corresponding column number.

For example:
```
    A -> 1
    B -> 2
    C -> 3
    ...
    Z -> 26
    AA -> 27
    AB -> 28 
```
Credits:
Special thanks to @ts for adding this problem and creating all test cases.

## Solution

进制转化，比如2进制转化成10进制

## Code
```c
int titleToNumber(char* s)
{
	if (s == NULL)
		return 0;
	int result = 0;
	while (*s) {
		result *= 26;
		result += *s - 'A' + 1;
		s++;
	}
	return result;
}
```

## 扩展

逆过程[Excel Sheet Column Title](../ExcelSheetColumnTitle)
