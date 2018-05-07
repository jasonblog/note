#ifndef design_patterns_observer_concrete_subject_h
#define design_patterns_observer_concrete_subject_h

#include <string>
#include "subject.h"
#include "observer.h"

using std::string;

class ConcreteSubject : public Subject
{
public:
    ConcreteSubject(): stat_{} {}
    void set_stat(const string& stat)
    {
        stat_ = stat;
    }
    string stat() const
    {
        return stat_;
    }
private:
    string stat_;
};


#endif
