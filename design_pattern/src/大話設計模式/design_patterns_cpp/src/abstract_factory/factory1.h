#ifndef design_patterns_abstract_factory_factory1_h
#define design_patterns_abstract_factory_factory1_h

#include "abstract_factory.h"
#include "product_a1.h"
#include "product_b1.h"

class Factory1 : public AbstractFactory
{
public:
    AbstractProductA* CreateProductA()
    {
        return new ProductA1;
    }
    AbstractProductB* CreateProductB()
    {
        return new ProductB1;
    }
};

#endif
