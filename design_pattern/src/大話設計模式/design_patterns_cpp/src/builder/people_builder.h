#ifndef design_patterns_builder_people_builder_h
#define design_patterns_builder_people_builder_h

#include <string>
#include "people.h"

class PeopleBuilder
{
public:
    People* GetPeople() const
    {
        return people_;
    }
    virtual void MakeHead() {}
    virtual void MakeBody() {}
    virtual void MakeArms() {}
    virtual void MakeLegs() {}
protected:
    PeopleBuilder()
        : people_{new People} {}
    virtual ~PeopleBuilder() {}
    PeopleBuilder(const PeopleBuilder&) = delete;
    PeopleBuilder& operator=(const PeopleBuilder&) = delete;
protected:
    People* people_;
};

#endif
