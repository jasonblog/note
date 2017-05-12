## 12. Integer to Roman

Given an integer, convert it to a roman numeral.

Input is guaranteed to be within the range from 1 to 3999.

## 罗马数字拼写规则

参考[维基百科](https://zh.wikipedia.org/wiki/%E7%BD%97%E9%A9%AC%E6%95%B0%E5%AD%97):


罗马数字共有7个，即I（1）、V（5）、X（10）、L（50）、C（100）、D（500）和M（1000）。按照下述的规则可以表示任意正整数。需要注意的是罗马数字中没有“0”，与进位制无关。一般认为罗马数字只用来记数，而不作演算。

* 重复数次：一个罗马数字重复几次，就表示这个数的几倍。
* 右加左减：
	+ 在较大的罗马数字的右边记上较小的罗马数字，表示大数字加小数字。
	+ 在较大的罗马数字的左边记上较小的罗马数字，表示大数字减小数字。
	+ 左减的数字有限制，仅限于**I、X、C**。比如45不可以写成VL，只能是XLV
	+ 阿拉伯数字每位数字分别表示,左减时不可跨越一个位值。比如，99不可以用IC（100 - 1）表示，而是用XCIX（[100 - 10] + [10 - 1]）表示。
	+ 左减数字必须为一位，比如8写成VIII，而非IIX。
	+ 右加数字不可连续超过三位，比如14写成XIV，而非XIIII。（见下方“数码限制”一项。）
* 加线乘千：
	+ 在罗马数字的上方加上一条横线或者加上下标的Ⅿ，表示将这个数乘以1000，即是原数的1000倍。
	+ 同理，如果上方有两条横线，即是原数的1000000（1000^{2}）倍。
* 数码限制：
	+ 同一数码最多只能连续出现三次，如40不可表示为XXXX，而要表示为XL。
	+ 例外：由于IV是古罗马神话主神朱庇特（即IVPITER，古罗马字母里没有J和U）的首字，因此有时用IIII代替IV。

## Solution

按照以上的规则转换，发现还是挺复杂的，依据**阿拉伯数字每位数字分别表示**, 因此可以把每一位的数值分别表示写成一个静态表，然后把数字逐位转化即可:

位值 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
-----|---|---|---|---|---|---|---|---|---
个位 | I | II | III | IV | V | VI | VII | VIII | IX
十位 | X | XX | XXX | XL | L | LX | LXX | LXXX | XC
百位|C|CC|CCC|CD|D|DC|DCC|DCCC|CM
千位|M|MM|MMM|-|-|-|-|-


```cpp
const string ROMAN_TABLE[4][9] = {
            {"I", "II", "III", "IV", "V", "VI", "VII","VIII","IX"},
            {"X", "XX", "XXX", "XL", "L","LX", "LXX","LXXX", "XC"},
            {"C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"},
            {"M", "MM", "MMM",  "", "", "", "", "", ""}
    };
```

然后逐位转化即可：

```cpp
string intToRoman(int num) {
    string result;
    for (int i = 0; num; i++, num /= 10) {
	    if (num % 10) {
		    result = ROMAN_TABLE[i][num % 10 - 1] + result;
	    }
    }
    return result;
}
```

## 参考

1. [Roman to Integer](../RomantoInteger)
