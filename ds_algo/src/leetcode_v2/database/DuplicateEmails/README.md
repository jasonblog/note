# Duplicate Emails

## Problem

Write a SQL query to find all duplicate emails in a table named `Person`.
```
+----+---------+
| Id | Email   |
+----+---------+
| 1  | a@b.com |
| 2  | c@d.com |
| 3  | a@b.com |
+----+---------+
```
For example, your query should return the following for the above table:
```
+---------+
| Email   |
+---------+
| a@b.com |
+---------+
```
Note: All emails are in lowercase.

## Solution 1

直接使用子查询

## Code
```sql
select Email
from (select Email, count(*)  count from Person  group by Email) as e
where e.count > 1;
```

## Solution 2

分组过滤

## Code
```sql
select Email
from Person group by Email
having count(Email) > 1;
```
