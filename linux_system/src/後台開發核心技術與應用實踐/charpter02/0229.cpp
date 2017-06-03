#include <iostream>
#include <string>
using namespace std;
/*声明基类Box*/
class Box
{
public:
    Box(int, int, int); //声明构造函数
    virtual void display();//声明输出函数
protected:  //受保护成员，派生类可以访问
    int length, height, width;
};
/*Box类成员函数的实现*/
Box:: Box(int l, int h, int w) //定义构造函数
{
    length = l;
    height = h;
    width = w;
}
void Box::display() //定义输出函数
{
    cout << "length:" << length << endl;
    cout << "height:" << height << endl;
    cout << "width:" << width << endl;
}
/*声明公用派生类FilledBox*/
class FilledBox : public Box
{
public:
    FilledBox(int, int, int, int, string); //声明构造函数
    virtual void display();//虚函数
private:
    int weight;//重量
    string fruit;//装着的水果
};
/* FilledBox类成员函数的实现*/
void FilledBox :: display() //定义输出函数
{
    cout << "length:" << length << endl;
    cout << "height:" << height << endl;
    cout << "width:" << width << endl;
    cout << "weight:" << weight << endl;
    cout << "fruit:" << fruit << endl;
}
FilledBox:: FilledBox(int l, int h, int w, int we, string f) : Box(l, h, w),
    weight(we), fruit(f) {}
int main() //主函数
{
    Box box(1, 2, 3); //定义Student类对象stud1
    FilledBox fbox(2, 3, 4, 5, "apple"); //定义FilledBox类对象fbox
    Box* pt = &box;//定义指向基类对象的指针变量pt
    pt->display();
    pt = &fbox;
    pt->display();
    return 0;
}
