use leetcode;
create table if not exists Scores(Id int, Score decimal(7, 2));
/*
insert into Scores values(1, 3.50);
insert into Scores values(2, 3.65);
insert into Scores values(3, 4.00);
insert into Scores values(4, 3.85);
insert into Scores values(5, 4.00);
insert into Scores values(6, 3.65);
*/
select Scores.Score as Score, COUNT(*) as Rank
from Scores
join (select distinct Score from Scores) Ranking
on Scores.Score <= Ranking.Score
group by Scores.Id
order by Scores.Score desc;
/*
select Scores.Score , Ranking.Score
	from Scores
	join (select distinct Score from Scores) Ranking
	on Scores.Score <= Ranking.Score
	order by Scores.Score desc;
*/
