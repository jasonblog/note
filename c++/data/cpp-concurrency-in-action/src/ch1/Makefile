CPP = g++
CPPFLAGS = -g -std=c++0x
OFLAG = -o
.SUFFIXES : .o .cpp .c
.cpp.o :
	$(CPP) $(CPPFLAGS) -c $<
.c.o :
	$(CPP) $(CPPFLAGS) -c $<

PROGS = hello

all: $(PROGS)

hello: hello.o 
	$(CPP) $(OFLAG) hello hello.o

clean:
	rm -f ${PROGS} *.o

