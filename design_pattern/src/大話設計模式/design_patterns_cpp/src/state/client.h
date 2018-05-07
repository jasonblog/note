#ifndef design_patterns_state_client_h
#define design_patterns_state_client_h

#include "work.h"
#include "state_morning.h"

void client()
{
    Work work(new StateMorning);
    work.set_hour(9);
    work.WriteProgram();
    work.set_hour(13);
    work.WriteProgram();
    work.set_hour(19);
    work.WriteProgram();
}

#endif
