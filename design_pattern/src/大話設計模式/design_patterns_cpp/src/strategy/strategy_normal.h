#ifndef design_patterns_strategy_strategy_normal_h
#define design_patterns_strategy_strategy_normal_h

#include "strategy.h"

class StrategyNormal : public Strategy
{
public:
    StrategyNormal() {}
    double GetResult(int num, double price) const override
    {
        return num * price;
    }
};

#endif
