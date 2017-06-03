test: test.o tree.o
	g++ -g -o test test.o tree.o
tree.o:tree.cpp tree.h
	g++ -g -c tree.cpp -o tree.o
test.o:test.cpp
	g++ -g -c test.cpp -o test.o
