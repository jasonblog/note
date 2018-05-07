#ifndef design_patterns_observer_concrete_observer_b_h
#define design_patterns_observer_concrete_observer_b_h

#include <string>
#include <iostream>
#include "subject.h"
#include "observer.h"

using std::string;

class ConcreteObserverB : public Observer
{
public:
    ConcreteObserverB(const string& name, ConcreteSubject* subject)
        : name_{name}, subject_{subject} {}
    ~ConcreteObserverB() {}
    ConcreteObserverB(const ConcreteObserverB&) = delete;
    ConcreteObserverB& operator=(const ConcreteObserverB&) = delete;

    void Update() override
    {
        printf("ConcreteObserverB %s accept subject stat %s\n",
               name_.c_str(), subject_->stat().c_str());
    }
private:
    string name_;
    ConcreteSubject* subject_;
};

#endif
