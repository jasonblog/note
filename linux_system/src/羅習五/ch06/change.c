#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char**argv) {
	int uid;
	
	sscanf(argv[1], "%d", &uid);
	setuid(0);
	setuid(uid);
	system("/bin/bash");
	return 0;
}
