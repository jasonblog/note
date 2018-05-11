#!/bin/sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

rm *.pb.h *.pb.cc 1>/dev/null 2>&1
cd ./proto/
for i in *.proto
do
	protoc -I=. --cpp_out=. $i
	if [ "$?" -eq 0 ]
	then
		echo "已生成$i.pb.h $i.pb.cc"
	else
		exit 1
	fi
done

mv ./*.pb.* ../
