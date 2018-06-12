#include <stdio.h>
int global=0;
int main(int argc, char **argv)
{
	int i;
	for (i=0; i<2000000000; i++) {
		global+=1;
	}
	printf("1000000000+1000000000 = %d\n", global);
	return 0;
}