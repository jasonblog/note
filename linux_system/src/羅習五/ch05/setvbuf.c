#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE* stream;
	int bufSize;
	int dataSize = 10000000;
	char *buf;
	int i;
	
	stream = fopen("./tmp", "w+");
	sscanf(argv[1], "%d", &bufSize);
	buf = (char*)malloc(bufSize);
	setvbuf(stream, buf, _IOFBF, bufSize);
	for (i=0; i<dataSize; i++)
		fwrite("d", 1, 1, stream);
	return 0;
}

