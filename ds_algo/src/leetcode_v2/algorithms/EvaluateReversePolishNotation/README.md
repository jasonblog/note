## Evaluate Reverse Polish Notation

Evaluate the value of an arithmetic expression in Reverse Polish Notation.

Valid operators are` +, -, *, /`. Each operand may be an integer or another expression.

Some examples:
```
  ["2", "1", "+", "3", "*"] -> ((2 + 1) * 3) -> 9
  ["4", "13", "5", "/", "+"] -> (4 + (13 / 5)) -> 6
```

## Solution

使用栈保存数字:

* 当遇到新的数字时，压入栈
* 当遇到操作符时，从栈中弹出两个数，先后弹出的数分别为第二操作数、第一操作数.
* 计算两个操作数和对应运算符进行运算的值，压入栈

## Code
```java
public int evalRPN(List<String> tokens) {
	LinkedList<String> repo = new LinkedList<String>();
	for (String token : tokens) {
		if (isOperator(token)) {
			String num1 = repo.pop();
			String num2 = repo.pop();
			int result = expr(num2, num1, token);
			repo.push(String.valueOf(result));
		} else {
			repo.push(token);
		}
	}
	return Integer.parseInt(repo.pop());
}
```
