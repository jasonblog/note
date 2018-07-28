# C++ 通過虛函數表，直接外部訪問類中的函數，並且脫離類


```cpp
#include <iostream>
class CStu
{
    virtual void go1()
    {
        std::cout << "go1" << std::endl;
    }
    virtual void go2()
    {
        std::cout << "go2" << std::endl;
    }
    virtual void go3()
    {
        std::cout << "go3" << std::endl;
    }
};

class B
{
public:
    B()
    {
        a = 100;
    }

private:
    int a;
};

int main()
{
    B b;

    std::cout << "int a=" << *((int*)&b) << std::endl; 

    CStu* p = new CStu;
    // p指针里面存放的数据是虚函数表的二级指针
    //*(int *)p  p占4个字节，强转成int * 后 解引用，取出二级指针地址
    //(int **)(*(int *)p)将二级指针地址由void **转为 int **
    //(int **)(*(int *)p) + n  相当于(int **)(*(int *)p)[n]，取出里面的元素指针
    // *((int **)(*(int *)p) + n) 解该引用，取出指针里面存放的地址
    //(void *)(*((int **)(*(int *)p) + n))将取出的地址值强转为void* 型，
    //外面赋值给函数指针，即可调用类里面的虚函数。
    std::cout << sizeof(p) << std::endl;
    std::cout << sizeof(long) << std::endl;
    std::cout << sizeof(long long) << std::endl;
    std::cout << *(unsigned long*)p << std::endl;
    std::cout << *(long long*)p << std::endl;
    std::cout << *((long**)(*(long*)p) + 0) << std::endl;
    std::cout << *((long**)(*(long*)p) + 1) << std::endl;
    std::cout << *((long**)(*(long*)p) + 2) << std::endl;
    typedef  void (*fun)();

    for (int i = 0; i < 3; i++) {
        fun f1 = (fun)((void*)(*((long**)(*(long*)p) + i)));
        f1();
    }

    return 0;
}
```