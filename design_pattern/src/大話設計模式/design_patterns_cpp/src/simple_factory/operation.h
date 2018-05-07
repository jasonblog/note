#ifndef design_patterns_factory_method_operation_h
#define design_patterns_factory_method_operation_h

class Operation
{
public:
    Operation(): left_num_{0}, right_num_{0} {}
    virtual ~Operation() {}
    double left_num() const
    {
        return left_num_;
    }
    void set_left_num(double left_num)
    {
        left_num_ = left_num;
    }
    double right_num() const
    {
        return right_num_;
    }
    void set_right_num(double right_num)
    {
        right_num_ = right_num;
    }

    virtual double GetResult() const
    {
        return 0;
    }
private:
    double left_num_;
    double right_num_;
};

#endif
