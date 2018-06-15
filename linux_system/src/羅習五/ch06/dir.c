#include <stdio.h>
#include <dirent.h>

int main(int argc, char **argv)
{
	DIR* dir;
	struct dirent* ent;
	dir = opendir(argv[1]);
	ent = readdir(dir);
	while(ent!=NULL) {
		printf("%s\n",  ent->d_name);
		ent = readdir(dir);
	}
	closedir(dir);
	
	return 0;
}

