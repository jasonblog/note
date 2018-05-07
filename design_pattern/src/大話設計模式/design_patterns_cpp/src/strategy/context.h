#ifndef design_patterns_strategy_context_h
#define design_patterns_strategy_context_h

#include <string>
#include "strategy_donate.h"
#include "strategy_normal.h"
#include "strategy_rebate.h"
#include "strategy_return.h"
using std::string;

class Context
{
public:
    Context(const string& str_strategy) : strategy{nullptr}
    {
        switch (str_strategy[0]) {
        case 'd': {
                int full = 0;
                int donate = 0;
                sscanf(str_strategy.c_str(), "denote %d by %d", &donate, &full);
                strategy = new StrategyDonate(full, donate);
                break;
            }

        case 'n':
            strategy = new StrategyNormal;
            break;

        case 'r':
            switch (str_strategy[2]) {
            case 'b': { // rebate
                    double rebate = 0;
                    sscanf(str_strategy.c_str(), "rebate %lf", &rebate);
                    strategy = new StrategyRebate(rebate);
                    break;
                }

            case 't': { // return
                    double full = 0;
                    double back = 0;
                    sscanf(str_strategy.c_str(), "return %lf by %lf", &back, &full);
                    strategy = new StrategyReturn(full, back);
                    break;
                }

            default:
                throw std::logic_error("not find strategy");
            }

            break;

        default:
            throw std::logic_error("not find strategy");
        }
    }
    ~Context()
    {
        if (strategy) {
            delete strategy;
        }
    }

    double GetResult(int num, double price) const
    {
        return strategy->GetResult(num, price);
    }
private:
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
private:
    Strategy* strategy;
};

#endif
