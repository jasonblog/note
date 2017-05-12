# Combine Two Tables

## Problem
Table: **Person**

```
+-------------+---------+
| Column Name | Type    |
+-------------+---------+
| PersonId    | int     |
| FirstName   | varchar |
| LastName    | varchar |
+-------------+---------+
```
PersonId is the primary key column for this table.

Table: **Address**

```
+-------------+---------+
| Column Name | Type    |
+-------------+---------+
| AddressId   | int     |
| PersonId    | int     |
| City        | varchar |
| State       | varchar |
+-------------+---------+
```
AddressId is the primary key column for this table.


Write a SQL query for a report that provides the following information for each person in the Person table, regardless if(无论如何） there is an address for each of those people:

```
FirstName, LastName, City, State
```

## Solution

无论地址是否存在，都输出每个Person，显然是左连接操作。

```sql
select FirstName, LastName, City, State 
from Person left join Address on Person.PersonId = Address.PersonId;
```

## 表连接总结

表的连接方式大体分为:

* 内连接
* 外连接
* 交叉连接

### 内连接 VS 外连接

假设有两个表A和B，内连接是A和B的交集,即输出的内容一定在A和B中都有的。而外连接除了A和B的交集，还有可能只存在于A或B的，只存在A而不存在B的，B列用null输出，
换句话说包括其中一个或者两个表的全集，如果另外一个表不满足连接条件，输出null。

等值连接是特殊的内连接，其连接条件是等于关系。而自然连接又是特殊的等值连接，它会去除重复属性。

外连接根据全集端不同，又分为左外连接(简称左连接），右外连接（简称右连接）和全连接。

mysql中使用join进行连接操作，默认为内连接。

下面用具体的实例看看不同的连接操作：
首先Person表内容为：
```
+----------+-----------+----------+
| PersonId | FirstName | LastName |
+----------+-----------+----------+
|        1 | Wang      | Allen    |
|        2 | Mary      | John     |
+----------+-----------+----------+
```
Address表：

```
+-----------+----------+----------+-----------+
| AddressId | PersonId | City     | State     |
+-----------+----------+----------+-----------+
|         1 |        2 | BeiJing  | BeiJing   |
|         2 |        5 | ShaoGuan | GuangDong |
+-----------+----------+----------+-----------+
```
### 内连接
```
mysql> select * from Person join Address on Person.PersonId = Address.PersonId;
+----------+-----------+----------+-----------+----------+---------+---------+
| PersonId | FirstName | LastName | AddressId | PersonId | City    | State   |
+----------+-----------+----------+-----------+----------+---------+---------+
|        2 | Mary      | John     |         1 |        2 | BeiJing | BeiJing |
+----------+-----------+----------+-----------+----------+---------+---------+
```
可见内连接只输出满足连接条件的行。

### 左连接
```
mysql> select * from Person left join Address on Person.PersonId = Address.PersonId;
+----------+-----------+----------+-----------+----------+---------+---------+
| PersonId | FirstName | LastName | AddressId | PersonId | City    | State   |
+----------+-----------+----------+-----------+----------+---------+---------+
|        2 | Mary      | John     |         1 |        2 | BeiJing | BeiJing |
|        1 | Wang      | Allen    |      NULL |     NULL | NULL    | NULL    |
+----------+-----------+----------+-----------+----------+---------+---------+
```
可见，左连接输出左边表的全集，不满足条件的右表输出NULL。

### 右连接
```
mysql> select * from Person right join Address on Person.PersonId = Address.PersonId;
+----------+-----------+----------+-----------+----------+----------+-----------+
| PersonId | FirstName | LastName | AddressId | PersonId | City     | State     |
+----------+-----------+----------+-----------+----------+----------+-----------+
|        2 | Mary      | John     |         1 |        2 | BeiJing  | BeiJing   |
|     NULL | NULL      | NULL     |         2 |        5 | ShaoGuan | GuangDong |
+----------+-----------+----------+-----------+----------+----------+-----------+
```
结果不言而喻。

### 全连接

mysql不直接支持全连接，可以分别左连接、右连接，然后求并集实现

```
mysql> select * from Person  left join Address on Person.PersonId = Address.PersonId
union 
(select * from Person right join Address on Person.PersonId = Address.PersonId);
+----------+-----------+----------+-----------+----------+----------+-----------+
| PersonId | FirstName | LastName | AddressId | PersonId | City     | State     |
+----------+-----------+----------+-----------+----------+----------+-----------+
|        2 | Mary      | John     |         1 |        2 | BeiJing  | BeiJing   |
|        1 | Wang      | Allen    |      NULL |     NULL | NULL     | NULL      |
|     NULL | NULL      | NULL     |         2 |        5 | ShaoGuan | GuangDong |
+----------+-----------+----------+-----------+----------+----------+-----------+
```

### 交叉连接

交叉连接相当于无连接条件，左右一一连接，形成笛卡尔积。若|A| = n, |B| = m 则 |A cross join B| = n * m

```
mysql> select * from Person  cross join Address;
+----------+-----------+----------+-----------+----------+----------+-----------+
| PersonId | FirstName | LastName | AddressId | PersonId | City     | State     |
+----------+-----------+----------+-----------+----------+----------+-----------+
|        1 | Wang      | Allen    |         1 |        2 | BeiJing  | BeiJing   |
|        2 | Mary      | John     |         1 |        2 | BeiJing  | BeiJing   |
|        1 | Wang      | Allen    |         2 |        5 | ShaoGuan | GuangDong |
|        2 | Mary      | John     |         2 |        5 | ShaoGuan | GuangDong |
+----------+-----------+----------+-----------+----------+----------+-----------+
```

### 自然连接 VS 等值连接

自然连接是除去重复属性的等值连接。

两者之间的区别和联系如下:

1. 自然连接一定是等值连接，但等值连接不一定是自然连接。等值连接不把重复的属性除去；而自然连接要把重复的属性除去。

2. 等值连接要求相等的分量，不一定是公共属性；而自然连接要求相等的分量必须是公共属性。


