#ifndef design_patterns_chain_of_responsiblity_client_h
#define design_patterns_chain_of_responsiblity_client_h

#include "handler.h"

void client()
{
    ConcreteHandler1 h1;
    ConcreteHandler2 h2;
    ConcreteHandler3 h3;
    h1.set_successor(&h2);
    h2.set_successor(&h3);

    Request request1(1, "request 1");
    Request request2(2, "request 2");
    Request request3(3, "request 3");

    h1.HandleRequest(request1);
    h1.HandleRequest(request2);
    h1.HandleRequest(request3);
}

#endif
