#include<iostream>
using namespace std;
class CBox
{
public:
    CBox(int h, int w, int l)
    {
        height = h;
        width = w;
        length = l;
        cout << "Constructor called." << endl; //构造函数被执行时输出信息
    }
    ~CBox()  //析构函数
    {
        cout << "Destructor called." << length << endl; //析构函数被执行时输出
    }
    int volume()
    {
        return height * width * length;
    }
private:
    int height, width, length;
};
int main()
{
    CBox box1(1, 2, 3);
    cout << box1.volume() << endl;
    CBox box2(2, 3, 4);
    cout << box2.volume() << endl;
    return 0;
}
