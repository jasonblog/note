use leetcode;
drop table if exists Employee;
create table Employee(Id int, Name varchar(20), Salary int, DepartmentId int);
insert into Employee values
(1, "Joe", 70000, 1),
(2, "Henry", 80000, 2),
(3,"Sam", 60000, 2),
(4,"Max", 90000, 1);

drop table if exists Department;
create table Department(Id int, Name varchar(20));
insert into Department values
(1,"IT"),
(2,"Sales");

select DepartmentId, max(Salary) as max from Employee group by DepartmentId;
/*
select d.Name as Department, e1.Name as Employee, e1.Salary as Salary
from Employee as e1
join (select DepartmentId, max(Salary) as max from Employee group by DepartmentId) as t
on e1.DepartmentId = t.DepartmentId
join Department as d
on e1.DepartmentId = d.Id
where e1.Salary = max;
*/

/*
select d.Name as Department, e.Name as Employee, max(e.Salary) as Salary
from Employee as e
join Department as d on e.DepartmentId = d.Id
group by e.DepartmentId;
*/
