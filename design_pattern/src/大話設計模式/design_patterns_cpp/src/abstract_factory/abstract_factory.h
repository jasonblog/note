#ifndef design_patterns_abstract_factory_abstract_factory_h
#define design_patterns_abstract_factory_abstract_factory_h

#include "abstract_product_a.h"
#include "abstract_product_b.h"

class AbstractFactory
{
public:
    virtual AbstractProductA* CreateProductA()
    {
        return nullptr;
    }
    virtual AbstractProductB* CreateProductB()
    {
        return nullptr;
    }
};

#endif
