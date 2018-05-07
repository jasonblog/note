#ifndef design_patterns_factory_method_operation_mul_h
#define design_patterns_factory_method_operation_mul_h

#include "operation.h"

class OperationMul : public Operation
{
public:
    OperationMul() {}
    ~OperationMul() {}
    double GetResult() const override
    {
        return left_num() * right_num();
    }
};

#endif
