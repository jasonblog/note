#ifndef design_patterns_state_afternoon_h
#define design_patterns_state_afternoon_h

#include <iostream>
#include "work.h"
#include "state_evening.h"

class StateAfternoon : public State
{
public:
    void WriteProgram(Work* work)
    {
        if (work->hour() <= 18) {
            printf("WriteProgram at %d, StateAfternoon\n", work->hour());
        } else {
            work->set_state(new StateEvening);
            work->WriteProgram();
        }
    }
};

#endif
