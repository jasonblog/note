## String to Integer

Implement atoi to convert a string to an integer.

**Hint: Carefully consider all possible input cases. If you want a challenge, please do not see below and ask yourself what are the possible input cases.**

>> Notes: It is intended for this problem to be specified vaguely (ie, no given input specs). You are responsible to gather all the input requirements up front.

The signature of the C++ function had been updated. If you still see your function signature accepts a const char * argument, please click the reload button  to reset your code definition.

spoilers alert... click to show requirements for atoi.

Requirements for atoi:

* The function first discards as many whitespace characters as necessary until the first non-whitespace character is found. Then, starting from this character, takes an optional initial plus or minus sign followed by as many numerical digits as possible, and interprets them as a numerical value.

* The string can contain additional characters after those that form the integral number, which are ignored and have no effect on the behavior of this function.

* If the first sequence of non-whitespace characters in str is not a valid integral number, or if no such sequence exists because either str is empty or it contains only whitespace characters, no conversion is performed.

* If no valid conversion could be performed, a zero value is returned. If the correct value is out of the range of representable values, INT\_MAX (2147483647) or INT\_MIN (-2147483648) is returned.

## Solution

算法并不难，关键就在于处理溢出的细节上。我们需要在溢出前，将要溢出时提前处理。

当前数字`x >= INT_MAX / 10`时，就退出循环，单独处理各种情况。
假设当前数值为x，最后一位数值last，则

* 若`str`后面还有至少2个数字，必然溢出
* 若`x > INT_MAX / 10 && last 存在`, 必然溢出
* 若`x == INT_MAX / 10 && last > 8`, 必然溢出
* 若`last == 8`, 如果`x > 0`，溢出,若`x < 0`,输出最小负数-2147483648
* 若`x == INT_MAX / 10 && last <= 7`, 不会溢出，直接输出

## Code
```c
int myAtoi(char *str)
{
	const int BOUND = INT_MAX / 10;
	if (str == NULL || strlen(str) == 0)
		return 0;
	int len = strlen(str);
	int i = 0;
	while (isblank(str[i])) ++i; // 去掉前导空白符
	int sign = 0;
	// 是否存在 + - 字符
	if (str[i] == '-') {
		sign = 1;
		++i;
	} else if (str[i] == '+') {
		sign = 0;
		++i;
	}
	int ans = 0;
	while (isdigit(str[i])) {
		ans *= 10;
		ans += (str[i] - '0');
		i++;
		// 可能溢出，必须额外预先处理
		if (ans >= BOUND)
			break;
	}
	if (i == len || !isdigit(str[i])) //字符串结束或者遇到非数字字符
		return sign ? -ans : ans;
	int last = str[i] - '0';
	// 比BOUND大，或者后面还有至少两个数字，一定溢出
	if (ans > BOUND || (i + 1 < len && isdigit(str[i + 1]))) {
		return sign ? -2147483648 : 2147483647;
	}
	if (last > 7) {
		if (sign && last == 8) // 最小负数,需要单独处理
			return -2147483648;
		return sign ? -2147483648 : 2147483647;
	}
	// 一定是-2147483647 ~ 2147483647范围的数字了
	ans *= 10;
	ans += last;
	return sign ? -ans : ans;
}
```
