#ifndef design_patterns_factory_method_operation_div_h
#define design_patterns_factory_method_operation_div_h

#include "operation.h"
#include <stdexcept>

class OperationDiv : public Operation
{
public:
    OperationDiv() {}
    ~OperationDiv() {}
    double GetResult() const override
    {
        if (right_num() == 0) {
            throw std::logic_error("right_num == 0");
        }

        return left_num() / right_num();
    }
};

#endif
