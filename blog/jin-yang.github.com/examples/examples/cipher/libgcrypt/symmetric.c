
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gcrypt.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#if 0
if (!gcry_control (GCRYCTL_ANY_INITIALIZATION_P))
{
    gcry_check_version(NULL); /* before calling any other functions */
    gcry_control (GCRYCTL_SET_THREAD_CBS, &gcry_threads_pthread);
    gcry_control (GCRYCTL_INIT_SECMEM, 32768, 0);
    gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
}
#endif

static void aes_test(int gcry_mode, char * init_vector)
{
    #define GCRY_CIPHER GCRY_CIPHER_AES256   // Pick the cipher here

    gcry_error_t     gcry_ret;
    gcry_cipher_hd_t cipher_hd;
    size_t           index;

   /* http://lists.gnupg.org/pipermail/gcrypt-devel/2003-August/000458.html
    * Because you can't know in a library whether another library has
    * already initialized the library
    */
    if (!gcry_control (GCRYCTL_ANY_INITIALIZATION_P)) {
        gcry_check_version(NULL); /* before calling any other functions */
    }

    printf("gcry_mode     = %s\n", gcry_mode == GCRY_CIPHER_MODE_ECB ? "ECB" : "CBC");

    gcry_ret = gcry_cipher_open(
        &cipher_hd,    // gcry_cipher_hd_t *hd
        GCRY_CIPHER,   // int algo
        gcry_mode,     // int mode
        0);            // unsigned int flags
    if (gcry_ret) {
        printf("gcry_cipher_open failed:  %s/%s\n",
                gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));
        return;
    }

    size_t key_length = gcry_cipher_get_algo_keylen(GCRY_CIPHER);
    char * sym_key = "one test AES key, just for test now"; // larger than 16 bytes
    printf("key_length    = %zd\n", key_length);
    printf("sym_key       = \"%s\"\n    strlen=%zd\n", sym_key, strlen(sym_key));
    gcry_ret = gcry_cipher_setkey(cipher_hd, sym_key, key_length);
    if (gcry_ret) {
        printf("gcry_cipher_setkey failed:  %s/%s\n",
                gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));
        return;
    }

    size_t blk_length = gcry_cipher_get_algo_blklen(GCRY_CIPHER);
    printf("blk_length    = %zd\n", blk_length);
    printf("init_vector   = \"%s\"\n    strlen=%zd\n", init_vector, strlen(init_vector));
    gcry_ret = gcry_cipher_setiv(cipher_hd, init_vector, blk_length);
    if (gcry_ret) {
        printf("gcry_cipher_setiv failed:  %s/%s\n",
                gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));
        return;
    }

    char * plaintxt = "123456789 abcdefghijklmnopqrstuvwzyz ABCDEFGHIJKLMNOPQRSTUVWZYZ";
    size_t plaintxt_length = strlen(plaintxt) + 1; // string plus termination
    char * encypted_txt = malloc(plaintxt_length);
    printf("plaintxt      = \"%s\"\n    length=%zd\n", plaintxt, plaintxt_length);
    gcry_ret = gcry_cipher_encrypt(
        cipher_hd,         // gcry_cipher_hd_t h
        encypted_txt,      // unsigned char *out
        plaintxt_length,   // size_t outsize
        plaintxt,          // const unsigned char *in
        plaintxt_length);  // size_t inlen
    if (gcry_ret) {
        printf("gcry_cipher_encrypt failed:  %s/%s\n",
                gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));
        return;
    }
    printf("encypted_txt  = ");
    for (index = 0; index < plaintxt_length; index++)
        printf("%02X", (unsigned char)encypted_txt[index]);
    printf("\n");

    char * decrpyted_txt = malloc(plaintxt_length);
    gcry_ret = gcry_cipher_setiv(cipher_hd, init_vector, blk_length);
    if (gcry_ret) {
        printf("gcry_cipher_setiv failed:  %s/%s\n",
                gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));
        return;
    }
    gcry_ret = gcry_cipher_decrypt(
        cipher_hd,          // gcry_cipher_hd_t h
        decrpyted_txt,      // unsigned char *out
        plaintxt_length,    // size_t outsize
        encypted_txt,       // const unsigned char *in
        plaintxt_length);   // size_t inlen
    if (gcry_ret) {
        printf("gcry_cipher_decrypt failed:  %s/%s\n",
                gcry_strsource(gcry_ret), gcry_strerror(gcry_ret));
        return;
    }
    printf("decrpyted_txt = %s\n", decrpyted_txt);

    // clean up
    gcry_cipher_close(cipher_hd);
    free(encypted_txt);
    free(decrpyted_txt);
}

int main(void)
{
    aes_test(GCRY_CIPHER_MODE_ECB, "a test ini value");
    aes_test(GCRY_CIPHER_MODE_ECB, "different value!");
    aes_test(GCRY_CIPHER_MODE_CBC, "a test ini value");
    aes_test(GCRY_CIPHER_MODE_CBC, "different value!");
}
