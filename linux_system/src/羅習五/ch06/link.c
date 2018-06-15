#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv)
{
	int ret;
	printf("%d \n", argc);
	if (argc == 4) {/*softlink*/
		ret = symlink(argv[2], argv[3]);
		printf("%s, %s\n", argv[2], argv[3]);
		if (ret != 0) perror("soft link:");
	} else {
		ret = link(argv[1], argv[2]);
		printf("%s, %s\n", argv[1], argv[2]);
		if (ret != 0) perror("hard link");
	}
	return 0;
}

