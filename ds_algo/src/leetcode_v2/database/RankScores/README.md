# Rank Scores

## Problem

Write a SQL query to rank scores. If there is a tie between two scores, both should have the same ranking. Note that after a tie, the next ranking number should be the next consecutive integer value. In other words, there should be no "holes" between ranks.
```
+----+-------+
| Id | Score |
+----+-------+
| 1  | 3.50  |
| 2  | 3.65  |
| 3  | 4.00  |
| 4  | 3.85  |
| 5  | 4.00  |
| 6  | 3.65  |
+----+-------+
```
For example, given the above Scores table, your query should generate the following report (order by highest score):
```
+-------+------+
| Score | Rank |
+-------+------+
| 4.00  | 1    |
| 4.00  | 1    |
| 3.85  | 2    |
| 3.65  | 3    |
| 3.65  | 3    |
| 3.50  | 4    |
+-------+------+
```

## Solution

这个题目比较难。需要得到排名，我们可以换个思路，即需要统计每个元素它大于等于的有多少个元素,
比如{4.00: [4.00]}, 而3.85: [4.00, 3.85], {3.65:[4.00,3.85,3.65]}, {3.50:[4.00,3.85,3.65,3.5]}


要得到这个列表，只需要做一个内连接操作，即
```sql
select Scores.Score , Ranking.Score
	from Scores
	join (select distinct Score from Scores) Ranking
	on Scores.Score <= Ranking.Score
	order by Scores.Score desc;
```
输出为

```
4.00	4.00
4.00	4.00
3.85	4.00
3.85	3.85
3.65	3.65
3.65	4.00
3.65	4.00
3.65	3.85
3.65	3.85
3.65	3.65
3.50	4.00
3.50	3.85
3.50	3.50
3.50	3.65
```
我们可以得到排名为4.00:1 3.85: 2, 3.65: 3, 3.50:4, 只需要按照id分组，统计即可
```sql
select Scores.Score as Score, COUNT(*) as Rank
from Scores
join (select distinct Score from Scores) Ranking
on Scores.Score <= Ranking.Score
group by Scores.Id
order by Scores.Score desc;
```
