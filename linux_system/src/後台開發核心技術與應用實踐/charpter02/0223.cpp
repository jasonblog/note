template<class T>//声明一个模板，虚拟类型名为T
class Operation
{
public:
    Operation(T a, T b): x(a), y(b) {}
    T add()
    {
        return x + y;
    }
    T subtract()
    {
        return x - y;
    }
private:
    T x, y;
};
