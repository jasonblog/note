#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <grp.h>

/*
Le funzioni di seguito elencate consentono di scandire il database dei gruppi,
solitamente collocato in /etc/group e implementato con la struttura 'group'
definita in <grp.h>.

HEADER    : <grp.h>
PROTOTYPE : struct group *getgrent(void);
            void setgrent(void);
        void endgrent(void);
SEMANTICS : La funzione getpgrent() consente di ottenere il prossimo campo del
            database dei gruppi; la funzione setgrent() apre il file relativo ai
        gruppi, se non gia' aperto, riavvolgendolo, ossia puntando al primo
        record; la funzione endgrent() chiude tutti i file ad operazioni
        avvenute.
RETURNS   : La funzione getgrent() ritorna un puntatore alla struttura group in
            caso di successo, NULL in caso di errore.
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{

    /* Il programma scrive nel file 'database' i campi di ciascun gruppo */

    struct group* grp;
    char** tmp_memb;
    FILE* fp;
    char* database = "database_group.txt";

    if ((fp = fopen(database, "w+")) == NULL) {
        fprintf(stderr, "Err.(%s): %s\n", strerror(errno), database);
        exit(EXIT_FAILURE);
    }

    setgrent();

    while ((grp = getgrent()) != NULL) {
        fprintf(fp, "%30s | ", grp->gr_name);
        fprintf(fp, "%s | ", grp->gr_passwd);
        fprintf(fp, "%5d | ", grp->gr_gid);

        tmp_memb = grp->gr_mem;

        while (*tmp_memb != NULL) {
            fprintf(fp, "%s, ", *tmp_memb);
            tmp_memb++;
        }

        fprintf(fp, "\n");
    }

    endgrent();
    fclose(fp);

    return (EXIT_SUCCESS);
}
