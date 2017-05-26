# Second Highest Salary

## Problem
Write a SQL query to get the second highest salary from the Employee table.
```
+----+--------+
| Id | Salary |
+----+--------+
| 1  | 100    |
| 2  | 200    |
| 3  | 300    |
+----+--------+
```
For example, given the above Employee table, the second highest salary is 200. If there is no second highest salary, then the query should return null.

## Solution 1

可以使用子查詢的方式,即

```sql
select max(Salary) as SecondHighestSalary from Employee where Salary < (select max(Salary) from Employee);
```

這種方式更通用，但使用了子查詢（不相關子查詢），效率會低點。

## Solution 2

使用mysql特有`LIMIT` 和 `IFNULL`語法，下面簡單介紹下:

### LIMIT

LIMIT可以限制select語句的輸出行數，可以有一個參數或者兩個參數。

如果只有一個參數，則表示限制的最大行數，如果不足最大行數，則輸出所有行。

如果兩個參數，前一個參數表示與第一行的偏移量，偏移量為0表示從第一行開始輸出，第二個參數表示最大行數。如果偏移量超出行數範圍，輸出為空


### IFNULL

兩個參數，如果第一個參數為空集合，則輸出第二個參數。

綜合以上兩個函數，可以實現為:

```sql
select ifnull((select distinct Salary from Employee order by Salary desc limit 1,1), null) as SecondHighestSalary;
```
