# 如何在運行時加載C＋＋函數和類別



另外一個問題就是dlopen函數僅支持load 函數，不支持load class。 
很明顯的是如果你想使用該class，就需要new instance出來。




C++有個特殊的關鍵字來聲明函數用C的方式來綁定——extern “C”，在C++中函數如果用extern “C”在前面聲明的話，就表示該函數的symbol以C語言方式來命名。

所以只有非成員函數可以用extern “C”來聲明，而且他們不能被重載。 
雖然很侷限，但是這樣足夠利用dlopen來運行時調用function了。需要強調的是用extern “C”不是就不可以在function內寫C++ 代碼，依然可以調用class和class的function的。

用dlsym，加載C＋＋函數就像加載C函數一樣，你要加載的函數必須要用`extern “C”`來聲明，`避免name mangling`。

-main.cpp

```cpp
#include <iostream>
#include <dlfcn.h>
#include "polygon.hpp"

int main()
{
    using std::cout;
    using std::cerr;
    // load the triangle library
    std::cout << "dlopen[+]" << std::endl;
    void* triangle = dlopen("./triangle.so", RTLD_LAZY);
    std::cout << "dlopen[-]" << std::endl;

    if (!triangle) {
        cerr << "Cannot load library: " << dlerror() << '\n';
        return 1;
    }

    // reset errors
    dlerror();
    // load the symbols
    create_t* create_triangle = (create_t*) dlsym(triangle, "create");
    const char* dlsym_error = dlerror();

    if (dlsym_error) {
        cerr << "Cannot load symbol create: " << dlsym_error << '\n';
        return 1;
    }

    destroy_t* destroy_triangle = (destroy_t*) dlsym(triangle, "destroy");
    dlsym_error = dlerror();

    if (dlsym_error) {
        cerr << "Cannot load symbol destroy: " << dlsym_error << '\n';
        return 1;
    }

    // create an instance of the class
    polygon* poly = create_triangle();
    // use the class
    poly->set_side_length(7);
    cout << "The area is: " << poly->area() << '\n';
    // destroy the class
    destroy_triangle(poly);
    // unload the triangle library
    dlclose(triangle);
}
```



- polygon.hpp

```cpp
#ifndef POLYGON_HPP
#define POLYGON_HPP
#include <iostream>

class polygon
{
protected:
    double side_length_;
public:
    polygon()
        : side_length_(0)
    {
        std::cout << "polygon Constructor" << std::endl;
    }

    virtual ~polygon()
    {
        std::cout << "polygon Destructor" << std::endl;
    }

    void set_side_length(double side_length)
    {
        side_length_ = side_length;
    }

    virtual double area() const = 0;
};
// the types of the class factories
typedef polygon* create_t();
typedef void destroy_t(polygon*);
#endif
```

- triangle.cpp

```cpp
#include <iostream>
#include <cmath>
#include "polygon.hpp"

class triangle : public polygon
{
public:
    triangle()
    {
        std::cout << "triangle Constructor" << std::endl;
    }

    ~triangle()
    {
        std::cout << "triangle Destructor" << std::endl;
    }

    virtual double area() const
    {
        return side_length_ * side_length_ * sqrt(3) / 2;
    }
};
// the class factories
extern "C" polygon* create()
{
    return new triangle;
}
extern "C" void destroy(polygon* p)
{
    delete p;
}
```

- Makefile

```sh
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
```
