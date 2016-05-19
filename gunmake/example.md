# example


```sh
ARM_CC = arm-linux-gnueabi-gcc
CC = gcc
CXX = g++
LDFLAGS = -lm
CFLAGS = -Wall -pedantic -g -std=c99 -lpthread 
 
OBJS = main.o                       \
	   GyroscopeBiasEstimator.o     \
	   HeadTracker.o                \
	   HeadTransform.o              \
	   LowPassFilter.o              \
	   Matrix3x3d.o                 \
	   Matrix.o                     \
	   OrientationEKF.o             \
	   So3Util.o                    \
	   Vector3d.o                   \
	   MadgwickAHRS.o
 
main: ${OBJS}
	${CC} -o main ${OBJS} ${LDFLAGS} ${CFLAGS}
clean:
	rm -f main ${OBJS}

```


- 編譯多個執行檔

```sh
ARM_CC = arm-linux-gnueabi-gcc
CC = gcc
CXX = g++
LDFLAGS = -lm
CFLAGS = -Wall -pedantic -ggdb3 -O0
 
0505_OBJS = 0505.o
2DMI_OBJS = 2DMI.o
IFBP2D_OBJS = IFBP2D.o
IFBP_OBJS = IFBP.o
 
all: 0505 2DMI IFBP2D IFBP

0505: ${0505_OBJS}
	${CXX} -o $@ $? 

2DMI: ${2DMI_OBJS}
	${CXX} -o $@ $?

IFBP2D: ${IFBP2D_OBJS}
	${CXX} -o $@ $?

IFBP: ${IFBP2D_OBJS}
	${CXX} -o $@ $?

.SUFFIXES : .cpp
.cpp.o:
	$(CXX) $(CFLAGS) $(LDFLAGS) -c $<

clean:
	rm -f *.o 0505 2DMI IFBP2D IFBP

```


## 編譯兩個so檔案最後編譯成執行檔

- add

```c
int add(int a, int b)
{
    return a + b;
}
```

- sub.c

```c
int sub(int a, int b) 
{
    return a - b;
}
```
- main.c

```c
#include <stdio.h>

int a = 100;
int b = 200;

int add(int,int);
int sub(int,int);

int main(int argc, char* argv[])
{
    printf("add=%d\n", add(a, b));
    printf("sub=%d\n", sub(a, b));
    return 0;
}
```

- Makefile

- all: add.so sub.so main  有順序問題要先編譯出so檔案在main , 所以main擺最後

```c
CC = gcc
SOFLAGS = -shared -fPIC
LDFLAGS = -lm
CFLAGS = -Wall -pedantic -ggdb3 -O0 -std=c99 -lpthread
 
all: add.so sub.so main

main:
	${CC}  -o main  main.c -L./ -ladd -L./ -lsub ${CFLAGS} ${LDFLAGS} 

add.so: add.c
	$(CC) ${SOFLAGS} -o libadd.so $? 

sub.so: sub.c
	$(CC) ${SOFLAGS} -o libsub.so $? 

.PHONY: clean
clean:
	rm -f main *.so *.o

```