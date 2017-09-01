#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

void print_usage(FILE* stream, int exit_code);

/* Il nome del programma  */
const char* program_name;

int main(int argc, char* argv[])
{
    int next_option;

    /* La lista con le opzioni  */
    const char* const short_options = "ho:v";

    /* L'array con le opzioni valide  */
    const struct option long_options[] = {
        {"help", 0, NULL, 'h'},
        {"output", 1, NULL, 'o'},
        {"verbose", 0, NULL, 'v'},
        {0, 0, 0, 0}
    };

    /* Il nome del file da ricevere  */
    const char* output_filename = NULL;

    int verbose = 0;

    /* Definizione del nome del programma, passato come argomento alla funzione
    main */
    program_name = argv[0];

    do {
        next_option = getopt_long(argc, argv, short_options, long_options, NULL);

        switch (next_option) {
        case 'h':
            print_usage(stdout, 0);

        /* Questa opzione ha un argomento, il nome del file di output */
        case 'o':
            output_filename = optarg;
            break;

        case 'v':
            verbose = 1;
            break;

        /* L'utente specifica un'opzione non valida  */
        case '?':
            print_usage(stderr, 1);

        /* Si esce dal programma senza aver fatto alcunche' */
        case -1:
            break;

        default:
            abort();
        }
    } while (next_option != -1);

    if (verbose) {
        int i;

        for (i = optind; i < argc; ++i) {
            printf("Argument: %s\n", argv[i]);
        }
    }

    return (EXIT_SUCCESS);
}

/* Stampa sullo stdout e in alcuni casi sullo stderr, informazioni sul coretto
utilizzo del programma  */
void print_usage(FILE* stream, int exit_code)
{
    fprintf(stream, "Usage:  %s options [ inputfile ... ]\n", program_name);
    fprintf(stream,
            "  -h  --help             Display this usage information.\n"
            "  -o  --output filename  Write output to file.\n"
            "  -v  --verbose          Print verbose messages.\n");
    exit(exit_code);
}
