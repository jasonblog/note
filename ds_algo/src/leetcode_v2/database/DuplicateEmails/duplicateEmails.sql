use leetcode;
drop table if exists Person;
create table Person(Id int, Email varchar(20));
insert into Person values(1, "a@b.com");
insert into Person values(2, "c@d.com");
insert into Person values(3, "a@b.com");
/*
select Email from (select Email, count(*)  count from Person  group by Email) as e where e.count > 1;
*/
select Email from Person group by Email having count(Email) > 1;
