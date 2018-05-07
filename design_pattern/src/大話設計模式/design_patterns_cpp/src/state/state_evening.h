#ifndef design_patterns_state_evening_h
#define design_patterns_state_evening_h

#include <iostream>
#include "work.h"

class StateEvening : public State
{
public:
    void WriteProgram(Work* work)
    {
        printf("WriteProgram at %d, StateEvening\n", work->hour());
    }
};

#endif
