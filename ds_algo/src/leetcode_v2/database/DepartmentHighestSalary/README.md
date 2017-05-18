# Department Highest Salary

## Problem

The `Employee` table holds all employees. Every employee has an `Id`, a `salary`, and there is also a column for the `department Id`.
```
+----+-------+--------+--------------+
| Id | Name  | Salary | DepartmentId |
+----+-------+--------+--------------+
| 1  | Joe   | 70000  | 1            |
| 2  | Henry | 80000  | 2            |
| 3  | Sam   | 60000  | 2            |
| 4  | Max   | 90000  | 1            |
+----+-------+--------+--------------+
```
The `Department` table holds all departments of the company.
```
+----+----------+
| Id | Name     |
+----+----------+
| 1  | IT       |
| 2  | Sales    |
+----+----------+
```
Write a SQL query to find employees who have the highest salary in each of the departments. For the above tables, `Max` has the highest `salary` in the `IT department` and `Henry` has the highest `salary` in the `Sales department`.
```
+------------+----------+--------+
| Department | Employee | Salary |
+------------+----------+--------+
| IT         | Max      | 90000  |
| Sales      | Henry    | 80000  |
+------------+----------+--------+
```

## Solution 1

这道题很明显是聚集查询。首先想到的自然是`max` 和`group`语句, 于是不加思索的使用以下sql语句:
```sql
select d.Name as Department, e.Name as Employee, max(e.Salary) as Salary
from Employee as e
join Department as d on e.DepartmentId = d.Id
group by e.DepartmentId;
```

这是错误的查询，错误原因在于:

>> 对于带`group by`子句的`select`语句，`select`子句中的结果列必须是`group by`子句中的分组列或者聚集函数，不能有其他无关属性.

因此`select`中的`d.Name`, `e.Name` 都不能出现在结果列中。

如果我们只需要`DepartmentId` 和 最高`Salary`，则很容易实现:
```
mysql> select DepartmentId, max(Salary) as max from Employee group by DepartmentId;
+--------------+-------+
| DepartmentId | max   |
+--------------+-------+
|            1 | 90000 |
|            2 | 80000 |
+--------------+-------+
```
我们可以利用这个临时表连接`Employee`和`Department`就可以得到其他的属性列。

## Code
```sql
select d.Name as Department, e1.Name as Employee, e1.Salary as Salary
from Employee as e1
join (select DepartmentId, max(Salary) as max from Employee group by DepartmentId) as t
on e1.DepartmentId = t.DepartmentId
join Department as d
on e1.DepartmentId = d.Id
where e1.Salary = max;
```

## Solution 2

同样的假如我们已经得到`DepartmentId` 和最高`Salary`，则可以使用`in`子句实现。

## Code
```sql
SELECT D.Name AS Department ,E.Name AS Employee ,E.Salary 
from 
Employee E,
Department D 
WHERE E.DepartmentId = D.id 
AND (DepartmentId,Salary) in 
(SELECT DepartmentId,max(Salary) as max FROM Employee GROUP BY DepartmentId) 
```

## 关于聚集查询

在实际应用中，常常需要计算一些统计量。例如，总数、平均数、最大值、最小值等。
SQL的聚集函数可以单独使用，也可以配合`group by`子句使用。单独使用时，聚集函数作用于整个查询结果，
而与`group by`子句配合使用时，聚集函数作用于查询结果的每个分组。

聚集函数具有如下形式：
```
	COUNT ([ALL | DISTINCT] *)
或者
	<聚集函数> ([ALL | DISTINCT] <值表达式>)
```
第一种情况比较简单，`COUNT(*)` 或者 `COUNT(ALL *)`返回每个分组中的元组个数，而`COUNT(DISTINCT *)`返回每个分组中不同元组的个数。

第二种情况,聚集函数可以是`COUNT`， `MAX`， `MIN`，`AVG`等。值表达式是可求值的表达式，通常是属性。短语`ALL`或者`DISTINCT`是可选的，
默认是`ALL`.

## 分组

SQL语言提供了`group by`子句，其一般形式如下:
```
group by <分组列>{,<分组列>} [having <分组条件>]
```
其中，分组列是属性，可选的`having`子句用来过滤不满足条件的分组，
类似于`where`子句的查询条件，但其中允许出现聚集函数。

**对于带`group by`子句的select查询，select 子句中的结果列必须是`group by`子句中的分组列或者聚集函数，不能有其他无关属性列**

