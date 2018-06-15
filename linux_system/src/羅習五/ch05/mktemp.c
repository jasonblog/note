#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main() {
	FILE* stream;
	char tmpStr[] = "./shiwulo_XXXXXX";
	mktemp(tmpStr);
	printf("%s\n", tmpStr);
	stream = fopen(tmpStr, "w+");	/*600*/
	if (stream == NULL)
		perror("error: ");
	fputs("hello\0", stdout);
	return 0;
}
