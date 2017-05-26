# Transpose File

## Problem

Given a text file `file.txt`, transpose its content.

You may assume that each row has the same number of columns and each field is separated by the ' ' character.

For example, if file.txt has the following content:
```
name age
alice 21
ryan 30
```
Output the following:
```
name alice ryan
age 21 30
```

## Solution

使用awk解決,使用一個數組保存每列的值，每讀取一行，把每列追加到對應的列內容上。

## Code

```
#!/bin/bash
awk '
{
	for (i = 1; i <= NF; i++) {
		if (NR == 1) {
			result[i] = $i;
		} else {
			result[i] = result[i] " " $i
		}
	}
}
END {
	for (i = 1; result[i] != ""; i++) {
		print result[i];
	}
}' file.txt
```
