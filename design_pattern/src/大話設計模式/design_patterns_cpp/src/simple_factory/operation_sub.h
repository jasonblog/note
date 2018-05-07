#ifndef design_patterns_factory_method_operation_sub_h
#define design_patterns_factory_method_operation_sub_h

#include "operation.h"

class OperationSub : public Operation
{
public:
    OperationSub() {}
    ~OperationSub() {}
    double GetResult() const override
    {
        return left_num() - right_num();
    }
};

#endif
