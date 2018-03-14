#ifndef TESTING_H
#define TESTING_H 1

#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

static int fail_count__ = 0;
static int check_count__ = 0;

#ifndef DBL_PRECISION
#define DBL_PRECISION 1e-12
#endif

#define DEF_TEST(func) static int test_##func(void)

#define RUN_TEST(func)                                                         \
  do {                                                                         \
    int status;                                                                \
    printf("Testing %s ...\n", #func);                                         \
    status = test_##func();                                                    \
    printf("%s.\n", (status == 0) ? "Success" : "FAILURE");                    \
    if (status != 0) {                                                         \
      fail_count__++;                                                          \
    }                                                                          \
  } while (0)

#define END_TEST exit((fail_count__ == 0) ? 0 : 1);

#define LOG(result, text)                                                      \
  printf("%s %i - %s\n", result ? "ok" : "not ok", ++check_count__, text)

#define OK1(cond, text)                                                        \
  do {                                                                         \
    _Bool result = (cond);                                                     \
    LOG(result, text);                                                         \
    if (!result) {                                                             \
      return -1;                                                               \
    }                                                                          \
  } while (0)
#define OK(cond) OK1(cond, #cond)

#define EXPECT_EQ_STR(expect, actual)                                          \
  do {                                                                         \
    /* Evaluate 'actual' only once. */                                         \
    const char *got__ = actual;                                                \
    if (strcmp(expect, got__) != 0) {                                          \
      printf("not ok %i - %s = \"%s\", want \"%s\"\n", ++check_count__,        \
             #actual, got__, expect);                                          \
      return (-1);                                                             \
    }                                                                          \
    printf("ok %i - %s = \"%s\"\n", ++check_count__, #actual, got__);          \
  } while (0)

#define EXPECT_EQ_INT(expect, actual)                                          \
  do {                                                                         \
    int want__ = (int)expect;                                                  \
    int got__ = (int)actual;                                                   \
    if (got__ != want__) {                                                     \
      printf("not ok %i - %s = %d, want %d\n", ++check_count__, #actual,       \
             got__, want__);                                                   \
      return (-1);                                                             \
    }                                                                          \
    printf("ok %i - %s = %d\n", ++check_count__, #actual, got__);              \
  } while (0)

#define EXPECT_EQ_UINT64(expect, actual)                                       \
  do {                                                                         \
    uint64_t want__ = (uint64_t)expect;                                        \
    uint64_t got__ = (uint64_t)actual;                                         \
    if (got__ != want__) {                                                     \
      printf("not ok %i - %s = %" PRIu64 ", want %" PRIu64 "\n",               \
             ++check_count__, #actual, got__, want__);                         \
      return (-1);                                                             \
    }                                                                          \
    printf("ok %i - %s = %" PRIu64 "\n", ++check_count__, #actual, got__);     \
  } while (0)

#define EXPECT_EQ_DOUBLE(expect, actual)                                       \
  do {                                                                         \
    double want__ = (double)expect;                                            \
    double got__ = (double)actual;                                             \
    if (isnan(want__) && !isnan(got__)) {                                      \
      printf("not ok %i - %s = %.15g, want %.15g\n", ++check_count__, #actual, \
             got__, want__);                                                   \
      return (-1);                                                             \
    } else if (!isnan(want__) && (((want__ - got__) < -DBL_PRECISION) ||       \
                                  ((want__ - got__) > DBL_PRECISION))) {       \
      printf("not ok %i - %s = %.15g, want %.15g\n", ++check_count__, #actual, \
             got__, want__);                                                   \
      return (-1);                                                             \
    }                                                                          \
    printf("ok %i - %s = %.15g\n", ++check_count__, #actual, got__);           \
  } while (0)

#define CHECK_NOT_NULL(expr)                                                   \
  do {                                                                         \
    void *ptr_;                                                                \
    ptr_ = (expr);                                                             \
    OK1(ptr_ != NULL, #expr);                                                  \
  } while (0)

#define CHECK_ZERO(expr)                                                       \
  do {                                                                         \
    long status_;                                                              \
    status_ = (long)(expr);                                                    \
    OK1(status_ == 0L, #expr);                                                 \
  } while (0)

#endif /* TESTING_H */
