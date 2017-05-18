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

使用自连接，然后一一和上一个比较，最后分组过滤即可
```sql
select log1.Num from Logs log1 
join Logs log2 
on log1.Id + 1 = log2.Id and log1.Num = log2.Num 
group by log1.Num 
having count(log1.Num) >= 2;
```

结果超时, 可能是分组导致的。

## Solution 2

还是使用自连接，不过为了避免group开销，直接使用三个表连接，分别指向Id, Id + 1, Id + 2，并分别让Num相等即可

```sql
select distinct a.Num 
from Logs a, Logs b,Logs c 
where a.Id=b.Id+1 and a.Num=b.Num and b.Id=c.Id+1 and b.Num=c.Num;
```

## Solution 3

统计法。如果这一题使用c编程实现，逐行读取，这就容易了。

可以使用case语句实现:
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
注意在case后的结果别名为n，当不能直接在该语句中使用，必须使用另一个select语句查询。
`when`是一个短路的，相当于`if`。`:=`用于赋值，`=`用于比较,必须在`from`语句中初始化`prevNum, count`的值

## mysql case 语句

相当于switch语句，when后面结条件，并且是短路的。例子:
```sql
select Num,
	case
		when Num % 2 = 0 then "Even"
		when Num % 2 = 1 then "Odd"
	end Numtype 
from Logs;
```
