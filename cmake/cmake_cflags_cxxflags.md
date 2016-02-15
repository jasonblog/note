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
SET (CMAKE_C_FLAGS "-O0 -ggdb -D DEBUG")
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

