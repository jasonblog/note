#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>

// How to get the highest value of time_t variable in hex.
int main(void)
{
    time_t highest_value = 0x7FFFFFFF;

    char* str_datec = ctime(&highest_value);

    if (str_datec == NULL) {
        fprintf(stderr, "Err. ctime() failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("  Highest value (ctime): %s", str_datec);

    // Correct technique
    char* str_datea = asctime(gmtime(&highest_value));

    if (str_datea == NULL) {
        fprintf(stderr, "Err. asctime() failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Highest value (asctime): %s\n", str_datea);

    return (EXIT_SUCCESS);
}
