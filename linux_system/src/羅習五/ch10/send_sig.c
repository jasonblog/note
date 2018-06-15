#include <assert.h>
#include <signal.h>
#include <stdio.h>

int main() {
  int pid, signum, times;
  int i;
  printf("process ID\n");
  scanf("%d", &pid);
  printf("signal number\n");
  scanf("%d", &signum);
  printf("times\n");
  scanf("%d", &times);
  for (i = 0; i < times; i++)
    assert(kill(pid, signum) == 0);
}
