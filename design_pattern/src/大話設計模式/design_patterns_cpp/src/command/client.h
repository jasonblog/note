#ifndef design_patterns_command_client_h
#define design_patterns_command_client_h

#include "invoker.h"

void client()
{
    Cook cook;
    Waiter waiter;
    BakeMuttonCommand mutton1(&cook);
    BakeMuttonCommand mutton2(&cook);
    BakeChickenWingCommand chickenWing(&cook);
    waiter.AddOrder(&mutton1);
    waiter.AddOrder(&mutton2);
    waiter.AddOrder(&chickenWing);
    waiter.RemoveOrder(&mutton2);
    waiter.Notify();
}

#endif
