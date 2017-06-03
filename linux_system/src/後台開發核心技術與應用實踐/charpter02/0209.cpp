#include <iostream>
using namespace std;
class Box
{
public:
    Box();//声明一个无参的构造函数
    /*声明一个有参的构造函数，并用参数的初始化列表对数据成员初始化*/
    Box(int h, int w, int l): height(h), width(w), length(l) {}
    int volume();
private:
    int height, width, length;
};
Box::Box() //定义无参的构造函数
{
    height = 1;
    width = 2;
    length = 3;
}
int Box::volume()
{
    return height * width * length;
}
int main()
{
    Box box1;//不指定实参
    cout << "box1's volume: " << box1.volume() << endl;
    Box box2(2, 5, 10); //指定实参
    cout << "box2's volume: " << box2.volume() << endl;
    return 0;
}
