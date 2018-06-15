#include <stdio.h>
#include <errno.h>

int main() {
	errno=1;
	perror("the error is");
}
