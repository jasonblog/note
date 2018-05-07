#ifndef design_patterns_builder_people_director_h
#define design_patterns_builder_people_director_h

#include "people_builder.h"

class PeopleDirector
{
public:
    void MakePeople(PeopleBuilder& people_builder)
    {
        people_builder.MakeHead();
        people_builder.MakeBody();
        people_builder.MakeArms();
        people_builder.MakeLegs();
    }
};

#endif
