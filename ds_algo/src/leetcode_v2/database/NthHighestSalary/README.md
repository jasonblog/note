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

## mysql 函数

mysql 可以自定义过程（无返回值）和函数（有返回值），参数也可以定义为`IN`或者`OUT`,`IN`就是传人的参数,`OUT`就是要写进去的参数（有点像传引用）

语法:
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
定义例子:
```mysql
delimiter //
CREATE PROCEDURE simpleproc (OUT param1 INT)
 BEGIN
	SELECT COUNT(*) INTO param1 FROM t;
END//
```
使用`call simpleproc(@count)`调用以上过程，然后`select @count`输出结果

定义函数例子:

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
调用`select hello('world');`输出`Hello, world!`

## mysql变量

mysql使用`declare 变量名 变量类型`声明变量, 使用`set 变量名 = 值`设置变量值

## mysql分支循环

```
if语句
if(condition) then s1;
elseif (condition) then s2;
else s3;
end if;

[begin_label:]while 循环
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
