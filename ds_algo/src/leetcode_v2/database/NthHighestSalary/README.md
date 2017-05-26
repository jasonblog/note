## Nth Highest Salary

Write a SQL query to get the nth highest salary from the `Employee` table.
```
+----+--------+
| Id | Salary |
+----+--------+
| 1  | 100    |
| 2  | 200    |
| 3  | 300    |
+----+--------+
```
For example, given the above `Employee` table, the nth highest salary where `n = 2 is 200`. If there is no nth highest salary, then the query should return `null`.

## Solution

同 [SecondHighestSalary](../SecondHighestSalary)

## mysql 函數

mysql 可以自定義過程（無返回值）和函數（有返回值），參數也可以定義為`IN`或者`OUT`,`IN`就是傳人的參數,`OUT`就是要寫進去的參數（有點像傳引用）

語法:
```mysql
CREATE
    [DEFINER = { user | CURRENT_USER }]
    PROCEDURE sp_name ([proc_parameter[,...]])
    [characteristic ...] routine_body

CREATE
    [DEFINER = { user | CURRENT_USER }]
    FUNCTION sp_name ([func_parameter[,...]])
    RETURNS type
    [characteristic ...] routine_body

proc_parameter:
    [ IN | OUT | INOUT ] param_name type

func_parameter:
    param_name type

type:
    Any valid MySQL data type

characteristic:
    COMMENT 'string'
  | LANGUAGE SQL
  | [NOT] DETERMINISTIC
  | { CONTAINS SQL | NO SQL | READS SQL DATA | MODIFIES SQL DATA }
  | SQL SECURITY { DEFINER | INVOKER }

routine_body:
    Valid SQL routine statement
```
定義例子:
```mysql
delimiter //
CREATE PROCEDURE simpleproc (OUT param1 INT)
 BEGIN
	SELECT COUNT(*) INTO param1 FROM t;
END//
```
使用`call simpleproc(@count)`調用以上過程，然後`select @count`輸出結果

定義函數例子:

```mysql
delimiter $$
drop function if exists getNthHighestSalary $$
CREATE FUNCTION getNthHighestSalary(N INT) RETURNS INT
BEGIN
	declare x int;
	set N = N - 1;
	set x = (select distinct Salary from Employee order by Salary desc limit N, 1);
	if isnull(x)
		then
		return null;
	else
		return x;
	end if;
END$$
delimiter ;
```
調用`select hello('world');`輸出`Hello, world!`

## mysql變量

mysql使用`declare 變量名 變量類型`聲明變量, 使用`set 變量名 = 值`設置變量值

## mysql分支循環

```
if語句
if(condition) then s1;
elseif (condition) then s2;
else s3;
end if;

[begin_label:]while 循環
while condition do
s1;
s2;
end while[end_label]
```

## Code
```mysql
CREATE FUNCTION getNthHighestSalary(N INT) RETURNS INT
BEGIN
	declare x int;
	set N = N - 1;
	set x = (select distinct Salary from Employee order by Salary desc limit N, 1);
	if isnull(x)
		then
		return null;
	else
		return x;
	end if;
END
```
