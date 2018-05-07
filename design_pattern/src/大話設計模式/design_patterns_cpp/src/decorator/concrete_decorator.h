#ifndef design_patterns_decorator_concrete_decorator_h
#define design_patterns_decorator_concrete_decorator_h

#include "decorator.h"

class TShirts : public Decorator
{
public:
    void Show() const
    {
        printf("TShirts ");
        Decorator::Show();
    }
};

class BigTrousers : public Decorator
{
public:
    void Show() const
    {
        printf("BigTrousers ");
        Decorator::Show();
    }
};

class Sneakers : public Decorator
{
public:
    void Show() const
    {
        printf("Sneakers ");
        Decorator::Show();
    }
};

class LeatherShoes : public Decorator
{
public:
    void Show() const
    {
        printf("LeatherShoes ");
        Decorator::Show();
    }
};

#endif
