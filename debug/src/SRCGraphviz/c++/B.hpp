#ifndef _B_H_
#define _B_H_
#include "C.hpp"

struct B {
    C c;
    B() {}
    int Bone()
    {
        BTwo();
        c.CTwo();
        return 1;
    }

    int BTwo()
    {

        BThree();
        return 1;
    }

    int BThree()
    {
        return 1;
    }
};
#endif // _B_H_
