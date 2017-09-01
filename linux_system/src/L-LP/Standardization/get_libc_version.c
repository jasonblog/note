#include <stdio.h>
#include <stdlib.h>
#include <powerpc-linux-gnu/gnu/libc-version.h>

/* La libreria in questione, se trovata, fornira' la
versione della librera GNU C.

Nel caso specifico e' stata utilizzata una macchina con
processore PPC, come si evince chiaramante dal path. */

int main(void)
{
    const char* version = gnu_get_libc_version();

    printf("%s\n", version);

    return (EXIT_SUCCESS);
}
