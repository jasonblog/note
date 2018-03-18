#!/bin/bash
awk '
{
	for (i = 1; i <= NF; i++) {
		if (NR == 1) {
			result[i] = $i;
		} else {
			result[i] = result[i] " " $i
		}
	}
}
END {
	for (i = 1; result[i] != ""; i++) {
		print result[i];
	}
}' file.txt
