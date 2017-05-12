## Plus One

Given a non-negative number represented as an array of digits, plus one to the number.

The digits are stored such that the most significant digit is at the head of the list.

## Solution

直接从最后开始，处理进位
```cpp
vector<int> plusOne(vector<int> &digits) {
	int i = digits.size() - 1;
	if (i < 0) {
		digits.push_back(1);
		return digits;
	}
	bool carry = true;
	while (i >= 0 && carry) {
		digits[i] += 1;
		if (digits[i] > 9) {
			digits[i] -= 10;
			carry = true;
		} else {
			carry = false;
		}
		i--;
	}
	if (carry) {
		digits.insert(digits.begin(), 1);
	}
	return digits;
}
```
