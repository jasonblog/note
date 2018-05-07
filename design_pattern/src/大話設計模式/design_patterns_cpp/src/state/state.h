#ifndef design_patterns_state_state_h
#define design_patterns_state_state_h

#include "work.h"

class Work;

class State
{
public:
    virtual ~State() {}
    virtual void WriteProgram(Work*) {}
};

#endif
