## Multiply Strings

Given two numbers represented as strings, return multiplication of the numbers as a string.

Note: The numbers can be arbitrarily large and are non-negative.

## Solution

模拟法

根据乘法规则，乘数逐位与被乘数相乘，然后相加，因此先实现乘数只有一位的情况:

为了防止进位处理，把结果reverse保存，即高位在后面，地位在前面，然后被乘数和乘数逐一相乘，如果大于10，则需要进位:

```cpp
string multiply(string num1, int i) {
	if (i == 0)
		return "0";
	if (i == 1) {
		reverse(begin(num1), end(num1)); // 返回结果总是reverse的
		return num1;
	}
	string result;
	int n = num1.size();
	int j = n - 1;
	int carry = 0;
	for (int j = n - 1; j >= 0; --j) {
		int c = (num1[j] - '0') * i + carry;
		carry = c / 10;
		c %= 10;
		result.push_back(c + '0');
	}
	if (carry != 0)
		result.push_back(carry + '0');
	//reverse(begin(result), end(result));
	return result;
}
```


算得每个结果后，需要把所有的相乘结果相加，因此需要实现大数加法:

```cpp
/**
注意num1 和 num2都是reverse的，并且结果也是reverse的
*/
string add(string num1, string num2) {
	string result;
	int i = 0, j = 0;
	int carry = 0;
	while (i < num1.size() && j < num1.size()) {
		int a = num1[i] - '0';
		int b = num2[j] - '0';
		int c = a + b + carry;
		carry = c / 10;
		c %= 10;
		result.push_back(c + '0');
		i++, j++;
	}
	while (i < num1.size()) {
		int c = num1[i] - '0' + carry;
		carry = c / 10;
		c %= 10;
		result.push_back(c + '0');
		i++;
	}
	while (j < num2.size()) {

		int c = num2[j] - '0' + carry;
		carry = c / 10;
		c %= 10;
		result.push_back(c + '0');
		j++;
	}
	if (carry != 0) {
		result.push_back(carry + '0');
	}
	return result;
}
```

最后结果就是乘数逐位与被乘数相乘即可，注意乘数不是各位时，应该乘以10^<sup>n</sup>，n位第几位：

```cpp
string multiply(string num1, string num2) {
	if (  num1 == "0" || num1 == ""
	   || num2 == "0" || num2 == "") {
		return "0";
	}
	/*
	 * 反而更慢？ 
	if (num1.size() < num2.size())
		return multiply(num2, num1);
	*/
	string result;
	int n = num2.size();
	for (int i = n - 1; i >= 0; --i) {
		if (num2[i] == '0')
			continue;
		string s = multiply(num1, num2[i] - '0');
		string zeros;
		for (int j = 1; j < n - i; ++j)
			zeros.push_back('0');
		s = zeros + s;
		result = add(result, s);
	}
	reverse(begin(result), end(result));
	return result == "" ? "0" : result;
}
```
