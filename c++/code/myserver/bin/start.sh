#!/bin/sh
user=$USER
sh stop.sh

export TRITON_HOME=/home/dguco/data/cpppro/myserver
chmod +x $TRITON_HOME/bin/

rm ../log/* -f

cd $TRITON_HOME/bin
target='proxyserver'
./$target
sleep 1

cd $TRITON_HOME/bin
target='gateserver'
./$target
sleep 1

cd $TRITON_HOME/bin
target='dbserver'
./$target
sleep 1

export PIPE_ID=1
export SC_PIPE_ID=2
export CS_PIPE_ID=3

target='gameserver'
echo $target
cd $TRITON_HOME/bin
./$target
sleep 1

ipcs -m | grep $user
ps aux | grep $user | grep server | grep -v grep

