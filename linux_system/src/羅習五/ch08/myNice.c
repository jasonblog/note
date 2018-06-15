/*
目的：了解如何調整優先權
用法：myNice 接一個數字（可以是正整數，或者負整數）
測試：
myNice 10	//優先權降低10
myNice -10	//優先權提高10，如果不是超級使用者無法提高優先權，因此要用sudo myNice -10
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv) {
	int niceVal;
	int newNiceVal;
	int i;
	int ret=0;
	sscanf(argv[1], "%d", &niceVal);
	errno =0;
	newNiceVal = nice(niceVal);
	if (newNiceVal == -1 && errno !=0)
		perror("Error: nice");
	else {
		printf("new val = %d\n", newNiceVal);
	}

	for (int i=0; i<=500000000; i++) {
		if (i%50000000 == 0) fprintf(stderr, "*");
		ret+=i;
	}
	return ret;
}

