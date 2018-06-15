/*
usage: fork1
*/

#include <stdio.h>
#include <unistd.h>
int main () {
  int var = 0;
  pid_t pid;
  pid = fork ();
  printf ("%d", var);
  if (pid == 0) {
    /* child 執行 */
    fprintf(stderr, "chlid\n");
    var = 1;
  } else if (pid > 0) {
    /* parent 執行 */
    fprintf(stderr, "parent\n");
    var = 2;
  }
  printf ("%d", var);
  return 0;
}
