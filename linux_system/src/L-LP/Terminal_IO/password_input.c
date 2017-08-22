#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#define MAX_PASS 51

// Prototipe
int get_password(char buf[], int maxlen, int star);

/* The program's purpose is to read a password in input, each character is
replaced by a '*'.
*/

int main(void)
{
    char    pass[MAX_PASS];
    int     size;

    fputs("Please type your password: ", stdout);
    size = get_password(pass, MAX_PASS, '*');
    printf("\n'%s'(%d byte)\n", pass, size);

    return (EXIT_SUCCESS);
}

int get_password(char buf[], int maxlen, int star)
{
    struct termios ttystate, orig;
    int c;
    int pos = 0;

    tcgetattr(0, &ttystate);
    orig = ttystate;
    ttystate.c_lflag &= ~ICANON;
    ttystate.c_lflag &= ~ECHO;
    ttystate.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &ttystate);

    while ((c = getchar()) != '\n' && c != '\r') {
        if (pos < maxlen - 1) {
            buf[pos++] = c ;
            putchar(star);
        }
    }

    buf[pos] = '\0';
    tcsetattr(0, TCSANOW, &orig);

    return pos;
}
