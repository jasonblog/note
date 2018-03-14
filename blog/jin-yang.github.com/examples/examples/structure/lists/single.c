无序单向链表

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct foobar {
  char *name;
  struct foobar *next;
} foobar_t;
/*
typedef struct foobar foobar_t;
struct foobar {
  foobar_t *next;
};
*/
foobar_t *foobar_list = NULL;

int main()
{
  int i = 0;
  char buffer[512];
  for (i = 0; i < 8; i++) {
    foobar_t *fb = malloc(sizeof(foobar_t));
    if(fb == NULL) {
      fprintf(stderr, "malloc() failed\n");
      return -1;
    }
    snprintf(buffer, 512, "foobar list %d\n", i);
    fb->name = strdup(buffer);
    /* insert head */
    fb->next = foobar_list;
    foobar_list = fb;
  }

  foobar_t *f;
  for (f = foobar_list; f; f = f->next) {
    printf("%s", f->name);
    free(f->name);
    free(f);
  }

  return(0);
}
