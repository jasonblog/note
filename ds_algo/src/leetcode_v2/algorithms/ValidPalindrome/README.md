## Valid Palindrome

Given a string, determine if it is a palindrome, considering only alphanumeric characters and ignoring cases.

For example,
`"A man, a plan, a canal: Panama"` is a palindrome.
`"race a car"` is not a palindrome.

Note:

* Have you consider that the string might be empty? This is a good question to ask during an interview.

* For the purpose of this problem, we define empty string as valid palindrome. 

## Solution

直接前后比较即可
```cpp
class Solution {
	public:
		bool isPalindrome(string s) {
			int n = s.size();
			if (n == 0)
				return true;
			int left = next(s, -1);
			int right = prev(s, n);
			for (int i = next(s, -1), j = prev(s, n); i < j; i = next(s, i), j = prev(s, j)) {
				if (!equals(s[i], s[j]))
					return false;
			}
			return true;
		}
	private:
		int next(const string &s, int i) {
			i++;
			while (!isalnum(s[i])) i++;
			return i;
		}
		int prev(const string &s, int j) {
			j--;
			while (!isalnum(s[j])) j--;
			return j;
		}
		bool equals(char a, char b) {
			a = tolower(a);
			b = tolower(b);
			return a == b;
		}
};
```
