## Compare Version Numbers

Compare two version numbers version1 and version2.
If version1 > version2 return 1, if version1 < version2 return -1, otherwise return 0.

You may assume that the version strings are non-empty and contain only digits and the . character.
The . character does not represent a decimal point and is used to separate number sequences.
For instance, 2.5 is not "two and a half" or "half way to version three", it is the fifth second-level revision of the second first-level revision.

Here is an example of version numbers ordering:

`0.1 < 1.1 < 1.2 < 13.37`
Credits:
Special thanks to @ts for adding this problem and creating all test cases.

## Sulution

直接按`.`劃分，依次比較即可，如果比較不相等後面就不需要比較了。比如`1.2.12 和 1.3.123`,當第二個子版本序列`2 < 3`，因此後面不需要比較了。

注意不能用`sprintf(buf, "%f", version)`把字符串轉化成浮點再比較大小，因為有可能有多個`.`,即多個子版本。

## Code
```c
int compareVersion(char* version1, char* version2) {
	char *p = version1;
	char *q = version2;
	while (*p || *q) {
		int d1 = 0;
		while (*p && isdigit(*p)) {
			d1 *= 10;
			d1 += (*p - '0');
			p++;
		}
		int d2 = 0;
		while (*q && isdigit(*q)) {
			d2 *= 10;
			d2 += (*q - '0');
			q++;
		}
		if (d1 > d2)
			return 1;
		if (d1 < d2)
			return -1;
		if (*p)
			p++;
		if (*q)
			q++;
	}
	return 0;
}
```
