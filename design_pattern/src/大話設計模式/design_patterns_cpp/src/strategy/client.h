#ifndef design_patterns_strategy_client_h
#define design_patterns_strategy_client_h

#include "context.h"

/*
 * strategy sample:
 * normal
 * rebate x
 * return x by y (y > x)
 * denote x by y (y > x)
 */
double calculate(int num, double price, const string& strategy)
{
    Context* context = new Context(strategy);
    double result = context->GetResult(num, price);
    delete context;
    return result;
}

#endif
