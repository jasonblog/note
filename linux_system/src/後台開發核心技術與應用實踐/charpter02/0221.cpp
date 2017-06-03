class Operation_int
{
public:
    Operation_int(int a, int b): x(a), y(b) {}
    int add()
    {
        return x + y;
    }
    int subtract()
    {
        return x - y;
    }
private:
    int x, y;
};
