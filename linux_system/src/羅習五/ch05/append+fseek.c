#include <stdio.h>
int main() {
	FILE *stream;
	char buf[100];
	int ret;
	stream = fopen("./tmp", "a+");
	ret = fseek(stream, 200, SEEK_SET);
	fread(buf, 100, 1, stream);
	printf(buf);
	printf("\nreturn value = %d\n", ret);
	fprintf(stream, "append?");
	printf("position = %d\n", ftell(stream));
	return 0;
}
