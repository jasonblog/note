#include <stdio.h>

struct X {
  void foo(int) {
    printf("in X.foo[%p]\n", &X::foo);
  }
};

void foo() { printf("in foo[%p]\n", foo); }

void bar() {
    printf("in bar[%p]\n", bar);
    foo();
}

int main() {
    printf("in main[%p]\n", main);
    bar();
    X x;
    x.foo(13);
    return 0;
}
