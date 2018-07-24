CXX = g++
SOFLAGS = -shared -fPIC
LDFLAGS = -ldl
CXXFLAGS = -Wall -pedantic -ggdb3 -O0 -std=c++11 -lpthread
                                                  
all : a.out                                       

a.out: main.cpp libtriangle 
	$(CXX) $(CXXFLAGS)  main.cpp -o main $(LDFLAGS)
                                                  
libtriangle:                                            
	$(CXX) $(CXXFLAGS) $(SOFLAGS) -o triangle.so triangle.cpp
.PHONY: clean                                     
clean:                                            
	@echo "remove the main and so" 
	rm -rf  main *.so
