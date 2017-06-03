class Time
{
public:
    Time() //这就是构造函数
    {
        hour = 0;
        minute = 0;
        second = 0;
    }
    set_time();
    get_time();
private:
    int hour, minute, second;
};
