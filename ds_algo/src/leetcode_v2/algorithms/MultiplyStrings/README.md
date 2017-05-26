## Multiply Strings

Given two numbers represented as strings, return multiplication of the numbers as a string.

Note: The numbers can be arbitrarily large and are non-negative.

## Solution

模擬法

根據乘法規則，乘數逐位與被乘數相乘，然後相加，因此先實現乘數只有一位的情況:

為了防止進位處理，把結果reverse保存，即高位在後面，地位在前面，然後被乘數和乘數逐一相乘，如果大於10，則需要進位:

```cpp
string multiply(string num1, int i) {
	if (i == 0)
		return "0";
	if (i == 1) {
		reverse(begin(num1), end(num1)); // 返回結果總是reverse的
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


算得每個結果後，需要把所有的相乘結果相加，因此需要實現大數加法:

```cpp
/**
注意num1 和 num2都是reverse的，並且結果也是reverse的
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

最後結果就是乘數逐位與被乘數相乘即可，注意乘數不是各位時，應該乘以10^<sup>n</sup>，n位第幾位：

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
