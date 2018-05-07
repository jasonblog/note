#ifndef design_patterns_proxy_proxy_h
#define design_patterns_proxy_proxy_h

#include <iostream>
#include "real.h"

class Proxy : public CommonInterface
{
public:
    Proxy() : real_{nullptr} {}
    ~Proxy()
    {
        if (real_) {
            delete real_;
        }
    }
    Proxy(const Proxy&) = delete;
    Proxy& operator=(const Proxy&) = delete;
    void Request() override
    {
        if (!real_) {
            real_ = new Real;
        }

        real_->Request();
    }
private:
    Real* real_;
};

#endif
