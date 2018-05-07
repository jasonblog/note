#ifndef design_patterns_observer_client_h
#define design_patterns_observer_client_h

#include "concrete_subject.h"
#include "concrete_observer_a.h"
#include "concrete_observer_b.h"

void client()
{
    ConcreteSubject subject;
    ConcreteObserverA observer_a("XiaoMing", &subject);
    ConcreteObserverB observer_b("XiaoHong", &subject);
    subject.Attach(&observer_a);
    subject.Attach(&observer_b);
    subject.set_stat("lalala");
    subject.Notify();
}

#endif
