#ifndef design_patterns_proxy_real_h
#define design_patterns_proxy_real_h

#include <iostream>
#include "common_interface.h"

class Real : public CommonInterface
{
public:
    virtual ~Real() {}
    void Request() override
    {
        printf("Real Request.\n");
    }
};
#endif
