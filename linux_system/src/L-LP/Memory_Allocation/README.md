### How to compile program

With `MAP_ANON` and many other flags (*man mmap*) will be necessary `_GNU_SOURCE` 
(*feature test macros*) to compile the program:

$ `gcc -std=c11 -Wall -pedantic -D_GNU_SOURCE prog.c`

$ `clang -std=c11 -Wall -pedantic -D_GNU_SOURCE prog.c`
