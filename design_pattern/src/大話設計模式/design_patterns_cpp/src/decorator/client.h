#ifndef design_patterns_decorator_client_h
#define design_patterns_decorator_client_h

#include "concrete_decorator.h"

void show_finery()
{
    printf("Show finery:\n");
    PeopleComponent jack("Jack");
    Sneakers sneakers;
    BigTrousers bigTrousers;
    TShirts tshirts;
    sneakers.PutOn(&jack);
    bigTrousers.PutOn(&sneakers);
    tshirts.PutOn(&bigTrousers);
    tshirts.Show();
}

#endif
