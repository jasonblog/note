#CXX_FLAGS= -O0
all: main.o trace.o
	clang++ ${CXX_FLAGS} main.o trace.o -o prog
main.o: main.cpp
	clang++ ${CXX_FLAGS} -finstrument-functions -c main.cpp -o main.o
trace.o: trace.cpp
	clang++ -c trace.cpp -o trace.o
