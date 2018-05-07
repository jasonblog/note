#ifndef design_patterns_abstract_factory_product_b2_h
#define design_patterns_abstract_factory_product_b2_h

#include <iostream>
#include <string>
#include "abstract_product_b.h"
using std::string;

// DB Department Table for SQL Server
class ProductB2 : public AbstractProductB
{
public:
    void Insert(const Department&)
    {
        printf("Access insert a department.\n");
    }
    void GetDepartment(int)
    {
        printf("Access get a department.\n");
    }
};

#endif
