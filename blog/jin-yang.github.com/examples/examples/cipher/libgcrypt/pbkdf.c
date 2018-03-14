#include <stdio.h>
#include <stdlib.h>
#include <gcrypt.h>


#define HASH_BYTES 512/8
int main(int argc, char **argv)
{
    char outbuf[HASH_BYTES];
    char *salt = "12345678", *sym_key = "1234567890";
    int index;
    gcry_error_t gcry_ret;

    gcry_ret = gcry_kdf_derive(sym_key, strlen(sym_key),
            GCRY_KDF_PBKDF2, GCRY_MD_SHA512,
            salt, strlen(salt), 10000,
            HASH_BYTES, outbuf);
    if (gcry_ret) {
        printf("gcry_kdf_derive failed:  %s/%s\n",
                gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));
        return -1;
    }
    printf("Hash key      = ");
    for (index = 0; index < HASH_BYTES; index++)
        printf("%02X", (unsigned char)outbuf[index]);
    printf("\n");


    return 0;
}
