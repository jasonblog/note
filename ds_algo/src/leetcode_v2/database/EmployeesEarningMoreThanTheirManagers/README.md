# Employees Earning More Than Their Managers

## Problem

The `Employee` table holds all employees including their managers. Every employee has an `Id`, and there is also a column for the `manager Id`.
```
+----+-------+--------+-----------+
| Id | Name  | Salary | ManagerId |
+----+-------+--------+-----------+
| 1  | Joe   | 70000  | 3         |
| 2  | Henry | 80000  | 4         |
| 3  | Sam   | 60000  | NULL      |
| 4  | Max   | 90000  | NULL      |
+----+-------+--------+-----------+
```
Given the `Employee` table, write a SQL query that finds out employees who earn more than their managers. For the above table, Joe is the only employee who earns more than his manager.
```
+----------+
| Employee |
+----------+
| Joe      |
+----------+
```

## Solution 1: 相关子查询

```sql
select e1.Name as Employee from Employee2 as e1
where e1.ManagerId is not NULL
and e1.Salary > (select Salary from Employee2 where Id=e1.ManagerId);
```

结果超时，原因是相关子查询效率非常低，应该尽量避免

## Solution 2: 内连接

```sql
select e1.Name as Employee from Employee2 as e1
join Employee2 as e2
on e1.ManagerId = e2.Id and e1.Salary > e2.Salary
where e1.ManagerId is not NULL;
```

结果顺利通过。
