use leetcode;
drop table if exists Customers;
create table Customers(Id int, Name varchar(20));
insert into Customers values (1, "Joe"), (2, "Henry"), (3,"Sam"),(4,"Max");

drop table if exists Orders;
create table Orders(Id int, CustomerId int);
insert into Orders values (1,3),(2,1);

select c.Name as Customers
from Customers as c
where not exists (select o.Id from Orders as o where c.Id = o.CustomerId);
