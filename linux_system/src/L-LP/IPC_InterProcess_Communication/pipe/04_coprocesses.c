#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAXBUF 1024

/* Un filtro UNIX e' un processo che legge dallo standard input e scrive sullo
standard output, di solito i filtri sono connessi linearmente mediante una
shell pipeline; pertanto se lo stesso processo legge dallo standard input e
scrive sullo standard output si sta parlando di coprocesso. */

/* Un processo crea due pipe, una e' lo standard input del coprocesso, l'altra
e' lo standard output del coprocesso; il programma e' un coprocesso che legge
due numeri dal proprio standard input, si sommano, e la somma viene scritta nel
proprio standard output. */

int main(void)
{
    int n, num1, num2;
    char buf[MAXBUF];

    while ((n = read(stdin, buf, MAXBUF)) > 0) {
        buf[n] = 0;     /* terminazione nulla */

        if (sscanf(buf, "%d%d", &num1, &num2) == 2) {
            sprintf(buf, "%d\n", num1 + num2);     /* somma dei due interi */
            n = strlen(buf);

            if (write(stdout, buf, n) != n) {
                fprintf(stderr, "Err. write() - %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        } else {
            if (write(stdout, "invalid args\n", 13) != 13) {
                fprintf(stderr, "Err. write() - %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }

    return (EXIT_SUCCESS);
}
