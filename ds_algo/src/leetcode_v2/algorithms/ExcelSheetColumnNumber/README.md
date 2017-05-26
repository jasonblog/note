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

進制轉化，比如2進制轉化成10進制

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

## 擴展

逆過程[Excel Sheet Column Title](../ExcelSheetColumnTitle)
