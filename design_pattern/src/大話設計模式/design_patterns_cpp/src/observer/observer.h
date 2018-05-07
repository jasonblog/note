#ifndef design_patterns_observer_observer_h
#define design_patterns_observer_observer_h

#include "subject.h"

class Observer
{
public:
    virtual ~Observer() {};
    virtual void Update() {}
protected:
    Observer() {};
};

#endif
