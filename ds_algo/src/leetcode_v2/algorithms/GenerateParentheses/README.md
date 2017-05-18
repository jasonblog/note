Given n pairs of parentheses, write a function to generate all combinations of well-formed parentheses.

For example, given n = 3, a solution set is:
```
((()))
(()())
(())()
()(())
()()()
```

## Solution

搜索题。使用递归实现。

设括号对为N, 已生成的字符串左括号个数为open, 还能匹配左括号个数为`n = N - open`,已生成的字符串为x。定义
`generate(vector<string> &result, string x, int open, int n)`, 则

* 若`n == 0`, 说明所有的左括号用完了，把剩下的右括号追加到x中, 并把x追加到result中
* 若`open == 0`, 此时由于前面没有左括号,因此不能匹配右括号，只能增加左括号, 即`generate(result, x + '(', 1, n - 1)`
* 若`open > 0`，此时既可以匹配左括号又可以匹配右括号:
	+ 匹配左括号: `generate(result, x + '(', open + 1, n - 1)`
	+ 匹配右括号: `generate(result, x + ')', open, n)`

## Code
```cpp
class Solution {
	public:
		vector<string> generateParenthesis(int n) {
			vector<string> result;
			if (n < 1)
				return result;
			generate(result, "", 0, n);
			return result;
		}
		void generate(vector<string> &result, string value, int open, int n) {
			if (n == 0) {
				for (int i = 0; i < open; ++i)
					value += ')';
				result.push_back(value);
				return;
			}
			if (open == 0) {
				generate(result, value + '(', 1, n - 1);
			} else {
				generate(result, value + ')', open - 1, n);
				generate(result, value + '(', open + 1, n - 1);
			}
		}
};
```
