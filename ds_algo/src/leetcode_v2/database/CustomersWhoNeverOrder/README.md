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

直接使用exists語句查詢

## Code
```sql
select c.Name as Customers
from Customers as c
where not exists (select o.Id from Orders as o where c.Id = o.CustomerId);
```

## exists語句

`exists`語句後面接一個`select`子查詢，但實際並不返回任何數據，只返回是否有數據，存在返回`true`，否則`false`.
