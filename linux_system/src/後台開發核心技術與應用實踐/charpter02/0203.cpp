struct SStudent {
public:
    void display()    //这是成员函数
    {
        cout << "num:" << num << endl;
        cout << "name:" << name << endl;
        cout << "age:" << age << endl;
    }                  //这里没有分号
private:
    int num;
    char name[20];
    int age;
};
