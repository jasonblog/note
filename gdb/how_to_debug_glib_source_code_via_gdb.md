# How to debug glib source code via gdb


## source code

```sh
wget http://ftp.acc.umu.se/pub/GNOME/sources/glib/2.49/glib-2.49.4.tar.xz
tar xvf glib-2.49.4.tar.xz
```

## Build

```sh
CFLAGS="-g -O0" ./configure  --prefix=/home/shihyu/gdb_test/glib/glib
make -j8 && make install
```

## glib example

```c
#include <stdio.h>
#include <glib.h>
int main()
{
    printf("Glib version: %u.%u.%u\n\n", glib_major_version, glib_minor_version,
           glib_micro_version);
    GHashTable* hash = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(hash, "Virginia", "Richmond");
    g_hash_table_insert(hash, "Texas", "Austin");
    g_hash_table_insert(hash, "Ohio", "Columbus");
    printf("There are %d keys in the hash\n", g_hash_table_size(hash));
    printf("The capital of Texas is %s\n", g_hash_table_lookup(hash, "Texas"));
    gboolean found = g_hash_table_remove(hash, "Virginia");
    printf("The value 'Virginia' was %sfound and removed\n", found ? "" : "not ");
    g_hash_table_destroy(hash);
    return 0;
}

```

```sh
gcc  -L./glib/lib/ -I./glib/include/glib-2.0/ -I ./glib/lib/glib-2.0/include/ hello.c -o hello  -lglib-2.0
```

## debug glib via gdb

```
LD_LIBRARY_PATH=/home/shihyu/gdb_test/glib/glib/lib/ cgdb ./hello
```


## gdb 

```sh
LD_LIBRARY_PATH=/home/shihyu/gdb_test/opencv/opencv/lib/ cgdb --args ./facedetect  lena.jpg
```