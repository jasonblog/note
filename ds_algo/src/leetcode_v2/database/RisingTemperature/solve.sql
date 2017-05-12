use leetcode;
drop table if exists Weather;
create table Weather(Id INT, Date DATE, Temperature INT);
insert into Weather values
(1, Date("2015-01-01"), 10),
(2, Date("2015-01-02"), 25),
(3, Date("2015-01-03"), 20),
(4, Date("2015-01-04"), 30);
select w1.Id from Weather w1, Weather w2 where TO_DAYS(w1.DATE) = TO_DAYS(w2.DATE) + 1 and w1.Temperature > w2.Temperature;
