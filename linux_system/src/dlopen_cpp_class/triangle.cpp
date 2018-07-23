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
