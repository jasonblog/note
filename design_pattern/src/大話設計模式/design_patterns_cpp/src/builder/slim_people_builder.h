#ifndef design_patterns_builder_slim_people_builder_h
#define design_patterns_builder_slim_people_builder_h

#include "people_builder.h"
#include "people.h"

class SlimPeopleBuilder : public PeopleBuilder
{
public:
    SlimPeopleBuilder() {}

    void MakeHead() override
    {
        people_->AddPart("Slim head");
    }
    void MakeBody() override
    {
        people_->AddPart("Slim body");
    }
    void MakeArms() override
    {
        people_->AddPart("Slim arms");
    }
    void MakeLegs() override
    {
        people_->AddPart("Slim legs");
    }
};

#endif
