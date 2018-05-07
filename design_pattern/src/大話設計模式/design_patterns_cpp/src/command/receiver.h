#ifndef design_patterns_command_receiver_h
#define design_patterns_command_receiver_h

#include <iostream>

class Cook
{
public:
    void MakeMutton()
    {
        printf("Make Mutton\n");
    }
    void MakeChickenWing()
    {
        printf("Make Chicken Wing.\n");
    }
};

#endif
