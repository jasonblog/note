#include <pwd.h>
#include <grp.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/syslog.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <net/if.h>
#include <sys/ioctl.h>

#define MAXBUF        1024
#define MAXPATH        128

char buffer[MAXBUF + 1];
char ip[128];
char port[128];
char back[128];
char home_dir[128];


void wrtinfomsg(char* msg)
{
    syslog(LOG_INFO, "%s", msg);
}


int get_arg(char* cmd)
{

    FILE* fp;
    char buffer[1024];
    size_t bytes_read;
    char* match;
    fp = fopen("/etc/test_httpd.conf", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    if (bytes_read == 0 || bytes_read == sizeof(buffer)) {
        return 0;
    }

    buffer[bytes_read] = '\0';

    if (!strncmp(cmd, "home_dir", 8)) {
        match = strstr(buffer, "home_dir=");

        if (match == NULL) {
            return 0;
        }

        bytes_read = sscanf(match, "home_dir=%s", home_dir);
        return bytes_read;
    }

    else if (!strncmp(cmd, "port", 4)) {
        match = strstr(buffer, "port=");

        if (match == NULL) {
            return 0;
        }

        bytes_read = sscanf(match, "port=%s", port);
        return bytes_read;
    }

    else if (!strncmp(cmd, "ip", 2)) {
        match = strstr(buffer, "ip=");

        if (match == NULL) {
            return 0;
        }

        bytes_read = sscanf(match, "ip=%s", ip);
        return bytes_read;
    } else if (!strncmp(cmd, "back", 4)) {
        match = strstr(buffer, "back=");

        if (match == NULL) {
            return 0;
        }

        bytes_read = sscanf(match, "back=%s", back);
        return bytes_read;
    } else {
        return 0;
    }
}


char file_type(mode_t st_mode)
{
    if ((st_mode & S_IFMT) == S_IFSOCK) {
        return 's';
    } else if ((st_mode & S_IFMT) == S_IFLNK) {
        return 'l';
    } else if ((st_mode & S_IFMT) == S_IFREG) {
        return '-';
    } else if ((st_mode & S_IFMT) == S_IFBLK) {
        return 'b';
    } else if ((st_mode & S_IFMT) == S_IFCHR) {
        return 'c';
    } else if ((st_mode & S_IFMT) == S_IFIFO) {
        return 'p';
    } else {
        return 'd';
    }
}

//search the up-path of dirpath
char* dir_up(char* dirpath)
{
    static char Path[MAXPATH];
    int len;

    strcpy(Path, dirpath);
    len = strlen(Path);

    if (len > 1 && Path[len - 1] == '/') {
        len--;
    }

    while (Path[len - 1] != '/' && len > 1) {
        len--;
    }

    Path[len] = 0;
    return Path;
}


//send the path data to client ;if path is a file ,send the data, if path is a dir, list
void GiveResponse(FILE* client_sock, char* Path)
{
    struct dirent* dirent;
    struct stat info;
    char Filename[MAXPATH];
    DIR* dir;
    int fd, len, ret;
    char* p, *realPath, *realFilename, *nport;

    struct passwd* p_passwd;
    struct group* p_group;
    char* p_time;

    //get th dir or file
    len = strlen(home_dir) + strlen(Path) + 1;
    realPath = malloc(len + 1);
    bzero(realPath, len + 1);
    sprintf(realPath, "%s/%s", home_dir, Path);

    //get port
    len = strlen(port) + 1;
    nport = malloc(len + 1);
    bzero(nport, len + 1);
    sprintf(nport, ":%s", port);


    //get file state to get the information :dir or file
    if (stat(realPath, &info)) {
        fprintf(client_sock,
                "HTTP/1.1 200 OK\r\nServer:Test http server\r\nConnection: close\r\n\r\n<html><head><title>%d - %s</title></head>"
                "<body><font size=+4>Linux HTTP server</font><br><hr width=\"100%%\"><br><center>"
                "<table border cols=3 width=\"100%%\">", errno,
                strerror(errno));
        fprintf(client_sock,
                "</table><font color=\"CC0000\" size=+2> connect to administrator, error code is: \n%s %s</font></body></html>",
                Path, strerror(errno));
        goto out;
    }

    //if file ,send file
    if (S_ISREG(info.st_mode)) {
        fd = open(realPath, O_RDONLY);
        len = lseek(fd, 0, SEEK_END);
        p = (char*) malloc(len + 1);
        bzero(p, len + 1);
        lseek(fd, 0, SEEK_SET);
        ret = read(fd, p, len);
        close(fd);
        fprintf(client_sock,
                "HTTP/1.1 200 OK\r\nServer: Test http server\r\nConnection: keep-alive\r\nContent-type: application/*\r\nContent-Length:%d\r\n\r\n",
                len);

        fwrite(p, len, 1, client_sock);
        free(p);
    } else if (S_ISDIR(info.st_mode)) {

        //if dir,list the dir
        dir = opendir(realPath);
        fprintf(client_sock,
                "HTTP/1.1 200 OK\r\nServer:Test http server\r\nConnection:close\r\n\r\n<html><head><title>%s</title></head>"
                "<body><font size=+4>Linux HTTP server file</font><br><hr width=\"100%%\"><br><center>"
                "<table border cols=3 width=\"100%%\">", Path);
        fprintf(client_sock,
                "<caption><font size=+3> Directory %s</font></caption>\n",
                Path);
        fprintf(client_sock,
                "<tr><td>name</td><td>type</td><td>owner</td><td>group</td><td>size</td><td>modify time</td></tr>\n");

        if (dir == NULL) {
            fprintf(client_sock,
                    "</table><font color=\"CC0000\" size=+2>%s</font></body></html>",
                    strerror(errno));
            return;
        }

        while ((dirent = readdir(dir)) != NULL) {
            if (strcmp(Path, "/") == 0) {
                sprintf(Filename, "/%s", dirent->d_name);
            } else {
                sprintf(Filename, "%s/%s", Path, dirent->d_name);
            }

            if (dirent->d_name[0] == '.') {
                continue;
            }

            fprintf(client_sock, "<tr>");
            len = strlen(home_dir) + strlen(Filename) + 1;
            realFilename = malloc(len + 1);
            bzero(realFilename, len + 1);
            sprintf(realFilename, "%s/%s", home_dir, Filename);

            if (stat(realFilename, &info) == 0) {
                if (strcmp(dirent->d_name, "..") == 0)
                    fprintf(client_sock, "<td><a href=\"http://%s%s%s\">(parent)</a></td>",
                            ip, atoi(port) == 80 ? "" : nport, dir_up(Path));
                else
                    fprintf(client_sock, "<td><a href=\"http://%s%s%s\">%s</a></td>",
                            ip, atoi(port) == 80 ? "" : nport, Filename, dirent->d_name);



                p_time = ctime(&info.st_mtime);
                p_passwd = getpwuid(info.st_uid);
                p_group = getgrgid(info.st_gid);

                fprintf(client_sock, "<td>%c</td>", file_type(info.st_mode));
                fprintf(client_sock, "<td>%s</td>", p_passwd->pw_name);
                fprintf(client_sock, "<td>%s</td>", p_group->gr_name);
                fprintf(client_sock, "<td>%d</td>", info.st_size);
                fprintf(client_sock, "<td>%s</td>", ctime(&info.st_ctime));
            }

            fprintf(client_sock, "</tr>\n");
            free(realFilename);
        }

        fprintf(client_sock, "</table></center></body></html>");
    } else {
        //if others,forbid access
        fprintf(client_sock,
                "HTTP/1.1 200 OK\r\nServer:Test http server\r\nConnection: close\r\n\r\n<html><head><title>permission denied</title></head>"
                "<body><font size=+4>Linux HTTP server</font><br><hr width=\"100%%\"><br><center>"
                "<table border cols=3 width=\"100%%\">");
        fprintf(client_sock,
                "</table><font color=\"CC0000\" size=+2> you access resource '%s' forbid to access,communicate with the admintor </font></body></html>",
                Path);
    }

out:
    free(realPath);
    free(nport);
}


void init_daemon(const char* pname, int facility)
{
    int pid;
    int i;
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGHUP , SIG_IGN);

    if (pid = fork()) {
        exit(EXIT_SUCCESS);
    } else if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    setsid();

    if (pid = fork()) {
        exit(EXIT_SUCCESS);
    } else if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < NOFILE; ++i) {
        close(i);
    }

    chdir("/tmp");
    umask(0);
    signal(SIGCHLD, SIG_IGN);
    openlog(pname, LOG_PID, facility);
    return;
}


int get_addr(char* str)
{
    int inet_sock;
    struct ifreq ifr;
    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
    strcpy(ifr.ifr_name, str);

    if (ioctl(inet_sock, SIOCGIFADDR, &ifr) < 0) {
        wrtinfomsg("bind");
        exit(EXIT_FAILURE);
    }

    sprintf(ip, "%s", inet_ntoa(((struct sockaddr_in*) &
                                 (ifr.ifr_addr))->sin_addr));
}

