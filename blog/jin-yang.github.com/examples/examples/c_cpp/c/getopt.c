#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int opterr, optopt;

int main(int argc, char *argv[]) {
    int want_opt[10] = {0, };  /* want option? */
    char ch;                   /* service variables */
    int i;
    char *my_argument;
    opterr = 4;

	printf("argc = %d\n", argc);
	for(i = 0; i <= argc; i++){
		printf("argv[%d] = %s, ", i, argv[i]);
	}
	printf("\n");

    /* optstring: a, b, c, h; and x taking an argument */
    while ((ch = getopt(argc, argv, "ab:c::d::h")) != -1) {     /* getopt() iterates over argv[] */
        switch (ch) {       /* what getopt() returned */
            case 'a':
                want_opt[0]++;
                break;
            case 'b':
                want_opt[1]++;
                break;
            case 'c':
                want_opt[2]++;
                my_argument = optarg;/* preserve the pointer to c' argument */
                break;
            case 'd':
                want_opt[3]++;
                break;
            case 'h':   /* want help */
            default:
                /* no options recognized: print options summary */
                printf("Usage:\n%s [-a] [-b] [-c] [-h] [-x]\n", argv[0]);

                exit(EXIT_FAILURE);
        }
    }

    /* print out results */
    printf("You requested:\n");
    if (want_opt[0]) printf("a [%d]\n", want_opt[0]);
    if (want_opt[1]) printf("b [%d]\n", want_opt[1]);
    if (want_opt[2]) printf("c [%d], %s\n", want_opt[2], my_argument);
    if (want_opt[3]) printf("d [%d]\n", want_opt[3]);
    printf("\n");

	printf("argc = %d\n", argc);
	for(i = 0; i <= argc; i++){
		printf("argv[%d] = %s, ", i, argv[i]);
	}
	printf("\n");

    return 0;
}

