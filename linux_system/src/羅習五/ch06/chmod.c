#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
	int perm;
	int owner, grp, others;
	sscanf(argv[2], "%1d%1d%1d", &owner, &grp, &others);
	printf("permission = %d %d %d\n", owner, grp, others);
	perm = owner<<6 | grp<<3 | others;
	chmod(argv[1], (mode_t)perm);
	
	return 0;
}

