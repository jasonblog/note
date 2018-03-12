#include <stdio.h>
#include <stdlib.h>
#include <gcrypt.h>

void str_hash(void)
{
    // $ echo -n "123456789 abcdefghijklmnopqrstuvwzyz ABCDEFGHIJKLMNOPQRSTUVWZYZ" | sha1sum
    // d2b1b6fb8364cbab255cc7afd5608b424a3e6334 -
    char *plaintxt = "123456789 abcdefghijklmnopqrstuvwzyz ABCDEFGHIJKLMNOPQRSTUVWZYZ";
    printf("Plain txt length: %zd\n", strlen(plaintxt));

    int hash_len = gcry_md_get_algo_dlen( GCRY_MD_SHA1 );
    printf("SHA1 hash length: %d\n", hash_len);
    char *hash = malloc(hash_len);
    gcry_md_hash_buffer(GCRY_MD_SHA1, hash, plaintxt, strlen(plaintxt));

    int i;
    for ( i = 0; i < hash_len; i++ )
        printf ( "%02x", (unsigned char)hash[i] );
    puts("");
    free(hash);
}

void file_hash(void)
{
    // $ sha1sum hash_test.txt
    // 09c34ad626c2e089eede68e046b9f7e7eca3c32e  hash_test.txt
    unsigned char buffer[4096], *hash, *ptr;
    size_t len;

    FILE *fd = fopen("hash_test.txt", "rb");
    if (fd == NULL) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    int hash_len = gcry_md_get_algo_dlen( GCRY_MD_SHA1 );
    printf("SHA1 hash length: %d\n", hash_len);
    hash = malloc(hash_len);

    gcry_md_hd_t cipher_hd;
    gcry_error_t cipher_err;
    cipher_err = gcry_md_open(&cipher_hd, GCRY_MD_SHA1, 0);
    if (cipher_err) {
        fprintf (stderr, "Failure: %s/%s\n",
        gcry_strsource (cipher_err), gcry_strerror (cipher_err));
        return;
    }
    gcry_md_reset(cipher_hd);

    while ((len = fread(buffer, 1, 4096, fd)) > 0)
        gcry_md_write(cipher_hd, buffer, len);
    fclose(fd);

    ptr = gcry_md_read(cipher_hd, GCRY_MD_SHA1);
    memcpy(hash, ptr, 20);

    gcry_md_close(cipher_hd);

    int i;
    for ( i = 0; i < hash_len; i++ )
        printf ( "%02x", (unsigned char)hash[i] );
    puts("");
    free( hash );
}

int main(int argc, char **argv)
{
    str_hash();
    file_hash();
    return 0;
}
