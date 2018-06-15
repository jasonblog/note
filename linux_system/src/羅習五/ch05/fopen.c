#include <stdio.h>

int main(int argc, char **argv)
{
	FILE* file;
	file = fopen("./tmp", "w");
	fprintf(file, "this is a tmp file\n");
	return 0;
}

