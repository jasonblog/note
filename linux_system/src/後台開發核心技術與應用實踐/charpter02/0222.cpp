class Operation_double
{
public:
    Operation_double(double a, double b): x(a), y(b) {}
    double add()
    {
        return x + y;
    }
    double subtract()
    {
        return x - y;
    }
private:
    double x, y;
};
