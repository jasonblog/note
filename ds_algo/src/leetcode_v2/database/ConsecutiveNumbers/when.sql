use leetcode;
select Num,
	case
		when Num % 2 = 0 then "Even"
		when Num % 2 = 1 then "Odd"
	end Numtype 
from Logs;

select Num from (
SELECT
        Num,
        CASE
            WHEN @prevNum = Num THEN @count := @count + 1
            WHEN @prevNum := Num THEN @count := 1
        END n
FROM Logs, (select @prevNum := NULL) initPrevNum, (select @count := 1) initCount
) as result where result.n >= 3;
/*
SELECT * FROM (
    SELECT
        `Num`,
        CASE
            WHEN @prevNum = `Num` THEN @count := @count + 1
            WHEN @prevNum := `Num` THEN @count := 1
        END n
    FROM `Logs`, (select @prevNum := NULL) r, (select @count := 1) s
) a;

*/
