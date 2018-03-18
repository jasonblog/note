#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char* argv[])
{
    int fd, c, res;
    struct termios oldtio, newtio;
    char buf[255];
    fd = open(argv[1], O_RDWR | O_NOCTTY);     //

    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    memset(&newtio, '\0', sizeof(newtio));
    newtio.c_cflag = B38400  | CS8 | CLOCAL | CREAD;//
    newtio.c_iflag = IGNPAR | ICRNL;//
    newtio.c_oflag = 0;        //
    newtio.c_lflag = ICANON;//
    tcflush(fd, TCIFLUSH); //
    tcsetattr(fd, TCSANOW, &newtio); //

    while (1) {
        res = read(fd, buf, 255);  //
        buf[res] = 0;              //
        printf(":recv %d bytes:%s\n\r", res, buf); //

        if (buf[0] == 'E') {       //
            break;
        }
    }

    tcsetattr(fd, TCSANOW, &oldtio);
}

