#include "../../testing.h"
#include "avltree.h"

#define STATIC_ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))

static int compare_total_count = 0;
#define RESET_COUNTS()                                                         \
  do {                                                                         \
    compare_total_count = 0;                                                   \
  } while (0)

static int compare_string(void const *v0, void const *v1) {
  assert(v0 != NULL);
  assert(v1 != NULL);

  compare_total_count++;
  return (strcmp(v0, v1));
}

DEF_TEST(string) {
  struct {
    char *key;
    char *value;
  } cases[] = {
      {"Eeph7chu", "vai1reiV"}, {"igh3Paiz", "teegh1Ee"},
      {"caip6Uu8", "ooteQu8n"}, {"Aech6vah", "AijeeT0l"},
      {"Xah0et2L", "gah8Taep"}, {"BocaeB8n", "oGaig8io"},
      {"thai8AhM", "ohjeFo3f"}, {"ohth6ieC", "hoo8ieWo"},
      {"aej7Woow", "phahuC2s"}, {"Hai8ier2", "Yie6eimi"},
      {"phuXi3Li", "JaiF7ieb"}, {"Shaig5ef", "aihi5Zai"},
      {"voh6Aith", "Oozaeto0"}, {"zaiP5kie", "seep5veM"},
      {"pae7ba7D", "chie8Ojo"}, {"Gou2ril3", "ouVoo0ha"},
      {"lo3Thee3", "ahDu4Zuj"}, {"Rah8kohv", "ieShoc7E"},
      {"ieN5engi", "Aevou1ah"}, {"ooTe4OhP", "aingai5Y"},
  };

  c_avl_tree_t *t;

  RESET_COUNTS();
  CHECK_NOT_NULL(t = c_avl_create(compare_string));

  /* insert */
  for (size_t i = 0; i < STATIC_ARRAY_SIZE(cases); i++) {
    char *key;
    char *value;

    CHECK_NOT_NULL(key = strdup(cases[i].key));
    CHECK_NOT_NULL(value = strdup(cases[i].value));

    CHECK_ZERO(c_avl_insert(t, key, value));
    EXPECT_EQ_INT((int)(i + 1), c_avl_size(t));
  }

  /* Key already exists. */
  for (size_t i = 0; i < STATIC_ARRAY_SIZE(cases); i++)
    EXPECT_EQ_INT(1, c_avl_insert(t, cases[i].key, cases[i].value));

  /* get */
  for (size_t i = 0; i < STATIC_ARRAY_SIZE(cases); i++) {
    char *value_ret = NULL;

    CHECK_ZERO(c_avl_get(t, cases[i].key, (void *)&value_ret));
    EXPECT_EQ_STR(cases[i].value, value_ret);
  }

  /* iterator */
  int count = 0;
  c_avl_iterator_t *iter;
  char *key = NULL;
  char *value = NULL;
  CHECK_NOT_NULL(iter = c_avl_get_iterator(t));
  while (c_avl_iterator_next(iter, (void *)&key, (void *)&value) == 0) {
    ++count;
  }
  EXPECT_EQ_INT(count, c_avl_size(t));
  while (c_avl_iterator_prev(iter, (void *)&key, (void *)&value) == 0) {
    ++count;
  }
  EXPECT_EQ_INT(count + 1, 2 * c_avl_size(t));
  c_avl_iterator_destroy(iter);

  /* remove half */
  for (size_t i = 0; i < STATIC_ARRAY_SIZE(cases) / 2; i++) {
    char *key = NULL;
    char *value = NULL;

    int expected_size = (int)(STATIC_ARRAY_SIZE(cases) - (i + 1));

    CHECK_ZERO(c_avl_remove(t, cases[i].key, (void *)&key, (void *)&value));

    EXPECT_EQ_STR(cases[i].key, key);
    EXPECT_EQ_STR(cases[i].value, value);

    free(key);
    free(value);

    EXPECT_EQ_INT(expected_size, c_avl_size(t));
  }

  /* pick the other half */
  for (size_t i = STATIC_ARRAY_SIZE(cases) / 2; i < STATIC_ARRAY_SIZE(cases);
       i++) {
    char *key = NULL;
    char *value = NULL;

    int expected_size = (int)(STATIC_ARRAY_SIZE(cases) - (i + 1));

    EXPECT_EQ_INT(expected_size + 1, c_avl_size(t));
    EXPECT_EQ_INT(0, c_avl_pick(t, (void *)&key, (void *)&value));

    free(key);
    free(value);

    EXPECT_EQ_INT(expected_size, c_avl_size(t));
  }




  c_avl_destroy(t);

  return (0);
}

