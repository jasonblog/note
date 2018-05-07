#ifndef design_patterns_abstract_factory_product_a1_h
#define design_patterns_abstract_factory_product_a1_h

#include <iostream>
#include <string>
#include "abstract_product_a.h"
using std::string;

// DB User Table for SQL Server

class ProductA1 : public AbstractProductA
{
public:
    void Insert(const User&)
    {
        printf("SQL Server insert a user.\n");
    }
    void GetUser(int)
    {
        printf("SQL Server get a user.\n");
    }
};

#endif
