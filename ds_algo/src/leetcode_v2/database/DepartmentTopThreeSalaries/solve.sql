use leetcode;
drop table if exists Employee;
create table Employee(Id int, Name varchar(20), Salary int, DepartmentId int);
insert into Employee values
(1, "Joe", 70000, 1),
(2, "Henry", 80000, 2),
(3,"Sam", 60000, 2),
(4,"Max", 90000, 1),
(5, "Janet", 69000, 1),
(6, "Randy", 85000, 1);

drop table if exists Department;
create table Department(Id int, Name varchar(20));
insert into Department values
(1,"IT"),
(2,"Sales");

select top(Salary) from Employee group by DepartmentId;