static int compare_int(void const *v0, void const *v1) {
  assert(v0 != NULL);
  assert(v1 != NULL);

  compare_total_count++;
  return (*(int *)v0 - *(int *)v1);
}

DEF_TEST(int) {
  struct {
    int   key;
    char *value;
  } cases[] = {
      {20, "vai1reiV"}, {84, "teegh1Ee"},
      {52, "ooteQu8n"}, {38, "AijeeT0l"},
      {22, "gah8Taep"}, {89, "oGaig8io"},
      {14, "ohjeFo3f"}, {63, "hoo8ieWo"},
      {27, "phahuC2s"}, {42, "Yie6eimi"},
      {93, "JaiF7ieb"}, {70, "aihi5Zai"},
      {71, "Oozaeto0"}, {19, "seep5veM"},
      {92, "chie8Ojo"}, {56, "ouVoo0ha"},
      {57, "ahDu4Zuj"}, {95, "ieShoc7E"},
      {76, "Aevou1ah"}, {55, "aingai5Y"},
  };

  c_avl_tree_t *t;

  RESET_COUNTS();
  CHECK_NOT_NULL(t = c_avl_create(compare_int));

  /* insert */
  for (size_t i = 0; i < STATIC_ARRAY_SIZE(cases); i++) {
    char *value;

    CHECK_NOT_NULL(value = strdup(cases[i].value));

    CHECK_ZERO(c_avl_insert(t, &cases[i].key, value));
    EXPECT_EQ_INT((int)(i + 1), c_avl_size(t));
  }

  /* Key already exists. */
  for (size_t i = 0; i < STATIC_ARRAY_SIZE(cases); i++)
    EXPECT_EQ_INT(1, c_avl_insert(t, &cases[i].key, cases[i].value));

  /* get */
  for (size_t i = 0; i < STATIC_ARRAY_SIZE(cases); i++) {
    char *value_ret = NULL;

    CHECK_ZERO(c_avl_get(t, &cases[i].key, (void *)&value_ret));
    EXPECT_EQ_STR(cases[i].value, value_ret);
  }

  /* remove half */
  for (size_t i = 0; i < STATIC_ARRAY_SIZE(cases) / 2; i++) {
    int  *key = NULL;
    char *value = NULL;

    int expected_size = (int)(STATIC_ARRAY_SIZE(cases) - (i + 1));

    CHECK_ZERO(c_avl_remove(t, &cases[i].key, (void *)&key, (void *)&value));

    EXPECT_EQ_INT(cases[i].key, *key);
    EXPECT_EQ_STR(cases[i].value, value);

    free(value);

    EXPECT_EQ_INT(expected_size, c_avl_size(t));
  }

  /* pick the other half */
  for (size_t i = STATIC_ARRAY_SIZE(cases) / 2; i < STATIC_ARRAY_SIZE(cases);
       i++) {
    int  *key = NULL;
    char *value = NULL;

    int expected_size = (int)(STATIC_ARRAY_SIZE(cases) - (i + 1));

    EXPECT_EQ_INT(expected_size + 1, c_avl_size(t));
    EXPECT_EQ_INT(0, c_avl_pick(t, (void *)&key, (void *)&value));

    free(value);

    EXPECT_EQ_INT(expected_size, c_avl_size(t));
  }

  c_avl_destroy(t);

  return (0);
}

int main(void) {
  RUN_TEST(string);
  RUN_TEST(int);

  END_TEST;
}

/* vim: set sw=2 sts=2 et : */
