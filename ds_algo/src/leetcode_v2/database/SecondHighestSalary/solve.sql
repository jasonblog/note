use leetcode;
create table if not exists Employee(Id int, Salary int);
/*
insert into Employee values(1, 100);
insert into Employee values(2, 200);
insert into Employee values(3, 300);
*/
/*
select max(Salary) as SecondHighestSalary from Employee where Salary < (select max(Salary) from Employee);
*/
select ifnull((select distinct Salary from Employee order by Salary desc limit 1,1), null) as SecondHighestSalary;
