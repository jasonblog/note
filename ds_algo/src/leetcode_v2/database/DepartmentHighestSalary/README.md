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

這道題很明顯是聚集查詢。首先想到的自然是`max` 和`group`語句, 於是不加思索的使用以下sql語句:
```sql
select d.Name as Department, e.Name as Employee, max(e.Salary) as Salary
from Employee as e
join Department as d on e.DepartmentId = d.Id
group by e.DepartmentId;
```

這是錯誤的查詢，錯誤原因在於:

>> 對於帶`group by`子句的`select`語句，`select`子句中的結果列必須是`group by`子句中的分組列或者聚集函數，不能有其他無關屬性.

因此`select`中的`d.Name`, `e.Name` 都不能出現在結果列中。

如果我們只需要`DepartmentId` 和 最高`Salary`，則很容易實現:
```
mysql> select DepartmentId, max(Salary) as max from Employee group by DepartmentId;
+--------------+-------+
| DepartmentId | max   |
+--------------+-------+
|            1 | 90000 |
|            2 | 80000 |
+--------------+-------+
```
我們可以利用這個臨時表連接`Employee`和`Department`就可以得到其他的屬性列。

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

同樣的假如我們已經得到`DepartmentId` 和最高`Salary`，則可以使用`in`子句實現。

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

## 關於聚集查詢

在實際應用中，常常需要計算一些統計量。例如，總數、平均數、最大值、最小值等。
SQL的聚集函數可以單獨使用，也可以配合`group by`子句使用。單獨使用時，聚集函數作用於整個查詢結果，
而與`group by`子句配合使用時，聚集函數作用於查詢結果的每個分組。

聚集函數具有如下形式：
```
	COUNT ([ALL | DISTINCT] *)
或者
	<聚集函數> ([ALL | DISTINCT] <值表達式>)
```
第一種情況比較簡單，`COUNT(*)` 或者 `COUNT(ALL *)`返回每個分組中的元組個數，而`COUNT(DISTINCT *)`返回每個分組中不同元組的個數。

第二種情況,聚集函數可以是`COUNT`， `MAX`， `MIN`，`AVG`等。值表達式是可求值的表達式，通常是屬性。短語`ALL`或者`DISTINCT`是可選的，
默認是`ALL`.

## 分組

SQL語言提供了`group by`子句，其一般形式如下:
```
group by <分組列>{,<分組列>} [having <分組條件>]
```
其中，分組列是屬性，可選的`having`子句用來過濾不滿足條件的分組，
類似於`where`子句的查詢條件，但其中允許出現聚集函數。

**對於帶`group by`子句的select查詢，select 子句中的結果列必須是`group by`子句中的分組列或者聚集函數，不能有其他無關屬性列**

