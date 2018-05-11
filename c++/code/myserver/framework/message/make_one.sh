#!/bin/sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
#cd ./proto/
filename=$1
if [ "$filename" = "" ];then
	echo "you have to input a proto file name"
	exit 1
else
	protoc -I=. --cpp_out=../ $filename
	echo "已生成$filename.cpp"
fi

