#ifndef design_patterns_observer_subject_h
#define design_patterns_observer_subject_h

#include <string>
#include <list>
#include "observer.h"
using std::list;
using std::string;

class Subject
{
public:
    virtual ~Subject() {};

    virtual void Attach(Observer* observer)
    {
        observers_.push_back(observer);
    }
    virtual void Detach(Observer* observer)
    {
        observers_.remove(observer);
    }
    virtual void Notify()
    {
        for (auto observer : observers_) {
            observer->Update();
        }
    }
protected:
    Subject(): observers_{} {}
private:
    list<Observer*> observers_;
};

#endif
