#ifndef design_patterns_abstract_factory_product_b1_h
#define design_patterns_abstract_factory_product_b1_h

#include <iostream>
#include <string>
#include "abstract_product_b.h"
using std::string;

// DB Department Table for SQL Server
class ProductB1 : public AbstractProductB
{
public:
    void Insert(const Department&)
    {
        printf("SQL Server insert a department.\n");
    }
    void GetDepartment(int)
    {
        printf("SQL Server get a department.\n");
    }
};

#endif
