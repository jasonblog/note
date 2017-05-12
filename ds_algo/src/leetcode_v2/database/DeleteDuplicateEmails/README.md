# Delete Duplicate Emails

Write a SQL query to delete all duplicate email entries in a table named `Person`, keeping only unique emails based on its smallest `Id`.
```
+----+------------------+
| Id | Email            |
+----+------------------+
| 1  | john@example.com |
| 2  | bob@example.com  |
| 3  | john@example.com |
+----+------------------+
```
`Id` is the primary key column for this table.
For example, after running your query, the above `Person` table should have the following rows:
```
+----+------------------+
| Id | Email            |
+----+------------------+
| 1  | john@example.com |
| 2  | bob@example.com  |
+----+------------------+
```

## Solution 1

只需要先找到按邮箱分组的最小Id，
```
mysql> select * from Person;
+------+-------+
| id   | Email |
+------+-------+
|    1 | a     |
|    2 | a     |
|    3 | b     |
|    4 | b     |
|    5 | b     |
|    6 | c     |
+------+-------+
6 rows in set (0.00 sec)

mysql> select min(Id) from Person group by Email;
+---------+
| min(Id) |
+---------+
|       1 |
|       3 |
|       6 |
+---------+
3 rows in set (0.00 sec)

```
然后删除这些不在最小Id的行即可。

```
mysql> delete from Person where Id not in (select min(Id) from Person group by Email);
ERROR 1093 (HY000): You can't specify target table 'Person' for update in FROM clause
```
可见，不能在`from`语句中出现需要待更新的表

我们可以把`select min(Id) from Person group by Email`生成一个临时表，然后再删除即可。

## Code

```sql
delete p from Person as p
where p.Id not in 
(select Id from (select min(Person.Id) as Id from Person group by Person.Email) as ToRemovedId);
```

## Solution 2

MYSQL的delete语句同样可以支持连接操作。先考虑select语句，需要查询重复的`Email`并且Id不是最小的。
```
mysql> select p1.Id, p1.Email from Person p1, Person p2 where p1.Email = p2.Email and p1.Id > p2.Id;
+------+-------+
| Id   | Email |
+------+-------+
|    2 | a     |
|    4 | b     |
|    5 | b     |
|    5 | b     |
+------+-------+
4 rows in set (0.00 sec)
```
把`select`换成`delete`即可.

## Code
```sql
delete p1 from Person p1, Person p2 where p1.Email = p2.Email and p1.Id > p2.Id;
```

