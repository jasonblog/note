#ifndef _C_H_
#define _C_H_

struct C {

    C() {}
    int Cone()
    {
        CTwo();
        CThree();
        return 1;
    }

    int CTwo()
    {

        CThree();
        return 1;
    }

    int CThree()
    {
        return 1;
    }
};
#endif // _C_H_
