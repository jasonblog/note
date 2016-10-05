# Template in C


```c
#include <stdio.h>

#define printf_dec_format(x) _Generic((x), \
                                      char: "%c", \
                                      signed char: "%hhd", \
                                      unsigned char: "%hhu", \
                                      signed short: "%hd", \
                                      unsigned short: "%hd", \
                                      signed int: "%d", \
                                      unsigned int: "%u", \
                                      unsigned long int: "%ld", \
                                      long long int: "%lld", \
                                      unsigned long long int: "%llu", \
                                      float: "%f", \
                                      double: "%lf", \
                                      long double: "%Lf", \
                                      char *: "%s", \
                                      void *: "%p")

#define print(x)    printf( printf_dec_format(x), x)
#define printnl(x)  printf( printf_dec_format(x), x); printf("\n")

int main(int argc, char* argv)
{
    printnl('a');
    printnl((char) 'a');
    printnl(123);
    printnl(1.234);
}
```