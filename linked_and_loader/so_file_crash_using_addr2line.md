# so file crash  using addr2line


- tdl.c

```c
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

void print();

int main()
{
    print();

    return 0;
}
```

## 故意 crash 

- d2.c 

```c
#include <stdio.h>

int p = 3;
void print()
{
    int *p = NULL;
    *p = 10;
    printf("This is the second dll src!\n");
}
```

- Makefile

```sh
export  LD_LIBRARY_PATH=LD_LIBRARY_PATH:./libs/

CC = gcc

all: dl.so 
	$(CC) -o main tdl.c -L./libs -ldl 
	#$(CC) -o main tdl.c -L./libs -ldl -Wl,-rpath=/home/shihyu/dll/libs 
	#$(CC) -o main tdl.c ./libdl.so 
dl.so:
	$(CC) -O -fPIC -shared -o libs/libdl.so d2.c 
       
.PHONY: clean
clean:                             
	@rm -rf *.o *.so main    
```

```sh
dmesg | grep main
 
[ 3047.860204] main[8667]: segfault at 0 ip 00007fa1a7fcf6b4 sp 00007ffee91c0890 error 6 in libdl.so[7fa1a7fcf000+1000]
```


```sh
env LD_DEBUG=files LD_DEBUG_OUTPUT=myld.log ./main
```

- myld.log.8667

```sh
      8667:	
      8667:	file=libdl.so [0];  needed by ./main [0]
      8667:	file=libdl.so [0];  generating link map
      8667:	  dynamic: 0x00007fa1a81cfe18  base: 0x00007fa1a7fcf000   size: 0x0000000000201030
      8667:	    entry: 0x00007fa1a7fcf5b0  phdr: 0x00007fa1a7fcf040  phnum:                  7
      8667:	
      8667:	
      8667:	file=libc.so.6 [0];  needed by ./main [0]
      8667:	file=libc.so.6 [0];  generating link map
      8667:	  dynamic: 0x00007fa1a7fc8ba0  base: 0x00007fa1a7c06000   size: 0x00000000003c89a0
      8667:	    entry: 0x00007fa1a7c26950  phdr: 0x00007fa1a7c06040  phnum:                 10
      8667:	
      8667:	
      8667:	calling init: /lib/x86_64-linux-gnu/libc.so.6
      8667:	
      8667:	
      8667:	calling init: ./libs/libdl.so
      8667:	
      8667:	
      8667:	initialize program: ./main
      8667:	
      8667:	
      8667:	transferring control: ./main
      8667:	
```

## 00007fa1a7fcf6b4 (crash ip) -  0x00007fa1a7fcf000 (libdl.so base address)

```py
hex (0x00007fa1a7fcf6b4  - 0x00007fa1a7fcf000) = 0x6b4
```


```sh
addr2line -e libs/libdl.so 0x6b4  -f


print
??:?
```