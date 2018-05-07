#ifndef design_patterns_abstract_factory_product_a2_h
#define design_patterns_abstract_factory_product_a2_h

#include <iostream>
#include <string>
#include "abstract_product_a.h"
using std::string;

// DB User Table for Access
class ProductA2 : public AbstractProductA
{
public:
    void Insert(const User&)
    {
        printf("Access insert a user.\n");
    }
    void GetUser(int)
    {
        printf("Access get a user.\n");
    }
};

#endif
