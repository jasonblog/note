use leetcode;
drop table if exists Logs;
create table Logs(Id int, Num int);
insert into Logs values(1,1);
insert into Logs values(2,1);
insert into Logs values(3,1);
insert into Logs values(4,2);
insert into Logs values(5,1);
insert into Logs values(6,2);
insert into Logs values(7,2);
insert into Logs values(8,2);
insert into Logs values(9,1);
insert into Logs values(10,1);
insert into Logs values(11,1);
insert into Logs values(12,3);
insert into Logs values(13,3);
insert into Logs values(14,3);
insert into Logs values(15,4);
insert into Logs values(16,4);
insert into Logs values(17,5);
insert into Logs values(18,5);

select "Method 1";
select log1.Num from Logs log1 join Logs log2 on log1.Id + 1 = log2.Id and log1.Num = log2.Num group by log1.Num having count(log1.Num) >= 2;

select "Method 2";
select distinct a.Num from Logs a, Logs b,Logs c where a.Id=b.Id+1 and a.Num=b.Num and b.Id=c.Id+1 and b.Num=c.Num;

select "Method 3";
SELECT DISTINCT `Num` FROM (
    SELECT
        `Num`,
        CASE
            WHEN @prevNum = `Num` THEN @count := @count + 1
            WHEN (@prevNum := `Num`) IS NOT NULL THEN @count := 1
        END n
    FROM `Logs`, (SELECT @prevNum := NULL) r, (SELECT @count := 1) c
) a WHERE n >= 3;

select "Method 4";
SELECT DISTINCT `Num` FROM (
    SELECT
        `Num`,
        CASE
            WHEN @prevNum = `Num` THEN @count := @count + 1
            WHEN (@prevNum := `Num`) IS NOT NULL THEN @count := 1
        END n
    FROM `Logs`
) a WHERE n >= 3;
