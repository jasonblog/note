class Time
{
public:
    Time();//对构造函数进行声明
    set_time();
    get_time();
private:
    int hour, minute, second;
};
Time::Time() //定义构造函数，需要加上类名和域限定符"::"
{
    hour = 0;
    minute = 0;
    second = 0;
}
