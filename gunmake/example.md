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
CFLAGS = -Wall -pedantic -ggdb3 -O0 -std=c99 -lpthread -static
 
0505 = 0505.o
2DMI = 2DMI.o
IFBP2D = IFBP2D.o
IFBP = IFBP.o
 
all: 0505 2DMI IFBP2D IFBP

0505: ${0505}
	${CXX} -o $@ $? ${LDFLAGS} ${CFLAGS}

2DMI: ${2DMI}
	${CXX} -o $@ $? ${LDFLAGS} ${CFLAGS}

IFBP2D: ${IFBP2D}
	${CXX} -o $@ $? ${LDFLAGS} ${CFLAGS}

IFBP: ${IFBP}
	${CXX} -o $@ $? ${LDFLAGS} ${CFLAGS}

clean:
	rm -f *.o 0505 2DMI IFBP2D IFBP

```