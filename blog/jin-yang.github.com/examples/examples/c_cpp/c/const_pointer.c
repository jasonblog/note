#include <stdio.h>

int main(int argc, char **argv)
{
	int Array[] = {1, 2, 3, 4};
	int i, result, *ptr = Array;

	for(i = 0; i < 4; i++){
		printf("Array[%i] = %i\t", i, Array[i]);
	}
	putchar('\n'); putchar('\n');

	result = *ptr++;
	printf("After *ptr++, \n\tresult = %i, *ptr = %i \n\t", result, *ptr);
	for(i = 0; i < 4; i++){
		printf("Array[%i] = %i\t", i, Array[i]);
	}
	putchar('\n'); putchar('\n');

	Array[0] = 1;	Array[1] = 2;	Array[2] = 3;	Array[3] = 4; ptr = Array;
	result = ++*ptr;
	printf("After ++*ptr, \n\tresult = %i, *ptr = %i\n\t", result, *ptr);
	for(i = 0; i < 4; i++){
		printf("Array[%i] = %i\t", i, Array[i]);
	}
	putchar('\n'); putchar('\n');

	Array[0] = 1;	Array[1] = 2;	Array[2] = 3;	Array[3] = 4; ptr = Array;
	result = *ptr++;
	printf("After *ptr++, \n\tresult = %i, *ptr = %i\n\t", result, *ptr);
	for(i = 0; i < 4; i++){
		printf("Array[%i] = %i\t", i, Array[i]);
	}
	putchar('\n'); putchar('\n');

	Array[0] = 1;	Array[1] = 2;	Array[2] = 3;	Array[3] = 4; ptr = Array;
	result = (*ptr)++;
	printf("After (*ptr)++, \n\tresult = %i, *ptr = %i\n\t", result, *ptr);
	for(i = 0; i < 4; i++){
		printf("Array[%i] = %i\t", i, Array[i]);
	}
	putchar('\n');

	return 0;
}
