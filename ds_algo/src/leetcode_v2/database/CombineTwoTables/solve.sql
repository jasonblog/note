use leetcode;
create table if not exists Person(PersonId int,FirstName varchar(20),LastName varchar(20));
create table if not exists Address(AddressId int, PersonId int, City varchar(20), State varchar(20));
select FirstName, LastName, City, State from Person left join Address on Person.PersonId = Address.PersonId;
