#!/bin/sh

user=$USER

target='proxyserver'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	kill -9 $pid
done

target='gameserver'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	kill -9 $pid
done

target='dbserver'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	kill -9 $pid
done

target='gateserver'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	kill -9 $pid
done

sleep 1
echo "Delete share memory"
shmid=`ipcs -m | grep -w $user | awk '$6==0{printf " -m  " $2  " "}'`
echo "$shmid"
ipcrm $shmid

