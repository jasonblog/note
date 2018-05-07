#ifndef design_patterns_factory_method_operation_add_h
#define design_patterns_factory_method_operation_add_h

#include "operation.h"

class OperationAdd : public Operation
{
public:
    OperationAdd() {}
    ~OperationAdd() {}
    double GetResult() const override
    {
        return left_num() + right_num();
    }
};

#endif
