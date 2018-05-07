#ifndef design_patterns_observer_concrete_observer_a_h
#define design_patterns_observer_concrete_observer_a_h

#include <string>
#include <iostream>
#include "subject.h"
#include "observer.h"

using std::string;

class ConcreteObserverA : public Observer
{
public:
    ConcreteObserverA(const string& name, ConcreteSubject* subject)
        : name_{name}, subject_{subject} {}
    ~ConcreteObserverA() {}
    ConcreteObserverA(const ConcreteObserverA&) = delete;
    ConcreteObserverA& operator=(const ConcreteObserverA&) = delete;

    void Update() override
    {
        printf("ConcreteObserverA %s accept subject stat %s\n",
               name_.c_str(), subject_->stat().c_str());
    }

private:
    string name_;
    ConcreteSubject* subject_;
};

#endif
