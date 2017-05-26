## Consecutive Numbers

Write a SQL query to find all numbers that appear at least three times consecutively.
```
+----+-----+
| Id | Num |
+----+-----+
| 1  |  1  |
| 2  |  1  |
| 3  |  1  |
| 4  |  2  |
| 5  |  1  |
| 6  |  2  |
| 7  |  2  |
+----+-----+
```
For example, given the above Logs table, 1 is the only number that appears consecutively for at least three times.

## Solution 1

使用自連接，然後一一和上一個比較，最後分組過濾即可
```sql
select log1.Num from Logs log1 
join Logs log2 
on log1.Id + 1 = log2.Id and log1.Num = log2.Num 
group by log1.Num 
having count(log1.Num) >= 2;
```

結果超時, 可能是分組導致的。

## Solution 2

還是使用自連接，不過為了避免group開銷，直接使用三個表連接，分別指向Id, Id + 1, Id + 2，並分別讓Num相等即可

```sql
select distinct a.Num 
from Logs a, Logs b,Logs c 
where a.Id=b.Id+1 and a.Num=b.Num and b.Id=c.Id+1 and b.Num=c.Num;
```

## Solution 3

統計法。如果這一題使用c編程實現，逐行讀取，這就容易了。

可以使用case語句實現:
```sql
select Num from (
SELECT
        Num,
        CASE
            WHEN @prevNum = Num THEN @count := @count + 1
            WHEN @prevNum := Num THEN @count := 1
        END n
FROM Logs, (select @prevNum := NULL) initPrevNum, (select @count := 1) initCount
) as result where result.n >= 3;
```
注意在case後的結果別名為n，當不能直接在該語句中使用，必須使用另一個select語句查詢。
`when`是一個短路的，相當於`if`。`:=`用於賦值，`=`用於比較,必須在`from`語句中初始化`prevNum, count`的值

## mysql case 語句

相當於switch語句，when後面結條件，並且是短路的。例子:
```sql
select Num,
	case
		when Num % 2 = 0 then "Even"
		when Num % 2 = 1 then "Odd"
	end Numtype 
from Logs;
```
