# Word Frequency

## Problem

Write a bash script to calculate the frequency of each word in a text file words.txt.

For simplicity sake, you may assume:

    * words.txt contains only lowercase characters and space ' ' characters.
    * Each word must consist of lowercase characters only.
    * Words are separated by one or more whitespace characters.

For example, assume that `words.txt` has the following content:
```
the day is sunny the the
the sunny is is
```
Your script should output the following, sorted by descending frequency:
```
the 4
is 3
sunny 2
day 1
```

## Solution

可以使用[關聯數組實現](https://github.com/krystism/notes/blob/master/linux/wordCount.md)

後面需要按頻數降序排序可以使用`sort`實現

* -n 表示按照數值排序，而不是默認的字典排序
* -t & -k 其中-t指定字段分隔符，-k表示排序的字段
* -r 表示降序。

代碼為:
```bash
#!/bin/bash
declare -A count
for word in $(cat words.txt)
do
	let count[$word]++
done
(for word in ${!count[@]}
do
	printf "%s %d\n" $word ${count[$word]}
done) | sort -n -r -t ' ' -k 2
```
