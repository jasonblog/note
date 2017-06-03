class CStudent
{
    int num;
    char name[20];
    int age;           //这些是数据成员，也称为成员变量
    void display()    //这是成员函数
    {
        cout << "num:" << num << endl;
        cout << "name:" << name << endl;
        cout << "age:" << age << endl;
    }
};
CStudent cstu1, cstu2; //定义了2个对象
