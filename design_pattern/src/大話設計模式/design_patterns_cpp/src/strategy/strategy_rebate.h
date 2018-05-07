#ifndef design_patterns_strategy_strategy_rebate_h
#define design_patterns_strategy_strategy_rebate_h

#include "strategy.h"

class StrategyRebate : public Strategy
{
public:
    StrategyRebate(double rebate): rebate_{rebate} {}
    double GetResult(int num, double price) const override
    {
        return num * price * rebate_ / 10;
    }

private:
    double rebate_;
};

#endif
