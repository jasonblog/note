#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define size 10000
float table[size][size];
float col[size];
float row[size];

void initTable()
{
	int i, j;
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			table[i][j] *= 10.0;
}

void sumCol()
{
	int i, j;
	for (j = 0; j < size; j++)
		for (i = 0; i < size; i++)
			col[j] *= table[i][j];
}

void sumRow()
{
	int i, j;
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			row[i] *= table[i][j];
}


int main()
{
	initTable();
	table[random()%size][random()%size] = (float)random();
	sumRow();
	sumCol();
	printf("%f\n", col[random()%size]+row[random()%size]);
}
