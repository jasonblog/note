use leetcode;
delimiter $$ /* 由于函数里要使用分号分隔符，这里要定义另一个分隔符 */

create procedure simpleproc(OUT count int)
begin
	select count(*) into count from Employee;
end$$

delimiter ;
/* 使用call调用过程 */
call simpleproc(@a);
/* 输出变量a的值 */

select @a;
