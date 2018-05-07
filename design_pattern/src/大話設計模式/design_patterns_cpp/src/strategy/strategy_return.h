#ifndef design_patterns_strategy_strategy_return_h
#define design_patterns_strategy_strategy_return_h

#include "strategy.h"

class StrategyReturn : public Strategy
{
public:
    StrategyReturn(double full, double back)
        : full_{full}, back_{back} {}

    double GetResult(int num, double price) const override
    {
        double total = num * price;

        if (total >= full_) {
            total = total - back_;
        }

        return (total > 0) ? total : 0;
    }
private:
    double full_;
    double back_;
};

#endif
