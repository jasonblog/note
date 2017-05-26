## 12. Integer to Roman

Given an integer, convert it to a roman numeral.

Input is guaranteed to be within the range from 1 to 3999.

## 羅馬數字拼寫規則

參考[維基百科](https://zh.wikipedia.org/wiki/%E7%BD%97%E9%A9%AC%E6%95%B0%E5%AD%97):


羅馬數字共有7個，即I（1）、V（5）、X（10）、L（50）、C（100）、D（500）和M（1000）。按照下述的規則可以表示任意正整數。需要注意的是羅馬數字中沒有“0”，與進位制無關。一般認為羅馬數字只用來記數，而不作演算。

* 重複數次：一個羅馬數字重複幾次，就表示這個數的幾倍。
* 右加左減：
	+ 在較大的羅馬數字的右邊記上較小的羅馬數字，表示大數字加小數字。
	+ 在較大的羅馬數字的左邊記上較小的羅馬數字，表示大數字減小數字。
	+ 左減的數字有限制，僅限於**I、X、C**。比如45不可以寫成VL，只能是XLV
	+ 阿拉伯數字每位數字分別表示,左減時不可跨越一個位值。比如，99不可以用IC（100 - 1）表示，而是用XCIX（[100 - 10] + [10 - 1]）表示。
	+ 左減數字必須為一位，比如8寫成VIII，而非IIX。
	+ 右加數字不可連續超過三位，比如14寫成XIV，而非XIIII。（見下方“數碼限制”一項。）
* 加線乘千：
	+ 在羅馬數字的上方加上一條橫線或者加上下標的Ⅿ，表示將這個數乘以1000，即是原數的1000倍。
	+ 同理，如果上方有兩條橫線，即是原數的1000000（1000^{2}）倍。
* 數碼限制：
	+ 同一數碼最多隻能連續出現三次，如40不可表示為XXXX，而要表示為XL。
	+ 例外：由於IV是古羅馬神話主神朱庇特（即IVPITER，古羅馬字母裡沒有J和U）的首字，因此有時用IIII代替IV。

## Solution

按照以上的規則轉換，發現還是挺複雜的，依據**阿拉伯數字每位數字分別表示**, 因此可以把每一位的數值分別表示寫成一個靜態表，然後把數字逐位轉化即可:

位值 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
-----|---|---|---|---|---|---|---|---|---
個位 | I | II | III | IV | V | VI | VII | VIII | IX
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

然後逐位轉化即可：

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

## 參考

1. [Roman to Integer](../RomantoInteger)
