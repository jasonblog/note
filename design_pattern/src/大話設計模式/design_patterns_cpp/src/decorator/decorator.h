#ifndef design_patterns_decorator_decorator_h
#define design_patterns_decorator_decorator_h

#include "component.h"

class Decorator : public PeopleComponent
{
public:
    Decorator(): component_{nullptr} {}
    Decorator(const Decorator&) = delete;
    Decorator& operator=(const Decorator&) = delete;
    void PutOn(PeopleComponent* component)
    {
        component_ = component;
    }
    virtual void Show() const
    {
        component_->Show();
    }
protected:
    PeopleComponent* component_;
};

#endif
