#ifndef design_patterns_strategy_strategy_h
#define design_patterns_strategy_strategy_h

class Strategy
{
public:
    Strategy() {}
    virtual ~Strategy() {}
    virtual double GetResult(int, double) const
    {
        return 0;
    }
};

#endif
