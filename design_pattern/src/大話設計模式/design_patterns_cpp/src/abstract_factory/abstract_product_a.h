#ifndef design_patterns_abstract_factory_abstract_product_a_h
#define design_patterns_abstract_factory_abstract_product_a_h

#include <string>
using std::string;

// DB User Table Interface
struct User {
    int id;
    string name;
    User() : id{0}, name{} {}
};

class AbstractProductA
{
public:
    virtual void Insert(const User&) {}
    virtual void GetUser(int) {}
};

#endif
