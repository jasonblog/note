#ifndef design_patterns_state_morning_h
#define design_patterns_state_morning_h

#include <iostream>
#include "work.h"
#include "state_afternoon.h"

class StateMorning : public State
{
public:
    void WriteProgram(Work* work)
    {
        if (work->hour() <= 12) {
            printf("WriteProgram at %d, StateMorning\n", work->hour());
        } else {
            work->set_state(new StateAfternoon);
            work->WriteProgram();
        }
    }
};

#endif
