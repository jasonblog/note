#ifndef design_patterns_factory_method_client_h
#define design_patterns_factory_method_client_h

#include "operation_factory.h"

double calculate(double left_num, double right_num, const string& operate)
{
    OperationFactory factory;
    Operation* operation = factory.CreateOperation(operate);
    operation->set_left_num(left_num);
    operation->set_right_num(right_num);
    double result = operation->GetResult();
    delete operation;
    return result;
}

#endif
