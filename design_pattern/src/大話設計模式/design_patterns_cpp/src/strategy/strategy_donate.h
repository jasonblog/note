#ifndef design_patterns_strategy_strategy_donate_h
#define design_patterns_strategy_strategy_donate_h

#include "strategy.h"

class StrategyDonate : public Strategy
{
public:
    StrategyDonate(int full, int donate)
        : full_{full}, donate_{donate} {}

    double GetResult(int num, double price) const override
    {
        double total = 0;

        if (num >= full_) {
            total = (num - donate_) * price;
        } else {
            total = num * price;
        }

        return (total > 0) ? total : 0;
    }
private:
    int full_;
    int donate_;
};

#endif
