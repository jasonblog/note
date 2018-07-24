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
