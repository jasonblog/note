#ifndef design_patterns_decorator_component_h
#define design_patterns_decorator_component_h

#include <string>
#include <iostream>
using std::string;

class PeopleComponent
{
public:
    PeopleComponent(): name_{"x-man"} {}
    PeopleComponent(const string& name): name_{name} {}

    virtual void Show() const
    {
        printf("on %s\n", name_.c_str());
    }

private:
    string name_;
};

#endif
