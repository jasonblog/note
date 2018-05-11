#!/bin/sh

user=$USER

target='gameserver'
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
for pid in $pids
do
	kill  -USR1 $pid
done


