#!/bin/bash

user=$USER

target='gameserver'
stoptime=`date +%s`
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
for pid in $pids
do
	# 给gamesvrd发送关闭的信号
	kill -USR2 $pid 
	echo "shutdown gamesvrd pid:$pid"
done

echo "shutdown server now,please wait"

while [ 0 -eq 0 ]
do
#	lines=`sed -n '/Save all data finished. now exit./p' ../gameserver/log/gameserver.log`
#	grep "?*Send To Client Succeed?*" ../gameserver/log/gameserver.log > tmpfile
	# 每隔1秒钟遍历一次gameserver.log日志,查看是否存储所有数据完毕
	grep "?*Save all data finished. now exit.?*" ../log/gameserver.log > tmpfile
	while read line
	do
		line1=${line#*'['}
		line2=${line1%%']'*}
#		echo $line2
		linetime=`date +%s -d "$line2"`
#		echo $linetime
		if [ $linetime -gt $stoptime ]; then
			# 存储所有数据完毕,执行停服脚本关闭所有服务器
#			echo "$linetime > $stoptime"
			echo "stop server save all data has finished, now stop server with stop.sh."
			./stop.sh
			rm tmpfile
			exit
		fi
#		elif [ $linetime -eq $stoptime ]; then
#			echo "$linetime = $stoptime"
#		else
#			echo "$linetime < $stoptime"
#		fi
	done < tmpfile
	
	sleep 1
done


