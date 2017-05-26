## Evaluate Reverse Polish Notation

Evaluate the value of an arithmetic expression in Reverse Polish Notation.

Valid operators are` +, -, *, /`. Each operand may be an integer or another expression.

Some examples:
```
  ["2", "1", "+", "3", "*"] -> ((2 + 1) * 3) -> 9
  ["4", "13", "5", "/", "+"] -> (4 + (13 / 5)) -> 6
```

## Solution

使用棧保存數字:

* 當遇到新的數字時，壓入棧
* 當遇到操作符時，從棧中彈出兩個數，先後彈出的數分別為第二操作數、第一操作數.
* 計算兩個操作數和對應運算符進行運算的值，壓入棧

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
