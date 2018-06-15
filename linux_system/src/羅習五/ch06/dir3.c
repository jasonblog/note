#include <stdio.h>
#include <dirent.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

int main(int argc, char **argv)
{
	DIR *dir;
	struct dirent *ent;
	char *curDir = "./";
	char pathname[512];
	struct stat buf;
	int perm;
	char *time;

	dir = opendir(argv[1]);
	ent = readdir(dir);
	while (ent != NULL)
	{
		strcpy(pathname, curDir);
		strcat(pathname, ent->d_name);
		stat(pathname, &buf);
		perm = (buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
		time = ctime(&buf.st_atime);
		time[strlen(time) - 1] = 0;
		printf("%o  %8s  %8s %8d %s %s\n", perm, getpwuid(buf.st_uid)->pw_name, 
			getgrgid(buf.st_gid)->gr_name, (int)buf.st_size, time, ent->d_name);
		ent = readdir(dir);
	}
	closedir(dir);
	return 0;
}
