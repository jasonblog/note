## Happy Number
Write an algorithm to determine if a number is "happy".

A happy number is a number defined by the following process: Starting with any positive integer, replace the number by the sum of the squares of its digits, and repeat the process until the number equals 1 (where it will stay), or it loops endlessly in a cycle which does not include 1. Those numbers for which this process ends in 1 are happy numbers.

Example: 19 is a happy number
```
    1^2 + 9^2 = 82
    8^2 + 2^2 = 68
    6^2 + 8^2 = 100
    1^2 + 0^2 + 0^2 = 1
```
Credits:
Special thanks to @mithmatt for adding this problem and creating all test cases.

# Solution

計算模擬題

直接計算下一個值，如果等於1，返回true，否則放入hashset中，若已經存在hashset中，返回false
```cpp
bool isHappy(int n) {
	unordered_set<int> s;
	if (n == 0)
		return false;
	while (n != 1) {
		s.insert(n);
		n = next(n);
		if (s.find(n) != s.end()) {
			return false;
		}
	}
	return true;
}
int next(int n) {
	int ans = 0;
	while (n) {
		int mod = (n % 10);
		ans += (mod * mod);
		n /= 10;
	}
	return ans;
}
```

再根據提示，如果該數不是Happy的，則必然出現環，可以使用判斷[鏈表是否存在環](../LinkedListCycle)的方法，設置快慢指針，若快的能和慢的相遇，則有環

```cpp
bool isHappy(int n) {
	int slow = next(n);
	int fast = next(next(n));
	while (slow != 1 && slow != fast) {
		slow = next(slow);
		fast = next(next(fast));
	}
	return slow == 1;
}
```
