#include<iostream>
using namespace std;
#define pi 3.1415
class Circle
{
public:
    Circle(int r);//形参列表
    double Area();
private:
    int radius;//数据成员
};
Circle::Circle(int r)
{
    radius = r;
}
double Circle::Area()
{
    return pi * radius * radius;
}
int main()
{
    Circle cir1(10);
    cout << "cir1's area: " << cir1.Area() << endl;
    Circle cir2(1);
    cout << "cir2's area: " << cir2.Area() << endl;
    return 0;
}
