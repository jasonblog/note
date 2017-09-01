#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <shadow.h>

/*
Nel file /etc/passwd, come si puo' facilmente notare, il secondo campo relativo
alla password e' criptato, tale file oltre alla password stessa contiene diverse
informazioni relative agli account del sistema; i file di riferimento sono:
- /etc/shadow ;
- /etc/gshadow ;
- /etc/shadow- (backup).

Il file /etc/shadow non e' "world readable", pertanto solo 'root' puo' leggerlo
e modificarlo, contiene generalmente 9 campi:
1 - user login name;
2 - encrypted password;
3 - date of last password change;
4 - minimum password age;
5 - maximum password age;
6 - password warning period;
7 - password inactivity period;
8 - account expiration date;
9 - reserved field.

Ciascuno dei 9 campi e' definito nella struttura 'spwd' in <shadow.h>:

struct spwd
{
    char *sp_namp;
    char *sp_pwdp;
    long int sp_lstchg;
    long int sp_min;
    long int sp_max;
    long int sp_warn;
    long int sp_inact;
    long int sp_expire;
    unsigned long int sp_flag;
};

Vi sono diverse funzioni che consentono di interagine con le shadow password,
tra cui:

HEADER    : <shadow.h>
PROTOTYPE : struct spwd *getspnam(const char *name);
            struct spwd *getspent(void);
        void setspent(void);
        void endspent(void);
SEMANTICS : La funzione getspnam() ritorna un puntatore alla struttura
            contenente il campo relativo alla corrispondenza 'name'; la funzione
        getspent() ritorna un puntatore alla prossima voce dello shadow
        password database; la funzione setspent() riavvolge lo shadow
        password database; la funzione endspent() chiude lo shadow password
        database dopo che tutte le operazioni si sono cosnluse.
RETURNS   : Un puntatore alla struttura spwd in caso di successo, NULL in caso
            di errore
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    off_t who_am_i = getuid();
    struct spwd* sp, *sp2;
    char* user = "nobody";

    if (who_am_i != 0) {
        fprintf(stderr, "Only root can read shadow password database baby!\n");
        exit(EXIT_FAILURE);
    }

    if ((sp = getspnam("b3h3m0th")) == NULL) {
        fprintf(stderr, "Err. %s reading gestpnam()\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("     user name: %s\n", sp->sp_namp);
    printf("encrypted pass: %s\n", sp->sp_pwdp);

    setspent();

    while ((sp2 = getspent()) != NULL) {
        if (strcmp(user, sp2->sp_namp) == 0) {
            printf("User %s presente nel sistema\n", user);
            break;
        }

        printf("Searching...\n");
    }

    endspent();

    return (EXIT_SUCCESS);
}
