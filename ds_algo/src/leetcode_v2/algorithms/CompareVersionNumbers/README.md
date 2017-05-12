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

直接按`.`划分，依次比较即可，如果比较不相等后面就不需要比较了。比如`1.2.12 和 1.3.123`,当第二个子版本序列`2 < 3`，因此后面不需要比较了。

注意不能用`sprintf(buf, "%f", version)`把字符串转化成浮点再比较大小，因为有可能有多个`.`,即多个子版本。

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
