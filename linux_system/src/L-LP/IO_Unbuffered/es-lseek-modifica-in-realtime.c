#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_LEN_FILENAME    16
#define MAX_BUF             256
#define PERMS               0755

void print_file_content(char* file_name);

/* Il programma riceve una stringa in input, tale stringa viene salvata in un
file anch'esso definito dall'utente, dopodiche' la stringa viene proposta in
output ed eventualmente modificata mediante lseek() inserendo la posizione del
byte dal quale modificare e la grandezza della nuova stringa da inserire.  */

int main(int argc, char* argv[])
{
    int fd1, i, n = 0;
    char filename[MAX_LEN_FILENAME];
    char str_default[MAX_BUF];
    char* str_temp;
    unsigned int offset;
    int len_newstr;
    char str_new[MAX_BUF];

    fputs("Please, write a default string: \n", stdout);

    // Si acsuisisce in input la stringa 'str_default' con cui lavorare
    fgets(str_default, MAX_BUF, stdin);

    // Il nome del file all'interno del quale salvare la stringa iniziale
    fputs("Please, the filename to store string: ", stdout);
    scanf("%s", filename);

    // Si salva il file pocanzi inserito in input
    if ((fd1 = open(filename, O_RDWR | O_CREAT, PERMS)) == -1) {
        fprintf(stderr, "Err. when opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // Scrittura della stringa 'str_default' nel file
    write(fd1, str_default, strlen(str_default));

    /* Si e' conclusa la fase preparatoria del programma, ora non resta che
    verificare la correttezza della stringa iniziale 'str_default' ed
    eventualmente modificarla; per far cio' e' necessario visualizzare la
    stringa carattere per carattere.

    Il ciclo seguente stampera' sullo stdout l'intera stringa, di fianco ad
    ogni carattere ci sara' il corrispettivo byte. */

    // Nei sistemi unix resetta il terminale
    system("tput reset");

    // Stampa ciascun carattere e relativo byte
    for (i = 0; i < (strlen(str_default) - 1); i++) {
        printf("byte %2d %c\n", i, str_default[i]);
    }

    printf("Tot. byte: %d\n\n", (strlen(str_default) - 1));

    fputs("From which byte do you want to modify: ", stdout);
    scanf("%u", &offset);
    fputs("How many bytes: ", stdout);
    scanf("%d", &len_newstr);
    printf("Write a %d byte string to replace: ", len_newstr);
    scanf("%s", str_new);

    /* Si alloca lo spazio necessario per contenere la stringa originale,
    dopodiche' la si copia e per finire si aggiunge la modifica */
    str_temp = calloc(strlen(str_default), sizeof(char));
    strcpy(str_temp, str_default);

    for (i = offset; i < strlen(str_default) - 1; i++) {
        str_temp[i] = str_new[n];
        n++;
    }

    // Si modifica il descrittore di riferimento fd1
    lseek(fd1, offset, SEEK_SET);

    // Si scrive partendo dall'offset definito dalla chiamata a lseek()
    write(fd1, str_new, len_newstr);
    close(fd1);

    printf("original text: %s", str_default);
    fputs("replaced text: ", stdout);
    print_file_content(filename);

    return (EXIT_SUCCESS);
}

void print_file_content(char* file_name)
{
    FILE* fp;
    int c;

    if ((fp = fopen(file_name, "r")) == NULL) {
        fprintf(stderr, "Err. opening filename %s\n", file_name);
        exit(EXIT_FAILURE);
    }

    while ((c = fgetc(fp)) != EOF) {
        fputc(c, stdout);
    }

    fclose(fp);
}
