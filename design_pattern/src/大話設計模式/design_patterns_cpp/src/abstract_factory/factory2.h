#ifndef design_patterns_abstract_factory_factory2_h
#define design_patterns_abstract_factory_factory2_h

#include "abstract_factory.h"
#include "product_a2.h"
#include "product_b2.h"

class Factory2 : public AbstractFactory
{
public:
    AbstractProductA* CreateProductA()
    {
        return new ProductA2;
    }
    AbstractProductB* CreateProductB()
    {
        return new ProductB2;
    }
};

#endif
