# Customers Who Never Order

## Problem

Suppose that a website contains two tables, the `Customers` table and the `Orders` table. Write a SQL query to find all customers who never order anything.

Table: `Customers`.
```
+----+-------+
| Id | Name  |
+----+-------+
| 1  | Joe   |
| 2  | Henry |
| 3  | Sam   |
| 4  | Max   |
+----+-------+
```
Table: `Orders`.
```
+----+------------+
| Id | CustomerId |
+----+------------+
| 1  | 3          |
| 2  | 1          |
+----+------------+
```
Using the above tables as example, return the following:
```
+-----------+
| Customers |
+-----------+
| Henry     |
| Max       |
+-----------+
```

## Solution

直接使用exists语句查询

## Code
```sql
select c.Name as Customers
from Customers as c
where not exists (select o.Id from Orders as o where c.Id = o.CustomerId);
```

## exists语句

`exists`语句后面接一个`select`子查询，但实际并不返回任何数据，只返回是否有数据，存在返回`true`，否则`false`.
