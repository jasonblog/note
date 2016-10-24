#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>

#include <immintrin.h>

#define TEST_W 1024
#define TEST_H 1024

#include "impl.c"

static long diff_in_us(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec * 1000000.0 + diff.tv_nsec / 1000.0);
}

int main(int argc, char *argv[])
{
    /* verify the result of 4x4 matrix */
    {
        int test_src1[64] = { 0,  1,  2,  3,   0,  1,  2,  3,
                              4,  5,  6,  7,   4,  5,  6,  7,
                              8,  9, 10, 11,   8,  9, 10, 11,
                              12, 13, 14, 15, 12, 13, 14, 15,
                              0,  1,  2,  3,   0,  1,  2,  3,
                              4,  5,  6,  7,   4,  5,  6,  7,
                              8,  9, 10, 11,   8,  9, 10, 11,
                              12, 13, 14, 15, 12, 13, 14, 15,
                            };
        int test_src2[64] = { 16, 17, 18, 19, 16, 17, 18, 19,
                              20, 21, 22, 23, 20, 21, 22, 23,
                              24, 25, 26, 27, 24, 25, 26, 27,
                              28, 29, 30, 31, 28, 29, 30, 31,
                              16, 17, 18, 19, 16, 17, 18, 19,
                              20, 21, 22, 23, 20, 21, 22, 23,
                              24, 25, 26, 27, 24, 25, 26, 27,
                              28, 29, 30, 31, 28, 29, 30, 31,
                            };
        int testout[64];
        int expected[64] = {   304,  316,  328,  340,  304,  316,  328,  340,
                               1008, 1052, 1096, 1140, 1008, 1052, 1096, 1140,
                               1712, 1788, 1864, 1940, 1712, 1788, 1864, 1940,
                               2416, 2524, 2632, 2740, 2416, 2524, 2632, 2740,
                               304,  316,  328,  340,  304,  316,  328,  340,
                               1008, 1052, 1096, 1140, 1008, 1052, 1096, 1140,
                               1712, 1788, 1864, 1940, 1712, 1788, 1864, 1940,
                               2416, 2524, 2632, 2740, 2416, 2524, 2632, 2740
                           };

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++)
                printf(" %2d", test_src1[y * 8 + x]);
            printf("\n");
        }
        printf("\n");

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++)
                printf(" %2d", test_src2[y * 8 + x]);
            printf("\n");
        }
        printf("\n");

        naive_multiply(test_src1, test_src2, testout, 8, 8, 8, 8);

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++)
                printf(" %4d", testout[y * 8 + x]);
            printf("\n");
        }

        assert(0 == memcmp(testout, expected, 64 * sizeof(int)) &&
               "Verification fails");
    }

    {
        struct timespec start, end;
        int *src1 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *src2 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out1 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out2 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out3 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out4 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out5 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out6 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);

        srand(time(NULL));
        for (int i = 0; i < TEST_H; ++i) {
            for (int j = 0; j < TEST_W; ++j) {
                src1[i * TEST_W + j] = rand();
                src2[i * TEST_W + j] = rand();
            }
        }

#if defined(naive)
        clock_gettime(CLOCK_REALTIME, &start);
        naive_multiply(src1, src2, out1, TEST_W, TEST_H, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("naive: \t\t %ld us\n", diff_in_us(start, end));
#endif

#if defined(submatrix)
        clock_gettime(CLOCK_REALTIME, &start);
        submatrix_multiply(src1, src2, out2, TEST_W, TEST_H, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("submatrix: \t %ld us\n", diff_in_us(start, end));
#endif

#if defined(sse)
        clock_gettime(CLOCK_REALTIME, &start);
        sse_multiply(src1, src2, out3, TEST_W, TEST_H, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("sse: \t\t %ld us\n", diff_in_us(start, end));
#endif

#if defined(sse_prefetch)
        clock_gettime(CLOCK_REALTIME, &start);
        sse_prefetch_multiply(src1, src2, out4, TEST_W, TEST_H, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("sse_prefetch: \t %ld us\n", diff_in_us(start, end));
#endif

#if defined(avx)
        clock_gettime(CLOCK_REALTIME, &start);
        avx_multiply(src1, src2, out5, TEST_W, TEST_H, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("avx: \t\t %ld us\n", diff_in_us(start, end));
#endif

#if defined(avx_prefetch)
        clock_gettime(CLOCK_REALTIME, &start);
        avx_prefetch_multiply(src1, src2, out6, TEST_W, TEST_H, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("avx_prefetch: \t %ld us\n", diff_in_us(start, end));
#endif

        free(src1);
        free(src2);
        free(out1);
        free(out2);
        free(out3);
        free(out4);
        free(out5);
        free(out6);
    }

    return 0;
}
