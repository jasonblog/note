CPP = g++
CPPFLAGS = -g -std=c++0x
OFLAG = -o
.SUFFIXES : .o .cpp .c
.cpp.o :
	$(CPP) $(CPPFLAGS) -c $<
.c.o :
	$(CPP) $(CPPFLAGS) -c $<

PROGS = accumulate 

all: $(PROGS)

accumulate: accumulate.o 
	$(CPP) $(OFLAG) accumulate accumulate.o -lpthread

clean:
	rm -f ${PROGS} *.o

