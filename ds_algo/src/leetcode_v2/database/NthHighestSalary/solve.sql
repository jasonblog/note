use leetcode;
drop table if exists Employee;
create table Employee(Id int, Salary int);
/*
insert into Employee values (1, 300),(2,200),(3,100),(4,500),(5,700),(6,400);
*/
insert into Employee values (1, 200),(2,200);
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
select getNthHighestSalary(2);
