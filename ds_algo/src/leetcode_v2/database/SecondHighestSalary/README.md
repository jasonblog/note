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

可以使用子查询的方式,即

```sql
select max(Salary) as SecondHighestSalary from Employee where Salary < (select max(Salary) from Employee);
```

这种方式更通用，但使用了子查询（不相关子查询），效率会低点。

## Solution 2

使用mysql特有`LIMIT` 和 `IFNULL`语法，下面简单介绍下:

### LIMIT

LIMIT可以限制select语句的输出行数，可以有一个参数或者两个参数。

如果只有一个参数，则表示限制的最大行数，如果不足最大行数，则输出所有行。

如果两个参数，前一个参数表示与第一行的偏移量，偏移量为0表示从第一行开始输出，第二个参数表示最大行数。如果偏移量超出行数范围，输出为空


### IFNULL

两个参数，如果第一个参数为空集合，则输出第二个参数。

综合以上两个函数，可以实现为:

```sql
select ifnull((select distinct Salary from Employee order by Salary desc limit 1,1), null) as SecondHighestSalary;
```
