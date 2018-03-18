#!/bin/bash
file='file.txt'
len=$(awk 'FNR==1{print NF}' $file)
for i in $(seq 1 $len)
do
	cat $file | cut -d' ' -f $i | tr '\n' ' ' | sed 's/ $//g'
	echo
done
