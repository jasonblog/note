#!/bin/sh

user=$USER
curpwd=`pwd`
curpwd=${curpwd%/*}

target='proxyserver'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	svrdpwd=`ls -l /proc/$pid/exe|awk '{print $11}'`
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	if [ $curpwd = $svrdpwd ] ; then
 		kill -9 $pid
	fi
done

target='gameserver'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	svrdpwd=`ls -l /proc/$pid/exe|awk '{print $11}'`
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	if [ $curpwd = $svrdpwd ] ; then
 		kill -9 $pid
	fi
done

target='dbserver'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	svrdpwd=`ls -l /proc/$pid/exe|awk '{print $11}'`
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	echo $svrdpwd
	echo $curpwd
	if [ $curpwd = $svrdpwd ] ; then
 		kill -9 $pid
	fi
done

target='gateserver'
echo "Stop $target"
pids=`ps -ef | grep $target | grep -w $user | grep -v grep | awk '{print $2}'`
echo "pid $pids"
for pid in $pids
do
	svrdpwd=`ls -l /proc/$pid/exe|awk '{print $11}'`
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	svrdpwd=${svrdpwd%/*}
	if [ $curpwd = $svrdpwd ] ; then
 		kill -9 $pid
	fi
done

sleep 1
echo "Delete share memory"
shmid=`ipcs -m | grep -w $user | awk '$6==0{printf " -m  " $2  " "}'`
echo "$shmid"
ipcrm $shmid

