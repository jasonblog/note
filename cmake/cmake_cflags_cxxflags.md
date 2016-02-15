# cmake CFLAGS CXXFLAGS


- CMakeLists.txt 

```sh
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_C_FLAGS "-O0 -ggdb")
set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-O0 -ggdb")
set(CMAKE_CXX_FLAGS "-O0 -ggdb")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O0 -ggdb")
```


- hello.c

```c
#include <stdio.h>

int main() {

#ifdef DEBUG
    printf("Debug run\n");
#else
    printf("Release run\n");
#endif
    return 0;
}
```

- CMakeLists.txt add `SET (CMAKE_C_FLAGS "-O0 -ggdb -D DEBUG")`

```sh
PROJECT (HELLO)
SET (HELLO_SRCS hello.c)
SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb -D DEBUG")
ADD_EXECUTABLE (hello ${HELLO_SRCS})
```

```sh
mkdir build ; cd build;
cmake ../
make 
./hello
```

or 

```sh
export CFLAGS=-DDEBUG ;
cmake ../
make 
./hello
```


or 

```
cmake ../ -DCMAKE_C_FLAGS=-DDEBUG
make 
./hello
```



跟一般用Mafefile 可以直接在 make 時候加上 CFLAGS=-DDEBUG 但是cmake 無法

```sh
CC      = gcc
CFLAGS  = -g
RM      = rm -f


default: all

all: Hello

Hello: hello.c
	$(CC) $(CFLAGS) -o Hello hello.c

clean veryclean:
	$(RM) Hello
```

```sh
make CFLAGS=-DDEBUG 
```