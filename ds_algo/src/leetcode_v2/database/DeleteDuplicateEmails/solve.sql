use leetcode;
drop table if exists Person;
create table Person(id int, Email varchar(20));
insert into Person values (1, "a"),(2, "a"),(3,"b"),(4,"b"),(5,"b"), (6, "c");
/*
delete p from Person as p where p.Id not in (select Id from (select min(Person.Id) as Id from Person group by Person.Email) as NewPerson);
*/
/*
delete p1 from Person p1, Person p2 where p1.Email = p2.Email and p1.Id > p2.Id;
*/
select * from Person;
