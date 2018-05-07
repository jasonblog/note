#ifndef design_patterns_builder_fat_people_builder_h
#define design_patterns_builder_fat_people_builder_h

#include "people_builder.h"
#include "people.h"

class FatPeopleBuilder : public PeopleBuilder
{
public:
    FatPeopleBuilder() {}

    void MakeHead() override
    {
        people_->AddPart("Fat head");
    }
    void MakeBody() override
    {
        people_->AddPart("Fat body");
    }
    void MakeArms() override
    {
        people_->AddPart("Fat arms");
    }
    void MakeLegs() override
    {
        people_->AddPart("Fat legs");
    }
};

#endif
