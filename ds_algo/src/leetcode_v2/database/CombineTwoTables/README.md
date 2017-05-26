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


Write a SQL query for a report that provides the following information for each person in the Person table, regardless if(無論如何） there is an address for each of those people:

```
FirstName, LastName, City, State
```

## Solution

無論地址是否存在，都輸出每個Person，顯然是左連接操作。

```sql
select FirstName, LastName, City, State 
from Person left join Address on Person.PersonId = Address.PersonId;
```

## 表連接總結

表的連接方式大體分為:

* 內連接
* 外連接
* 交叉連接

### 內連接 VS 外連接

假設有兩個表A和B，內連接是A和B的交集,即輸出的內容一定在A和B中都有的。而外連接除了A和B的交集，還有可能只存在於A或B的，只存在A而不存在B的，B列用null輸出，
換句話說包括其中一個或者兩個表的全集，如果另外一個表不滿足連接條件，輸出null。

等值連接是特殊的內連接，其連接條件是等於關係。而自然連接又是特殊的等值連接，它會去除重複屬性。

外連接根據全集端不同，又分為左外連接(簡稱左連接），右外連接（簡稱右連接）和全連接。

mysql中使用join進行連接操作，默認為內連接。

下面用具體的實例看看不同的連接操作：
首先Person表內容為：
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
### 內連接
```
mysql> select * from Person join Address on Person.PersonId = Address.PersonId;
+----------+-----------+----------+-----------+----------+---------+---------+
| PersonId | FirstName | LastName | AddressId | PersonId | City    | State   |
+----------+-----------+----------+-----------+----------+---------+---------+
|        2 | Mary      | John     |         1 |        2 | BeiJing | BeiJing |
+----------+-----------+----------+-----------+----------+---------+---------+
```
可見內連接只輸出滿足連接條件的行。

### 左連接
```
mysql> select * from Person left join Address on Person.PersonId = Address.PersonId;
+----------+-----------+----------+-----------+----------+---------+---------+
| PersonId | FirstName | LastName | AddressId | PersonId | City    | State   |
+----------+-----------+----------+-----------+----------+---------+---------+
|        2 | Mary      | John     |         1 |        2 | BeiJing | BeiJing |
|        1 | Wang      | Allen    |      NULL |     NULL | NULL    | NULL    |
+----------+-----------+----------+-----------+----------+---------+---------+
```
可見，左連接輸出左邊表的全集，不滿足條件的右表輸出NULL。

### 右連接
```
mysql> select * from Person right join Address on Person.PersonId = Address.PersonId;
+----------+-----------+----------+-----------+----------+----------+-----------+
| PersonId | FirstName | LastName | AddressId | PersonId | City     | State     |
+----------+-----------+----------+-----------+----------+----------+-----------+
|        2 | Mary      | John     |         1 |        2 | BeiJing  | BeiJing   |
|     NULL | NULL      | NULL     |         2 |        5 | ShaoGuan | GuangDong |
+----------+-----------+----------+-----------+----------+----------+-----------+
```
結果不言而喻。

### 全連接

mysql不直接支持全連接，可以分別左連接、右連接，然後求並集實現

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

### 交叉連接

交叉連接相當於無連接條件，左右一一連接，形成笛卡爾積。若|A| = n, |B| = m 則 |A cross join B| = n * m

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

### 自然連接 VS 等值連接

自然連接是除去重複屬性的等值連接。

兩者之間的區別和聯繫如下:

1. 自然連接一定是等值連接，但等值連接不一定是自然連接。等值連接不把重複的屬性除去；而自然連接要把重複的屬性除去。

2. 等值連接要求相等的分量，不一定是公共屬性；而自然連接要求相等的分量必須是公共屬性。


