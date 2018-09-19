CPP = g++
CPPFLAGS = -g -std=c++0x
OFLAG = -o
.SUFFIXES : .o .cpp .c
.cpp.o :
	$(CPP) $(CPPFLAGS) -c $<
.c.o :
	$(CPP) $(CPPFLAGS) -c $<

PROGS = stack hierarchical

all: $(PROGS)

stack: stack.o 
	$(CPP) $(OFLAG) stack stack.o -lpthread

hierarchical: hierarchical.o 
	$(CPP) $(OFLAG) hierarchical hierarchical.o -lpthread

clean:
	rm -f ${PROGS} *.o

