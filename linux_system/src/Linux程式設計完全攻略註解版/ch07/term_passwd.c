#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<termios.h>
#define PASSWD_LEN 8
char* getpasswd(char* prompt)
{
    FILE* fp = NULL;

    if (NULL == (fp = fopen(ctermid(NULL), "r+"))) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", ctermid(NULL));
    setvbuf(fp, (char*) NULL, _IONBF, 0);
    sigset_t myset, setsave;
    sigemptyset(&myset);
    sigaddset(&myset, SIGINT);
    sigaddset(&myset, SIGTSTP);
    sigprocmask(SIG_BLOCK, &myset, &setsave);

    struct termios termnew, termsave;
    tcgetattr(fileno(fp), &termsave);
    termnew = termsave;
    termnew.c_lflag = termnew.c_lflag & ~(ECHO | ECHOCTL | ECHOE | ECHOK);

    tcsetattr(fileno(fp), TCSAFLUSH, &termnew);

    fputs(prompt, fp);
    static char buf[PASSWD_LEN + 1];
    int c;
    char* ptr = buf;

    while ((c = getc(fp)) != EOF && c != '\0' && c != '\n' && c != '\r') {
        if (ptr < &buf[PASSWD_LEN]) {
            *ptr++ = c;
        }

        fflush(fp);
    }

    *ptr = '\0';
    putc('\n', fp);
    tcsetattr(fileno(fp), TCSAFLUSH, &termsave);
    sigprocmask(SIG_BLOCK, &setsave, NULL);
    return buf;
}
int main(void)
{
    char* ptr = NULL;
    ptr = getpasswd("#");
    printf("%s\n", ptr);
}


