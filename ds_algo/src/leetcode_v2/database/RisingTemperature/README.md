## Rising Temperature 

Given a `Weather` table, write a SQL query to find all dates' `Id`s with higher temperature compared to its previous (yesterday's) dates.
```
+---------+------------+------------------+
| Id(INT) | Date(DATE) | Temperature(INT) |
+---------+------------+------------------+
|       1 | 2015-01-01 |               10 |
|       2 | 2015-01-02 |               25 |
|       3 | 2015-01-03 |               20 |
|       4 | 2015-01-04 |               30 |
+---------+------------+------------------+
```
For example, return the following Ids for the above `Weather` table:
```
+----+
| Id |
+----+
|  2 |
|  4 |
+----+
```

## Solution

自连接查询，需要注意日期的比较,mysql中使用`TO_DAYS`转化，否则是字符串比较。

## Code

```sql
select w1.Id from Weather w1, Weather w2 
where TO_DAYS(w1.DATE) = TO_DAYS(w2.DATE) + 1 
and w1.Temperature > w2.Temperature;
```
