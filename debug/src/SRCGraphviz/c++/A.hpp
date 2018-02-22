#ifndef _A_H_
#define _A_H_
#include "B.hpp"
#include "C.hpp"

struct A {
    B b;

    A() {}
    int Aone()
    {
        ATwo();
        b.BTwo();
        return 1;
    }

    int ATwo()
    {

        AThree();
        return 1;
    }

    int AThree()
    {
        return 1;
    }
};
#endif // _A_H_
