#!/bin/bash
declare -A count
for word in $(cat words.txt)
do
	let count[$word]++
done
(for word in ${!count[@]}
do
	printf "%s %d\n" $word ${count[$word]}
done) | sort -n -r -t ' ' -k 2
