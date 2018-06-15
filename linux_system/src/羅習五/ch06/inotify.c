#include <sys/inotify.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

void printInotifyEvent(struct inotify_event* event);

char wd[1000][10000];

int main(int argc, char **argv) {
	int fd, num, ret, i;
	char* p;
	char inotify_entity[BUF_LEN];
	fd = inotify_init();
	for (i=1; i< argc; i++) {
		ret=inotify_add_watch(fd, argv[i], IN_ALL_EVENTS);
		strcpy(wd[ret], argv[i]);
	}
	while(1) {
		num = read(fd, inotify_entity, BUF_LEN);
		for (p = inotify_entity; p < inotify_entity + num; ) {
			printInotifyEvent((struct inotify_event *) p);
			p+=sizeof(struct inotify_event) + ((struct inotify_event *)p)->len;
		}
	}
}

void printInotifyEvent(struct inotify_event* event) {
	char buf[4096]="";
	sprintf(buf, "[%s] ", wd[event->wd]);
	strncat(buf, "{", 4096);
	if (event->mask & IN_ACCESS) 		strncat(buf, "ACCESS, ", 4096);
	if (event->mask & IN_ATTRIB) 		strncat(buf,"ATTRIB, ", 4096);
	if (event->mask & IN_CLOSE_WRITE) 	strncat(buf,"CLOSE_WRITE, ", 4096);
	if (event->mask & IN_CLOSE_NOWRITE) 	strncat(buf,"CLOSE_NOWRITE, ", 4096);
	if (event->mask & IN_CREATE) 		strncat(buf,"CREATE, ", 4096);
	if (event->mask & IN_DELETE) 		strncat(buf,"DELETE, ", 4096);
	if (event->mask & IN_DELETE_SELF) 	strncat(buf,"DELETE_SELF, ", 4096);
	if (event->mask & IN_MODIFY) 		strncat(buf,"MODIFY, ", 4096);
	if (event->mask & IN_MOVE_SELF)		strncat(buf,"MOVE_SELF, ", 4096);
	if (event->mask & IN_MOVED_FROM)	strncat(buf,"MOVED_FROM, ", 4096);
	if (event->mask & IN_MOVED_TO)		strncat(buf,"MOVED_TO, ", 4096);
	if (event->mask & IN_OPEN)			strncat(buf,"OPEN, ", 4096);
	if (event->mask & IN_IGNORED)		strncat(buf,"IGNORED, ", 4096);
	if (event->mask & IN_ISDIR)			strncat(buf,"ISDIR, ", 4096);
	if (event->mask & IN_Q_OVERFLOW)	strncat(buf,"Q_OVERFLOW, ", 4096);
	buf[strlen(buf)-2]='\0';
	strncat(buf, "}", 4096);
	sprintf(buf, "%s cookie=%d", buf, event->cookie);
	if (event->len>0)
    	sprintf(buf, "%s name = %s\n", buf, event->name);
	else
		sprintf(buf, "%s name = null\n", buf);
	printf("%s", buf);
}
