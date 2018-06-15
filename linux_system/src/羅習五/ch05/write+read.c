#include <stdio.h>
int main(int argc, char **argv) {
	FILE* file;
	file = fopen("./tmp", argv[1]);
	fprintf(file, "this_is_a_tmp_file\n");;
	fclose(file);
	return 0;
}