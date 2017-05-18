use leetcode;
create table if not exists Employee2(Id int, Name varchar(20), Salary int, ManagerId int default NULL);
/*
insert into Employee2 values(1, "Joe", 70000, 3);
insert into Employee2 values(2, "Henry", 80000, 4);
insert into Employee2 values(3, "Sam", 60000, NULL);
insert into Employee2 values(4, "Max", 90000, NULL);
*/
/*
select e1.Name as Employee from Employee2 as e1
where e1.ManagerId is not NULL
and e1.Salary > (select Salary from Employee2 where Id=e1.ManagerId);
*/
select e1.Name as Employee from Employee2 as e1
join Employee2 as e2
on e1.ManagerId = e2.Id and e1.Salary > e2.Salary
where e1.ManagerId is not NULL;
