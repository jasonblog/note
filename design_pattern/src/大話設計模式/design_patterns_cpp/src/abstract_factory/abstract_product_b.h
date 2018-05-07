#ifndef design_patterns_abstract_factory_abstract_product_b_h
#define design_patterns_abstract_factory_abstract_product_b_h

#include <string>
using std::string;

// DB Department Table Interface
struct Department {
    int id;
    string name;
    Department() : id{0}, name{} {}
};

class AbstractProductB
{
public:
    virtual void Insert(const Department&) {}
    virtual void GetDepartment(int) {}
};

#endif
