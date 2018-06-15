#include <stdio.h>

int main() {
	FILE *stream;
	char buf[5000];
	int ret;
	stream = fopen("./tmp", "a+");
	ret=fread(buf, 10, 500, stream);
	printf("ret = %d\n", ret);
	ret=fread(buf, 10, 500, stream);
	printf("ret = %d\n", ret);
	if(ret!=1) {
		if (ferror(stream))
			printf("error\n");
		if (feof(stream))
			printf("EOF\n");
	}
	return 0;
}
