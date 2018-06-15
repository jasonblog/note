#include <stdio.h>
#include <stdlib.h>

#define size 10000
long table[size][size];
long col[size];
long row[size];

void initTable() {
	int i, j;
	for (i=0; i< size; i++)
		for (j=0; j<size; j++)
			table[i][j]=random();
}

void sumCol() {
	int i, j;
	for (j=0; j<size; j++)
		for (i=0; i<size; i++)
			col[j] = table[i][j];
}


void sumRow() {
	int i, j;
	for (i=0; i<size; i++)
		for (j=0; j<size; j++)
			row[i] = table[i][j];
}

void printResult() {
	int i;
	printf("     RAW\tCol\n");
	for (i=0; i<size; i++)
		printf("%8ld\t%8ld\n", row[i], col[i]);
}

int main() {
	printf("hello\n");
	initTable();
	sumRow();
	sumCol();
	printResult();
	return 0;
}
