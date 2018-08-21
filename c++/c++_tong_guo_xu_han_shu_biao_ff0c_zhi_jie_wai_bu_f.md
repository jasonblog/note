# C++ 通過虛函數表，直接外部訪問類中的函數，並且脫離類


```cpp
#include <iostream>
class CStu
{
public:
    CStu()
    {
        c = 10;
        d = 20;
        e = 30;
        f = 50;
    }
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
    // int a;
    int c;
    int d;
    int e;
    int f;
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
	CStu c;

    std::cout << "vtable address: " <<  ((long**)(*(long*)&c) + 0) << std::endl;
    std::cout << *((long**)(*(long*)&c) + 0) << std::endl;
    std::cout << *((long**)(*(long*)&c) + 1) << std::endl;
    std::cout << *((long**)(*(long*)&c) + 2) << std::endl;

    std::cout << (unsigned long)&c.c - (unsigned long)&c << std::endl;
    std::cout << (unsigned long)&c.d - (unsigned long)&c.c << std::endl;
    std::cout << (unsigned long)&c.e - (unsigned long)&c.d << std::endl;
    std::cout << (unsigned long)&c.f - (unsigned long)&c.e << std::endl;

	std::cout << "c address:" << &c << std::endl; 
	std::cout << "c.c address:" << &c.c << std::endl; 
	std::cout << "c.d address:" << &c.d << std::endl; 
	std::cout << "c.e address:" << &c.e << std::endl; 
	std::cout << "c.f address:" << &c.f << std::endl; 

	std::cout << "CStu size:" << sizeof(c) << std::endl; 
	std::cout << "c address:" << &c << std::endl; 

	std::cout << "c.c address:" << ((long*)&c + 1) << std::endl; 
    // c + 8 byte
    // ((long*)&c + 1) offset 8 byte
    // *((int*) (c + 8byte 後的address))  在抓取4byte
	std::cout << "c.c value:" << *((int*)((long*)&c + 1)) << std::endl; 

    // c + 12 byte
	std::cout << "c.d address:" << ((int*)&c + 3) << std::endl; 
	std::cout << "c.d value:" << *((int*)((int*)&c + 3)) << std::endl; 

    // c + 16 byte
	std::cout << "c.e address:" << ((int*)&c + 4) << std::endl; 
	std::cout << "c.e value:" << *((int*)((int*)&c + 4)) << std::endl; 

    // c + 20 byte
	std::cout << "c.f address:" << ((int*)&c + 5) << std::endl; 
	std::cout << "c.f value:" << *((int*)((int*)&c + 5)) << std::endl; 

    

    std::cout << "int a=" << *((int*)&b) << std::endl;

    CStu* p = new CStu;
    // p指針裡面存放的數據是虛函數表的二級指針
    //*(int *)p  p佔4個字節，強轉成int * 後 解引用，取出二級指針地址
    //(int **)(*(int *)p)將二級指針地址由void **轉為 int **
    //(int **)(*(int *)p) + n  相當於(int **)(*(int *)p)[n]，取出裡面的元素指針
    // *((int **)(*(int *)p) + n) 解該引用，取出指針裡面存放的地址
    //(void *)(*((int **)(*(int *)p) + n))將取出的地址值強轉為void* 型，
    //外面賦值給函數指針，即可調用類裡面的虛函數。
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