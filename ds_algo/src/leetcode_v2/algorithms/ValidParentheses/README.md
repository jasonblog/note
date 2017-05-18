## Valid Parentheses

Given a string containing just the characters '(', ')', '{', '}', '[' and ']', determine if the input string is valid.

The brackets must close in the correct order, "()" and "()[]{}" are all valid but "(]" and "([)]" are not.

## Solution

栈的基本应用

## Code
```c
bool isLeft(char c)
{
	return c == '(' || c == '[' || c == '{';
}
bool isMatch(char a, char b)
{
	return (a == '(' && b == ')')
		|| (a == '[' && b == ']')
		|| (a == '{' && b == '}');
}
bool isValid(char *s)
{
	int len = strlen(s);
	char *stack = malloc(sizeof(char) * len);
	int pos = -1;
	for (int i = 0; i < len; ++i) {
		if (isLeft(s[i])) {
			stack[++pos] = s[i];
		} else {
			if (isMatch(stack[pos], s[i])) {
				--pos;
			} else {
				return false;
			}
		}
	}
	free(stack);
	return pos == -1;
}
```
