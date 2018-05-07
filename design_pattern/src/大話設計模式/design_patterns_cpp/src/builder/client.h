#ifndef design_patterns_builder_client_h
#define design_patterns_builder_client_h

#include "fat_people_builder.h"
#include "slim_people_builder.h"
#include "people_director.h"

void client()
{
    FatPeopleBuilder fat;
    SlimPeopleBuilder slim;
    PeopleDirector director;
    director.MakePeople(fat);
    director.MakePeople(slim);
    People* alice = slim.GetPeople();
    People* bob   = fat.GetPeople();
    alice->set_name("alice");
    bob->set_name("bob");
    alice->Show();
    bob->Show();
    delete bob;
    delete alice;
}

#endif
