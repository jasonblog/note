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

搜索題。使用遞歸實現。

設括號對為N, 已生成的字符串左括號個數為open, 還能匹配左括號個數為`n = N - open`,已生成的字符串為x。定義
`generate(vector<string> &result, string x, int open, int n)`, 則

* 若`n == 0`, 說明所有的左括號用完了，把剩下的右括號追加到x中, 並把x追加到result中
* 若`open == 0`, 此時由於前面沒有左括號,因此不能匹配右括號，只能增加左括號, 即`generate(result, x + '(', 1, n - 1)`
* 若`open > 0`，此時既可以匹配左括號又可以匹配右括號:
	+ 匹配左括號: `generate(result, x + '(', open + 1, n - 1)`
	+ 匹配右括號: `generate(result, x + ')', open, n)`

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
