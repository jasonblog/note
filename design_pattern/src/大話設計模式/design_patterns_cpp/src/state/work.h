#ifndef design_patterns_state_work_h
#define design_patterns_state_work_h

#include "state.h"

class State;

class Work
{
public:
    Work(State* state): hour_{0}, state_{state} {}
    ~Work()
    {
        delete state_;
    }
    Work(const Work&) = delete;
    Work& operator=(const Work&) = delete;

    int hour() const
    {
        return hour_;
    }
    void set_hour(int hour)
    {
        hour_ = hour;
    }
    void set_state(State* state)
    {
        delete state_;
        state_ = state;
    }
    void WriteProgram()
    {
        state_->WriteProgram(this);
    }
private:
    int hour_;
    State* state_;
};

#endif
